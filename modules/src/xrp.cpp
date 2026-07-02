#include "xrp.h"
#include <ren/ArchiveXRP.h>
#include <ren/FileHandle.hpp>
#include "xrp_cp.h"

#define XRP_MSG_NO_ARCHIVE "No resource archive has been loaded."

namespace xrp
{
	ren::ArchiveXRP* resArchive = nullptr;
	std::string lastErrorMessage;

	bool init()
	{
		if (resArchive) {
			return false;
		}

		auto exeName = xrpGetExePath();
		ren::CFileInputHandle* archiveFile = new ren::CFileInputHandle(exeName.c_str());
		if (!archiveFile->isGood()) {
			delete archiveFile;
			return false;
		}

		ren::ArchiveXRP* archive = nullptr;
		try {
			archive = new ren::ArchiveXRP(archiveFile, true);
		}
		catch (const std::exception& e) {
			lastErrorMessage = e.what();
			return false;
		}
		if (!archive->isGood()) {
			delete archive;
			lastErrorMessage = "Unspecified defect in the resource archive.";
			return false;
		}
		resArchive = archive;
		return true;
	}

	void shutdown()
	{
		delete resArchive;
		resArchive = nullptr;
	}

	bool isGood()
	{
		return resArchive;
	}

	const char* getLastErrorMessage()
	{
		return lastErrorMessage.c_str();
	}

	int fileCount()
	{
		if (resArchive == nullptr) {
			lastErrorMessage = XRP_MSG_NO_ARCHIVE;
			return 0;
		}

		return resArchive->getFilecount();
	}

	std::wstring getNameW(int _index)
	{
		if (resArchive == nullptr) {
			lastErrorMessage = XRP_MSG_NO_ARCHIVE;
			return {};
		}

		return resArchive->getNameW(_index);
	}

	std::string getNameA(int _index)
	{
		if (resArchive == nullptr) {
			lastErrorMessage = XRP_MSG_NO_ARCHIVE;
			return {};
		}

		return resArchive->getNameA(_index);
	}

	int getSize(int _index)
	{
		if (resArchive == nullptr) {
			lastErrorMessage = XRP_MSG_NO_ARCHIVE;
			return 0;
		}

		return resArchive->getSize(_index);
	}

	int findName(const std::wstring& _name)
	{
		if (resArchive == nullptr) {
			lastErrorMessage = XRP_MSG_NO_ARCHIVE;
			return -1;
		}

		return resArchive->findName(_name);
	}

	int findName(const std::string& _name)
	{
		if (resArchive == nullptr) {
			lastErrorMessage = XRP_MSG_NO_ARCHIVE;
			return -1;
		}

		return resArchive->findName(_name);
	}

	size_t iNameCount(const std::wstring& _name)
	{
		if (resArchive == nullptr) {
			lastErrorMessage = XRP_MSG_NO_ARCHIVE;
			return 0;
		}

		resArchive->iNameCount(_name);
	}

	size_t iNameCount(const std::string& _name)
	{
		if (resArchive == nullptr) {
			lastErrorMessage = XRP_MSG_NO_ARCHIVE;
			return 0;
		}

		resArchive->iNameCount(_name);
	}

	int iFindName(const std::wstring& _name, size_t _element)
	{
		if (resArchive == nullptr) {
			lastErrorMessage = XRP_MSG_NO_ARCHIVE;
			return -1;
		}

		resArchive->iFindName(_name, _element);
	}

	int iFindName(const std::string& _name, size_t _element)
	{
		if (resArchive == nullptr) {
			lastErrorMessage = XRP_MSG_NO_ARCHIVE;
			return -1;
		}

		resArchive->iFindName(_name, _element);
	}

	ren::FileInputHandle* getHandle(const std::wstring& _name)
	{
		if (resArchive == nullptr) {
			lastErrorMessage = XRP_MSG_NO_ARCHIVE;
			return nullptr;
		}

		try {
			return resArchive->getHandle(_name);
		}
		catch (const std::exception& e) {
			lastErrorMessage = e.what();
			return nullptr;
		}
	}

	ren::FileInputHandle* getHandle(const std::string& _name)
	{
		if (resArchive == nullptr) {
			lastErrorMessage = XRP_MSG_NO_ARCHIVE;
			return nullptr;
		}

		try {
			return resArchive->getHandle(_name);
		}
		catch (const std::exception& e) {
			lastErrorMessage = e.what();
			return nullptr;
		}
	}

	ren::FileInputHandle* getHandle(int _index)
	{
		if (resArchive == nullptr) {
			lastErrorMessage = XRP_MSG_NO_ARCHIVE;
			return nullptr;
		}

		return resArchive->getHandle(_index);
	}
}