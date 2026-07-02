cmake_minimum_required(VERSION 3.16)

project("LAVE" VERSION 2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

find_package(wxWidgets REQUIRED COMPONENTS core base)
if (wxWidgets_USE_FILE) # not defined in CONFIG mode
	include(${wxWidgets_USE_FILE})
endif()

add_library(Stringtools INTERFACE)

target_include_directories(Stringtools
	INTERFACE
		${PROJECT_SOURCE_DIR}/modules/include
)

add_library(Exceptions INTERFACE)

target_include_directories(Exceptions 
	INTERFACE
		${PROJECT_SOURCE_DIR}/modules/include
)

add_library(Math INTERFACE)

target_include_directories(Math
	INTERFACE
		${PROJECT_SOURCE_DIR}/modules/include	
)

add_library(FileHandle INTERFACE)

target_include_directories(FileHandle 
	INTERFACE
		${PROJECT_SOURCE_DIR}/modules/include
)

target_link_libraries(FileHandle
	INTERFACE
		Math
)

add_library(xrp_def INTERFACE)
target_include_directories(xrp_def
	INTERFACE
		${PROJECT_SOURCE_DIR}/modules/include
)

add_library(ArchiveBase STATIC
	${PROJECT_SOURCE_DIR}/modules/include/ren/ArchiveLinear.h
	${PROJECT_SOURCE_DIR}/modules/src/ArchiveLinear.cpp
	${PROJECT_SOURCE_DIR}/modules/include/ren/ArchiveNamedEntries.h
	${PROJECT_SOURCE_DIR}/modules/src/ArchiveNamedEntries.cpp
)

target_include_directories(ArchiveBase
	PUBLIC
		${PROJECT_SOURCE_DIR}/modules/include
)

target_link_libraries(ArchiveBase
	PUBLIC
		Exceptions
		FileHandle
		Stringtools
)

add_library(xrp_mgr STATIC
	${PROJECT_SOURCE_DIR}/modules/include/xrp.h
	${PROJECT_SOURCE_DIR}/modules/include/xrp_cp.h
	${PROJECT_SOURCE_DIR}/modules/include/ren/ArchiveXRP.h
	${PROJECT_SOURCE_DIR}/modules/src/xrp.cpp
	${PROJECT_SOURCE_DIR}/modules/src/xrp_cp.cpp
	${PROJECT_SOURCE_DIR}/modules/src/ArchiveXRP.cpp
)

target_include_directories(xrp_mgr
	PUBLIC
		${PROJECT_SOURCE_DIR}/modules/include
)

target_link_libraries(xrp_mgr
	PUBLIC
		ArchiveBase
		Stringtools
		Exceptions
)

add_library(ArchiveInst STATIC
	${PROJECT_SOURCE_DIR}/modules/include/ren/ArchiveRCS.h
	${PROJECT_SOURCE_DIR}/modules/src/ArchiveRCS.cpp
	${PROJECT_SOURCE_DIR}/modules/include/ren/ArchiveGTA3.h
	${PROJECT_SOURCE_DIR}/modules/src/ArchiveGTA3.cpp
)

target_include_directories(ArchiveInst
	PUBLIC
		${PROJECT_SOURCE_DIR}/modules/include	
)

target_link_libraries(ArchiveInst
	PUBLIC
		ArchiveBase
)

add_executable(xrp_bldr
	${PROJECT_SOURCE_DIR}/src/xpf-respacker.cpp
)

target_link_libraries(xrp_bldr
	PRIVATE
		Stringtools
		xrp_def
)

add_executable(LAVE2 WIN32
	${PROJECT_SOURCE_DIR}/include/LaveApp.h
	${PROJECT_SOURCE_DIR}/src/LaveApp.cpp
	${PROJECT_SOURCE_DIR}/include/ArchiverFrame.h
	${PROJECT_SOURCE_DIR}/src/ArchiverFrame.cpp
	${PROJECT_SOURCE_DIR}/include/AboutDialog.h
	${PROJECT_SOURCE_DIR}/src/AboutDialog.cpp
	${PROJECT_SOURCE_DIR}/include/ImageView.h
	${PROJECT_SOURCE_DIR}/src/ImageView.cpp
	${PROJECT_SOURCE_DIR}/include/FileInputHandleWxStream.h
	${PROJECT_SOURCE_DIR}/src/FileInputHandleWxStream.cpp
	${PROJECT_SOURCE_DIR}/LAVE.rc
)

target_include_directories(LAVE2
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(LAVE2
	PRIVATE
		${wxWidgets_LIBRARIES}
		ArchiveInst
		xrp_mgr
)

set(FINAL_DIR ${CMAKE_BINARY_DIR}/final)
file(MAKE_DIRECTORY ${FINAL_DIR})

set(FINAL_APP ${FINAL_DIR}/LAVE2.exe)
set(XRP_SCRIPT ${PROJECT_SOURCE_DIR}/res/packfile.txt)

add_custom_command(
	OUTPUT ${FINAL_APP}

	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:LAVE2> ${FINAL_APP}

	COMMAND $<TARGET_FILE:xrp_bldr> ${XRP_SCRIPT} ${FINAL_APP}

	DEPENDS LAVE2 xrp_bldr

	COMMENT "Creating final executable with resources attached."
	VERBATIM
)

add_custom_target(LAVE2_final ALL
	DEPENDS ${FINAL_APP})