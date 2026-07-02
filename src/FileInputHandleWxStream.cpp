#include "FileInputHandleWxStream.h"
#include <ren/Exceptions.hpp>

FileInputHandleWxStream::FileInputHandleWxStream(ren::FileInputHandle* handle, bool takeOwnership) : 
	p_fileHandle(handle), p_ownsHandle(takeOwnership)
{
	if (!p_fileHandle) {
		ren::throwException<std::logic_error>("%s(): Cannot be initialized with a nullptr handle.", __FUNCTION__);
	}
}

FileInputHandleWxStream::~FileInputHandleWxStream()
{
	if (p_ownsHandle) {
		delete p_fileHandle;
	}
}

size_t FileInputHandleWxStream::OnSysRead(void* buffer, size_t bufsize)
{
	return p_fileHandle->read(buffer, bufsize);
}

wxFileOffset FileInputHandleWxStream::OnSysSeek(wxFileOffset pos, wxSeekMode mode)
{
	ren::SeekDir direction = ren::beg;
	switch (mode)
	{
	case wxFromStart:
	{
		direction = ren::beg;
		break;
	}
	case wxFromCurrent:
	{
		direction = ren::cur;
		break;
	}
	case wxFromEnd:
	{
		direction = ren::end;
		break;
	}
	}
	p_fileHandle->seek(pos, direction);
	return p_fileHandle->tell();
}

wxFileOffset FileInputHandleWxStream::OnSysTell() const
{
	return p_fileHandle->tell();
}



