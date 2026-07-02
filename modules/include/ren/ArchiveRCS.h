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
        struct IndBlock
        {
	        char filename[20];
	        int32_t offset;
        };

        struct IndEntry
        {
	        std::string filename;
	        int offset = 0;
	        int filesize = 0;
        };
    };
}