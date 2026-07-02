#include "xrp_cp.h"

#define PATHBUFFER_LENGTH 1024

#ifdef PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

std::wstring xrpGetExePath()
{
	wchar_t buffer[PATHBUFFER_LENGTH] = {};

	GetModuleFileNameW(NULL, buffer, PATHBUFFER_LENGTH);
	return std::wstring(buffer);
}

#elif defined PLATFORM_LINUX

#include <unistd.h>

std::string xrpGetExePath()
{
	char buffer[PATHBUFFER_LENGTH] = {};

	readlink("/proc/self/exe", buffer, PATHBUFFER_LENGTH - 1);
	return std::string(buffer);
}

#endif