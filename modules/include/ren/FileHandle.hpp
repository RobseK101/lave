#pragma once
#include <fstream>
#include <ren/Exceptions.hpp>
#include <cstring>
#include <vector>
#include <cstdio>
#include <ren/Math.hpp>
#ifndef _WIN32
#include <ren/StringUTF.hpp>
#endif // _WIN32

/// @file
/// @brief This header file contains the definition of the FileInputHandle and FileOutputHandle
///        interfaces (i.e. abstract classes), meant to function as general adaptors for file IO
///        streams. Additionally it includes implementations for std::fstreams, FILE* and virtual files
///        using const char arrays and std::vector.

namespace ren
{
    typedef intptr_t offset_t;

    enum SeekDir
    {
        beg, cur, end
    };

    /// The main interface class for input operations.
    class FileInputHandle
    {
    public:
        virtual ~FileInputHandle() = default;

        virtual size_t read(void *dest, size_t count) = 0;
        virtual size_t read(size_t offset, void *dest, size_t count) = 0;
        virtual size_t tell() const = 0;
        virtual void seek(size_t pos) = 0;
        virtual void seek(offset_t offset, SeekDir direction) = 0;
        virtual size_t filesize() const = 0;

        bool eof() const 
        {
            if (filesize() - tell() == 0) {
                return true;
            }
            else {
                return false;
            }
        }
    };

    /// Initialized with an existing std::ifstream.
    class stdFileInputHandle : public FileInputHandle
    {
    public:
        stdFileInputHandle() = delete;
        stdFileInputHandle(std::ifstream *str);

        virtual size_t read(void *dest, size_t count) override;
        virtual size_t read(size_t offset, void *dest, size_t count) override;
        virtual size_t tell() const override;
        virtual void seek(size_t pos) override;
        virtual void seek(offset_t offset, SeekDir direction) override;
        virtual size_t filesize() const override;

    private:
        std::ifstream *p_str;
        size_t p_size;
    };

    inline stdFileInputHandle::stdFileInputHandle(std::ifstream *str)
    {
        if (str->is_open())
        {
            p_str = str;
        }
        else
        {
            throwException<std::runtime_error>("%s(): File stream is invalid.", __FUNCTION__);
        }
        p_str->seekg(0, std::ios::end);
        p_size = p_str->tellg();
        p_str->seekg(0, std::ios::beg);
    }

    inline size_t stdFileInputHandle::read(void *dest, size_t count)
    {
        std::streampos positionBefore = p_str->tellg();
        p_str->read(reinterpret_cast<char*>(dest), count);
        return p_str->tellg() - positionBefore;
    } 

    inline size_t stdFileInputHandle::read(size_t offset, void *dest, size_t count)
    {
        std::streampos positionBefore = p_str->tellg();
        p_str->seekg(offset, std::ios::beg);
        p_str->read(reinterpret_cast<char*>(dest), count);
        return p_str->tellg() - positionBefore;
    }

    inline size_t stdFileInputHandle::tell() const
    {
        return p_str->tellg();
    }

    inline void stdFileInputHandle::seek(size_t pos)
    {
        p_str->seekg(math::max<size_t>(pos, p_size));
    }

    inline void stdFileInputHandle::seek(offset_t offset, SeekDir dir)
    {
        switch (dir)
        {
            case beg:
            {
                p_str->seekg(math::clamp<offset_t>(offset, 0, p_size), std::ios::beg);
                return;
            }
            case cur:
            {
                offset_t current = p_str->tellg();
                p_str->seekg(math::clamp<offset_t>(current + offset, 0, p_size), std::ios::beg);
                return;
            }
            case end:
            {
                p_str->seekg(math::clamp<offset_t>(offset, -(offset_t)p_size, 0), std::ios::end);
                return;
            }
        }
    }

    inline size_t stdFileInputHandle::filesize() const
    {
        return p_size;
    }

    /// Initialized with a const char*
    class MemoryInputHandle : public FileInputHandle
    {
    public:
        MemoryInputHandle() = delete;
        MemoryInputHandle(const char *data, size_t size);

        virtual size_t read(void *dest, size_t count) override;
        virtual size_t read(size_t offset, void *dest, size_t count) override;
        virtual size_t tell() const;
        virtual void seek(size_t pos) override;
        virtual void seek(offset_t offset, SeekDir dir) override;
        virtual size_t filesize() const override;

