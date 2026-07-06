#pragma once
#include <ren/ArchiveNamedEntries.h>
#include <cstdint>

namespace ren
{
    class ArchiveRCS : public ArchiveNamedEntries
    {
    public:
        ArchiveRCS() = delete;
        ArchiveRCS(FileInputHandle *_imgFile, FileInputHandle *_indFile, bool _takeOwnership = false);

        const char* archiveTypeName() const override;

    private:
        static constexpr wchar_t unnamedBaseName[] = L"resource";

        struct IndBlock
        {
	        char filename[20];
	        int32_t offset;
        };

        struct IndBlockUnnamed
        {
            int32_t offset;
        };

        struct IndEntry
        {
	        std::string filename;
	        int offset = 0;
	        int filesize = 0;
        };

        struct IndEntryUnnamed
        {
            int offset = 0;
            int filesize = 0;
        };

        enum Version
        {
            VERSION_INVALID = -1, VERSION_UNNAMED = 0, VERSION_NAMED = 1
        };

        static std::pair<Version, int> checkInd(const std::vector<char>& _directoryData);
    };
}