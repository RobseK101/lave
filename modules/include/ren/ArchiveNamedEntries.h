#pragma once
#include <ren/ArchiveLinear.h>
#include <map>
#include <vector>

namespace ren
{
    class ArchiveNamedEntries : public ArchiveLinear
    {
    public:
        virtual ~ArchiveNamedEntries();
        ArchiveNamedEntries(const ArchiveNamedEntries&) = delete;
        ArchiveNamedEntries(ArchiveNamedEntries&&) = delete;

        virtual int getFilecount() const override;
        virtual std::wstring getNameW(int _index) override;
        virtual std::string getNameA(int _index) override;
        virtual int getOffset(int _index) override;
        virtual int getSize(int _index) override;

        virtual int findName(const std::wstring& _name) override;
        virtual int findName(const std::string& _name) override;

        virtual size_t iNameCount(const std::wstring& _name) override;
        virtual size_t iNameCount(const std::string& _name) override;

        virtual int iFindName(const std::wstring& _name, size_t _element) override;
        virtual int iFindName(const std::string& _name, size_t _element) override;

        virtual FileInputHandle* getHandle(const std::wstring& _name) override;
        virtual FileInputHandle* getHandle(const std::string& _name) override;
        virtual FileInputHandle* getHandle(int _index) override;

        virtual bool isGood() const override;
        bool namedEntries() const override;
        virtual const char* archiveTypeName() const = 0;

    protected:
        ArchiveNamedEntries() = delete;
        ArchiveNamedEntries(FileInputHandle *_imageHandle, bool _takeOwnership = false);
        void fillDictionary();

        struct Entry
        {
            std::wstring filename;
            int offset;
            int size;
        };

        typedef std::map<std::wstring,int> Dictionary;
        typedef std::multimap<std::wstring,int> DictionaryI;
        
        FileInputHandle *p_imageHandle;
        std::vector<Entry> p_entries;
        Dictionary p_dictionary;
        DictionaryI p_dictionaryI;
        bool p_initSuccess;
        bool p_owner;
    };
}