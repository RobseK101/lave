#pragma once

#if defined _WIN32
#define PLATFORM_WINDOWS
#elif defined __GNUC__ && __unix__
#define PLATFORM_LINUX
#endif

#include <string>

#ifdef PLATFORM_WINDOWS
std::wstring xrpGetExePath();
#elif defined PLATFORM_LINUX
std::string xrpGetExePath();
#else 
#error Unsupported platform
#endif