    private:
        const char *p_data;
        size_t p_position;
        size_t p_size;
    };

    inline MemoryInputHandle::MemoryInputHandle(const char* data, size_t size) :
        p_data(data), p_position(0), p_size(size) {}

    inline size_t MemoryInputHandle::read(void *dest, size_t count)
    {
        size_t readable = math::min(count, p_size - p_position);
        std::memcpy(dest, p_data + p_position, readable);
        p_position += readable;
        return readable;
    }

    inline size_t MemoryInputHandle::read(size_t offset, void* dest, size_t count)
    {
        if (offset > p_size) {
            return 0;
        }        

        size_t readable = math::min(count, p_size - offset);
        std::memcpy(dest, p_data + offset, readable);
        p_position = offset + readable;
        return readable;
    }

    inline size_t MemoryInputHandle::tell() const
    {
        return p_position;
    }

    inline void MemoryInputHandle::seek(size_t pos)
    {
        p_position = math::min(pos, p_size);
    }

    inline void MemoryInputHandle::seek(offset_t offset, SeekDir dir)
    {
        switch (dir)
        {
            case beg:
            {
                p_position = math::clamp<offset_t>(offset, 0, p_size);
                return;
            }
            case cur:
            {
                p_position = math::clamp<offset_t>((offset_t)p_position + offset, 0, p_size);
                return;
            }
            case end:
            {
                p_position = math::clamp<offset_t>((offset_t)p_size + offset, 0, p_size);
                return;
            }
        }
    }

    inline size_t MemoryInputHandle::filesize() const
    {
        return p_size;
    }

    /// Self contained file input stream; uses FILE* internally.
    class CFileInputHandle : public FileInputHandle
    {
    public:
        virtual ~CFileInputHandle();
        CFileInputHandle() = delete;
        CFileInputHandle(const wchar_t *_filename);
        CFileInputHandle(const char *_filename);

        virtual size_t read(void *dest, size_t count) override;
        virtual size_t read(size_t offset, void *dest, size_t count) override;
        virtual size_t tell() const override;
        virtual void seek(size_t pos) override;
        virtual void seek(offset_t offset, SeekDir dir) override;
        virtual size_t filesize() const override;

        bool isGood() const;

    private:
        FILE* p_stream;
        size_t p_size;
    };

    inline CFileInputHandle::~CFileInputHandle()
    {
        if (p_stream) {
            fclose(p_stream);
        }
    }

#ifdef _WIN32

    inline CFileInputHandle::CFileInputHandle(const wchar_t *_filename)
    {

        FILE *stream = _wfopen(_filename, L"rb");
        if (stream) {
            fseek(stream, 0, SEEK_END);
            p_size = ftell(stream);
            fseek(stream, 0, SEEK_SET);
            p_stream = stream;
        }
        else {
            p_size = 0;
            p_stream = nullptr;
        }
    }

#else

    inline CFileInputHandle::CFileInputHandle(const wchar_t *_filename)
    {
        std::string utf8String = wstrToUtf8(_filename);
        FILE *stream = fopen(utf8String.c_str(), "rb");
        if (stream) {
            fseek(stream, 0, SEEK_END);
            p_size = ftell(stream);
            fseek(stream, 0, SEEK_SET);
            p_stream = stream;
        }
        else {
            p_size = 0;
            p_stream = nullptr;
        }
    }

#endif // _WIN32

    inline CFileInputHandle::CFileInputHandle(const char *_filename)
    {
        FILE *stream = fopen(_filename, "rb");
        if (stream) {
            fseek(stream, 0, SEEK_END);
            p_size = ftell(stream);
            fseek(stream, 0, SEEK_SET);
            p_stream = stream;
        }
        else {
            p_size = 0;
            p_stream = nullptr;
        }
    }

    inline size_t CFileInputHandle::read(void *dest, size_t count)
    {
        if (p_stream == nullptr) {
            throwException<std::runtime_error>("%s(): File stream is invalid.", __FUNCTION__);
        }
        return fread(dest, 1, count, p_stream);
    }

    inline size_t CFileInputHandle::read(size_t offset, void *dest, size_t count)
    {
        if (p_stream == nullptr) {
            throwException<std::runtime_error>("%s(): File stream is invalid.", __FUNCTION__);
        }
        fseek(p_stream, offset, SEEK_SET);
        return fread(dest, 1, count, p_stream);
    }

