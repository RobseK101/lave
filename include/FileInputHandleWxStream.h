#pragma once
#include <wx/stream.h>
#include <ren/FileHandle.hpp>

class FileInputHandleWxStream : public wxInputStream
{
public:
	FileInputHandleWxStream() = delete;
	FileInputHandleWxStream(ren::FileInputHandle* handle, bool takeOwnership = false);
	FileInputHandleWxStream(const FileInputHandleWxStream&) = delete;
	FileInputHandleWxStream(FileInputHandleWxStream&&) = delete;

	virtual ~FileInputHandleWxStream();

protected:
	virtual size_t OnSysRead(void *buffer, size_t bufsize) override;
	virtual wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override;
	virtual wxFileOffset OnSysTell() const override;

private:
	ren::FileInputHandle* p_fileHandle;
	bool p_ownsHandle;
};