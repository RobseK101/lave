#include <ren/ArchiveNamedEntries.h>
#include <ren/Stringtools.hpp>
#include <ren/Exceptions.hpp>

namespace ren
{
    ArchiveNamedEntries::~ArchiveNamedEntries()
    {
        if (p_initSuccess && p_owner) {
            delete p_imageHandle;
        }
    }

    int ArchiveNamedEntries::getFilecount() const
    {
        return p_entries.size();
    }

    std::wstring ArchiveNamedEntries::getNameW(int _index)
    {
        if (_index >= 0 && _index < p_entries.size()) {
            return p_entries[_index].filename;
        }
        else {
            throwException<std::out_of_range>("%s(): Requested name through invalid index '%d'.", 
                __FUNCTION__, _index);
        }
    }

    std::string ArchiveNamedEntries::getNameA(int _index) {
        if (_index >= 0 && _index < p_entries.size()) {
            return st::wstrToStr(p_entries[_index].filename, '_');
        }
        else {
            throwException<std::out_of_range>("%s(): Requested name through invalid index '%d'.", 
                __FUNCTION__, _index);
        }
    }

    int ArchiveNamedEntries::getOffset(int _index)
    {
        if (_index >= 0 && _index < p_entries.size()) {
            return p_entries[_index].offset;
        }
        else {
            throwException<std::out_of_range>("%s(): Requested offset through invalid index '%d'.", 
                __FUNCTION__, _index);
        }
    }

    int ArchiveNamedEntries::getSize(int _index) 
    {
        if (_index >= 0 && _index < p_entries.size()) {
            return p_entries[_index].size;
        }
        else {
            throwException<std::out_of_range>("%s(): Requested filesize through invalid index '%d'.", 
                __FUNCTION__, _index);
        }
    }

    int ArchiveNamedEntries::findName(const std::wstring& _name) 
    {
        Dictionary::const_iterator it = p_dictionary.find(_name);
        if (it != p_dictionary.end()) {
            return it->second;
        }
        else {
            return -1;
        }
    }

    int ArchiveNamedEntries::findName(const std::string& _name)
    {
        Dictionary::const_iterator it = p_dictionary.find(st::strToWStr(_name));
        if (it != p_dictionary.end()) {
            return it->second;
        }
        else {
            return -1;
        }
    }

    size_t ArchiveNamedEntries::iNameCount(const std::wstring& _name)
    {
        return p_dictionaryI.count(_name);
    }

    size_t ArchiveNamedEntries::iNameCount(const std::string& _name) 
    {
        return p_dictionaryI.count(st::strToWStr(_name));
    }

    int ArchiveNamedEntries::iFindName(const std::wstring& _name, size_t _element)
    {
        std::pair<DictionaryI::const_iterator,DictionaryI::const_iterator> range = p_dictionaryI.equal_range(_name);
        
        int resultIndex = 0;
        while (range.first != range.second) {
            if (resultIndex == _element) {
                return range.first->second;
            }
            resultIndex++;
            range.first++;
        }
        return -1;
    }

    int ArchiveNamedEntries::iFindName(const std::string& _name, size_t _element)
    {
        std::pair<DictionaryI::const_iterator,DictionaryI::const_iterator> range 
            = p_dictionaryI.equal_range(st::strToWStr(_name));
        
        int resultIndex = 0;
        while (range.first != range.second) {
            if (resultIndex == _element) {
                return range.first->second;
            }
            resultIndex++;
            range.first++;
        }
        return -1;
    }

    FileInputHandle* ArchiveNamedEntries::getHandle(const std::wstring& _name)
    {
        Dictionary::iterator it = p_dictionary.find(_name);
        if (it != p_dictionary.end()) {
            return new SectionInputHandle(p_imageHandle, p_entries[it->second].offset, p_entries[it->second].size);
        }
        else {
            std::string nameConverted = st::wstrToStr(_name, '_');
            throwException<std::runtime_error>("%s(): Requested file handle through invalid name \"%s\".", 
                __FUNCTION__, nameConverted.c_str());
        }
    }

    FileInputHandle* ArchiveNamedEntries::getHandle(const std::string& _name)
    {
        Dictionary::iterator it = p_dictionary.find(st::strToWStr(_name));
        if (it != p_dictionary.end()) {
            return new SectionInputHandle(p_imageHandle, p_entries[it->second].offset, p_entries[it->second].size);
        }
        else {
            throwException<std::runtime_error>("%s(): Requested file handle through invalid name \"%s\".", 
                __FUNCTION__, _name.c_str());
        }
    }

    FileInputHandle* ArchiveNamedEntries::getHandle(int _index)
    {
        if (_index >= 0 && _index < p_entries.size()) {
            return new SectionInputHandle(p_imageHandle, p_entries[_index].offset, p_entries[_index].size);
        }
        else {
            throwException<std::out_of_range>("%s(): Requested file handle through invalid index '%d'.", 
                __FUNCTION__, _index);
        }
    }

    bool ArchiveNamedEntries::isGood() const
    {
        if (p_imageHandle == nullptr) {
            return false;
        }
        else {
            return true;
        }
    }

    bool ArchiveNamedEntries::namedEntries() const 
    {
        return true;
    }

    ArchiveNamedEntries::ArchiveNamedEntries(FileInputHandle *_imageHandle, bool _takeOwnership) : 
        p_imageHandle(_imageHandle), p_owner(_takeOwnership), p_initSuccess(false)
    {
        if (_imageHandle == nullptr) {
            throwException<std::invalid_argument>("%s(): A nullptr is not allowed for initialization!", __FUNCTION__);
        }
    }

    void ArchiveNamedEntries::fillDictionary()
    {
        for (int i = 0; i < p_entries.size(); i++) {
            std::pair<std::wstring,int> currentEntry(p_entries[i].filename, i);
            std::pair<std::wstring,int> currentEntryI(st::toUpper(currentEntry.first),currentEntry.second);
            p_dictionary.insert(currentEntry);
            p_dictionaryI.insert(currentEntryI);
        }
    }
}