    inline size_t CFileInputHandle::tell() const
    {
        if (p_stream == nullptr) {
            throwException<std::runtime_error>("%s(): File stream is invalid.", __FUNCTION__);
        }
        return ftell(p_stream);
    }

    inline void CFileInputHandle::seek(size_t pos)
    {
        if (p_stream == nullptr) {
            throwException<std::runtime_error>("%s(): File stream is invalid.", __FUNCTION__);
        }
        fseek(p_stream, math::max<size_t>(pos, p_size), SEEK_SET);
    }

    inline void CFileInputHandle::seek(offset_t offset, SeekDir dir)
    {
        if (p_stream == nullptr) {
            throwException<std::runtime_error>("%s(): File stream is invalid.", __FUNCTION__);
        }
        switch (dir)
        {
            case beg:
            {
                fseek(p_stream, math::clamp<offset_t>(offset, 0, p_size), SEEK_SET);
                return;
            }
            case cur:
            {
                offset_t current = ftell(p_stream);
                fseek(p_stream, math::clamp<offset_t>(current + offset, 0, p_size), SEEK_SET);
                return;
            }
            case end:
            {
                fseek(p_stream, math::clamp<offset_t>(offset, -(offset_t)p_size, 0), SEEK_END);
                return;
            }
        }
    }

    inline size_t CFileInputHandle::filesize() const
    {
        return p_size;
    }

    inline bool CFileInputHandle::isGood() const
    {
        return p_stream;
    }

    /// File input handle on a section of a file. Warning: inherently not multithreading safe!
    class SectionInputHandle : public FileInputHandle
    {
    public:
        SectionInputHandle() = delete;
        SectionInputHandle(FileInputHandle *_archive, size_t _offset, size_t _size);
        ~SectionInputHandle() {};

        virtual size_t read(void *dest, size_t count) override;
        virtual size_t read(size_t offset, void *dest, size_t count) override;
        virtual size_t tell() const override;
        virtual void seek(size_t pos) override;
        virtual void seek(offset_t offset, SeekDir dir) override;
        virtual size_t filesize() const override;

    private:
        void updateArchiveCursor();

        FileInputHandle *p_archiveHandle;
        const size_t p_offset;
        const size_t p_filesize;
        size_t p_position;
    };

    inline SectionInputHandle::SectionInputHandle(FileInputHandle *_archive, size_t _offset, size_t _size) :
        p_archiveHandle(_archive), p_offset(_offset), p_filesize(_size)
    {
        if (p_archiveHandle == nullptr) {
            throwException<std::invalid_argument>("%s(): A nullptr is not allowed for initialization!", __FUNCTION__);
        }
        if (p_offset + p_filesize > p_archiveHandle->filesize()) {
            throwException<std::out_of_range>("%s(): The sub file's size exceeds the main file's file size!", __FUNCTION__);
        }

        p_archiveHandle->seek(p_offset);
        p_position = 0;
    }

    inline size_t SectionInputHandle::read(void *dest, size_t count)
    {
        updateArchiveCursor();
        size_t readable = math::min(count, p_filesize - p_position);
        p_archiveHandle->read(dest, readable);
        p_position += readable;
        return readable;
    }

    inline size_t SectionInputHandle::read(size_t offset, void *dest, size_t count)
    {
        if (offset > p_filesize) {
            return 0;
        }
        size_t readable = math::min(count, p_filesize - offset);
        p_archiveHandle->read(p_offset + offset, dest, readable);
        p_position = p_offset + offset + readable;
        return readable;
    }

    inline size_t SectionInputHandle::tell() const
    {
        return p_position;
    }

    inline void SectionInputHandle::seek(size_t pos)
    {
        p_position = math::min(pos, p_filesize);
        p_archiveHandle->seek(p_offset + p_position);
    }

    inline void SectionInputHandle::seek(offset_t offset, SeekDir dir)
    {
        switch (dir) 
        {
            case beg:
            {
                p_position = math::clamp<offset_t>(offset, 0, p_filesize);
                p_archiveHandle->seek(p_offset + p_position);
                return;
            }
            case cur:
            {
                p_position = math::clamp<offset_t>((offset_t)p_position + offset, 0, p_filesize);
                p_archiveHandle->seek(p_offset + p_position);
                return;
            }
            case end:
            {
                p_position = math::clamp<offset_t>((offset_t)p_filesize + offset, 0, p_filesize);
                p_archiveHandle->seek(p_offset + p_position);
                return;
            }
        }
    }

