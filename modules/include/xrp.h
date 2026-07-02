#pragma once
#include <ren/FileHandle.hpp>
#include <string>

namespace xrp
{
	bool init();
	void shutdown();
	bool isGood();
	const char* getLastErrorMessage();

	int fileCount();
	std::wstring getNameW(int _index);
	std::string getNameA(int _index);
	int getSize(int _index);

	int findName(const std::wstring& _name);
	int findName(const std::string& _name);

	size_t iNameCount(const std::wstring& _name);
	size_t iNameCount(const std::string& _name);

	int iFindName(const std::wstring& _name, size_t _element);
	int iFindName(const std::string& _name, size_t _element);

	ren::FileInputHandle* getHandle(const std::wstring& _name);
	ren::FileInputHandle* getHandle(const std::string& _name);
	ren::FileInputHandle* getHandle(int _index);
}