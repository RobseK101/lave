#include <ren/ArchiveLinear.h>
#include <ren/Exceptions.hpp>

namespace ren
{
    ArchiveLinear::~ArchiveLinear() {}

    std::wstring ArchiveLinear::getNameW(int _index)
    {
        throwException<std::logic_error>("%s(): Not implemented for this archive type (%s).", 
            __FUNCTION__, archiveTypeName());
    }

    std::string ArchiveLinear::getNameA(int _index)
    {
        throwException<std::logic_error>("%s(): Not implemented for this archive type (%s).",
            __FUNCTION__, archiveTypeName());
    }

    int ArchiveLinear::findName(const std::wstring& _name)
    {
        throwException<std::logic_error>("%s(): Not implemented for this archive type (%s).",
            __FUNCTION__, archiveTypeName());
    }

    int ArchiveLinear::findName(const std::string& _name)
    {
        throwException<std::logic_error>("%s(): Not implemented for this archive type (%s).",
            __FUNCTION__, archiveTypeName());
    }

    size_t ArchiveLinear::iNameCount(const std::wstring& _name) 
    {
        throwException<std::logic_error>("%s(): Not implemented for this archive type (%s).",
            __FUNCTION__, archiveTypeName());
    }

    size_t ArchiveLinear::iNameCount(const std::string& _name)
    {
        throwException<std::logic_error>("%s(): Not implemented for this archive type (%s).",
            __FUNCTION__, archiveTypeName());
    }

    int ArchiveLinear::iFindName(const std::wstring& _name, size_t _element)
    {
        throwException<std::logic_error>("%s(): Not implemented for this archive type (%s).",
            __FUNCTION__, archiveTypeName());
    }

    int ArchiveLinear::iFindName(const std::string& _name, size_t _element)
    {
        throwException<std::logic_error>("%s(): Not implemented for this archive type (%s).",
            __FUNCTION__, archiveTypeName());
    }

    FileInputHandle* ArchiveLinear::getHandle(const std::wstring& _name)
    {
        throwException<std::logic_error>("%s(): Not implemented for this archive type (%s).",
            __FUNCTION__, archiveTypeName());
    }

    FileInputHandle* ArchiveLinear::getHandle(const std::string& _name)
    {
        throwException<std::logic_error>("%s(): Not implemented for this archive type (%s).",
            __FUNCTION__, archiveTypeName());
    }
}