    inline size_t SectionInputHandle::filesize() const
    {
        return p_filesize;
    }

    inline void SectionInputHandle::updateArchiveCursor()
    {
        size_t virtualAbsolute = p_offset + p_position;
        if (p_archiveHandle->tell() != virtualAbsolute) {
            p_archiveHandle->seek(virtualAbsolute, beg);
        }
    }

    /// Base interface for all file input operations
    class FileOutputHandle
    {
    public:
        virtual void write(size_t _offset, const void* _src, size_t _count) = 0;
        virtual void write(const void* _src, size_t _count) = 0;
        virtual size_t streamOffset() const = 0;
        virtual size_t filesize() const = 0;
    };

    /// Initialized with an existing std::ofstream.
    class stdFileOutputHandle : public FileOutputHandle
    {
    public:
        stdFileOutputHandle() = delete;
        stdFileOutputHandle(std::ofstream* _str);

        void write(size_t _offset, const void* _src, size_t _count) override;
        void write(const void* _src, size_t _count) override;
        size_t streamOffset() const override;
        // Buggy!!!
        size_t filesize() const override;

    private:
        std::ofstream* p_str;
        size_t p_size;
    };

    inline stdFileOutputHandle::stdFileOutputHandle(std::ofstream* _str)
    {
        if (_str && _str->is_open())
        {
            p_str = _str;
        }
        else
        {
            throwException<std::runtime_error>("%s(): File stream is invalid.", __FUNCTION__);
        }
        p_str->seekp(0, std::ios::end);
        p_size = p_str->tellp();
    }

    inline void stdFileOutputHandle::write(size_t _offset, const void* _src, size_t _count)
    {
        p_str->seekp(_offset, std::ios::beg);
        p_str->write(reinterpret_cast<const char*>(_src), _count);
    }

    inline void stdFileOutputHandle::write(const void* _src, size_t _count)
    {
        p_str->write(reinterpret_cast<const char*>(_src), _count);
    }

    inline size_t stdFileOutputHandle::streamOffset() const
    {
        return p_str->tellp();
    }

    // Buggy!!!
    inline size_t stdFileOutputHandle::filesize() const
    {
        return p_size;
    }

    /// Initialized with a bounds checked writable block of memory. 
    class MemoryOutputHandle : public FileOutputHandle
    {
    public:
        MemoryOutputHandle() = delete;
        MemoryOutputHandle(char* _buffer, size_t _size);

        void write(size_t _offset, const void* _src, size_t _count) override;
        void write(const void* _src, size_t _count) override;
        size_t streamOffset() const override;
        size_t filesize() const override;

    private:
        char* p_buffer;
        size_t p_size;
        size_t p_streampos;
    };

    inline MemoryOutputHandle::MemoryOutputHandle(char* _buffer, size_t _size)
    {
        if (_buffer) {
            p_buffer = _buffer;
            p_size = _size;
            p_streampos = 0;
        }
        else {
            throwException<std::logic_error>("%s(): Buffer pointer cannot be NULL!", __FUNCTION__);
        }
    }

    inline void MemoryOutputHandle::write(size_t _offset, const void* _src, size_t _count)
    {
        if (_offset + _count > p_size) {
            throwException<std::out_of_range>("%s(): Attempted to write out of bounds!", __FUNCTION__);
        }
        std::memcpy(p_buffer + _offset, _src, _count);
        p_streampos = _offset + _count;
    }

    inline void MemoryOutputHandle::write(const void* _src, size_t _count)
    {
        if (p_streampos + _count > p_size) {
            throwException<std::out_of_range>("%s(): Attempted to write out of bounds!", __FUNCTION__);
        }
        std::memcpy(p_buffer + p_streampos, _src, _count);
        p_streampos += _count;
    }

    inline size_t MemoryOutputHandle::streamOffset() const
    {
        return p_streampos;
    }

    inline size_t MemoryOutputHandle::filesize() const
    {
        return p_size;
    }

    /// Initialized with an std::vector<char>. Write as much as your RAM can handle!
    class VectorOutputHandle : public FileOutputHandle
    {
    public:
        VectorOutputHandle() = delete;
        VectorOutputHandle(std::vector<char>* _vector);

        void write(size_t _offset, const void* _src, size_t _count) override;
        void write(const void* _src, size_t _count) override;
        size_t streamOffset() const override;
        size_t filesize() const override;

