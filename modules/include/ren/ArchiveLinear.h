#pragma once
#include <ren/FileHandle.hpp>
#include <string>

namespace ren
{
    // Interface class
    class ArchiveLinear
    {
    public:
        virtual ~ArchiveLinear();

        virtual int getFilecount() const = 0;
        virtual std::wstring getNameW(int _index);
        virtual std::string getNameA(int _index);
        virtual int getOffset(int _index) = 0;
        virtual int getSize(int _index) = 0;

        virtual int findName(const std::wstring& _name);
        virtual int findName(const std::string& _name);

        virtual size_t iNameCount(const std::wstring& _name);
        virtual size_t iNameCount(const std::string& _name);

        virtual int iFindName(const std::wstring& _name, size_t _element);
        virtual int iFindName(const std::string& _name, size_t _element);

        virtual FileInputHandle* getHandle(const std::wstring& _name);
        virtual FileInputHandle* getHandle(const std::string& _name);
        virtual FileInputHandle* getHandle(int _index) = 0;

        virtual bool isGood() const = 0;
        virtual bool namedEntries() const = 0;
        virtual const char* archiveTypeName() const = 0;
    };  
}