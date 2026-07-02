#pragma once

#include <ren/ArchiveNamedEntries.h>
#include <xrp_def.h>

namespace ren
{
    class ArchiveXRP : public ArchiveNamedEntries
    {
    public:
        ArchiveXRP() = delete;
        ArchiveXRP(FileInputHandle* _archive, bool _takeOwnership = false);

        const char* archiveTypeName() const override;
    };
}