    private:
        std::vector<char>* p_vector;
        size_t p_streampos;
    };

    inline VectorOutputHandle::VectorOutputHandle(std::vector<char>* _vector)
    {
        if (_vector) {
            p_vector = _vector;
            p_streampos = 0;
        }
        else {
            throwException<std::logic_error>("%s(): Vector pointer cannot be NULL!", __FUNCTION__);
        }
    }

    inline void VectorOutputHandle::write(size_t _offset, const void* _src, size_t _count)
    {
        if (_offset + _count > p_vector->size()) {
            p_vector->resize(_offset + _count);
        }
        std::memcpy(p_vector->data() + _offset, _src, _count);
        p_streampos = _offset + _count;
    }

    inline void VectorOutputHandle::write(const void* _src, size_t _count)
    {
        if (p_streampos + _count > p_vector->size()) {
            p_vector->resize(p_streampos + _count);
        }
        std::memcpy(p_vector->data() + p_streampos, _src, _count);
        p_streampos += _count;
    }

    inline size_t VectorOutputHandle::streamOffset() const
    {
        return p_streampos;
    }

    inline size_t VectorOutputHandle::filesize() const
    {
        return p_vector->size();
    }

    /// Self contained file output stream; uses FILE* internally.
    class CFileOutputHandle : public FileOutputHandle
    {
    public:
        CFileOutputHandle() = delete;
        CFileOutputHandle(const wchar_t *_filename, bool _truncate = false);
        CFileOutputHandle(const char *_filename, bool _truncate = false);

        void write(size_t _offset, const void* _src, size_t _count) override;
        void write(const void* _src, size_t _count) override;
        size_t streamOffset() const override;
        size_t filesize() const override;
        bool isGood() const;

    private:
        FILE* p_stream;
        size_t p_size;
    };

#ifdef _WIN32

    inline CFileOutputHandle::CFileOutputHandle(const wchar_t *_filename, bool _truncate)
    {
        FILE* stream = (_truncate) ? _wfopen(_filename, L"wtb") : _wfopen(_filename, L"wb");
        if (stream) {
            fseek(stream, 0, SEEK_END);
            p_size = ftell(stream);
            p_stream = stream;
        }
        else {
            p_size = 0;
            p_stream = nullptr;
        }
    }

#else

    inline CFileOutputHandle::CFileOutputHandle(const wchar_t *_filename, bool _truncate)
    {
        std::string convertedWString = wstrToUtf8(_filename);
        FILE* stream = (_truncate) ? fopen(convertedWString.c_str(), "wtb") : fopen(convertedWString.c_str(), "wb");
        if (stream) {
            fseek(stream, 0, SEEK_END);
            p_size = ftell(stream);
            p_stream = stream;
        }
        else {
            p_size = 0;
            p_stream = nullptr;
        }
    }

#endif // _WIN32

    inline CFileOutputHandle::CFileOutputHandle(const char *_filename, bool _truncate)
    {
        FILE* stream = (_truncate) ? fopen(_filename, "wtb") : fopen(_filename, "wb");
        if (stream) {
            fseek(stream, 0, SEEK_END);
            p_size = ftell(stream);
            p_stream = stream;
        }
        else {
            p_size = 0;
            p_stream = nullptr;
        }
    }

    inline void CFileOutputHandle::write(size_t _offset, const void* _src, size_t _count)
    {
        if (p_stream == nullptr) {
            throwException<std::runtime_error>("%s(): File stream is invalid.", __FUNCTION__);
        }
        fseek(p_stream, _offset, SEEK_SET);
        fwrite(_src, 1, _count, p_stream);
        size_t newpos = ftell(p_stream);
        if (newpos > p_size) {
            p_size = newpos;
        }
    }

    inline void CFileOutputHandle::write(const void* _src, size_t _count)
    {
        if (p_stream == nullptr) {
            throwException<std::runtime_error>("%s(): File stream is invalid.", __FUNCTION__);
        }
        fwrite(_src, 1, _count, p_stream);
        size_t newpos = ftell(p_stream);
        if (newpos > p_size) {
            p_size = newpos;
        }
    }

    inline size_t CFileOutputHandle::streamOffset() const
    {
        return ftell(p_stream);
    }

    inline size_t CFileOutputHandle::filesize() const
    {
        return p_size;
    }

    inline bool CFileOutputHandle::isGood() const
    {
        return p_stream;
    }
}
