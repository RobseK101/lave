#pragma once
#include <ren/ArchiveNamedEntries.h>
#include <cstdint>

namespace ren
{
    // Format for IMG/DIR archives used in GTA III and VC
    class ArchiveGTA3 : public ArchiveNamedEntries
    {
    public:
        ArchiveGTA3() = delete;
        ArchiveGTA3(FileInputHandle *_imgFile, FileInputHandle *_dirFile, bool _takeOwnership = false);

        const char* archiveTypeName() const override;

    private:
        static constexpr int imgBlockSize = 0x800;

        struct DirBlock
        {
            int32_t blockOffset;
            int32_t blockCount;
            char filename[24];
        };
    };
}