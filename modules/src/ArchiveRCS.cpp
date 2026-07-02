#include <ren/ArchiveRCS.h>
#include <vector>
#include <cstdint>
#include <cstring>
#include <ren/Stringtools.hpp>
#include <ren/Exceptions.hpp>

namespace ren
{
    ArchiveRCS::ArchiveRCS(FileInputHandle *_imgFile, FileInputHandle *_indFile, bool _takeOwnership) : 
        ArchiveNamedEntries(_imgFile, _takeOwnership)
    {
        if (_indFile == nullptr) {
            throwException<std::invalid_argument>("%s(): A nullptr is not allowed for initialization!", __FUNCTION__);
        }

        // read in the directory
        std::vector<char> directoryData(_indFile->filesize());
        _indFile->read(0, directoryData.data(), directoryData.size());

        // get the filecount
        int16_t filecount;
        std::memcpy(&filecount, &directoryData[0], 2);

        // check the filecount
        int calculatedFilecount = (directoryData.size() - 2) / sizeof(IndBlock);
        int calculatedRemainder = (directoryData.size() - 2) % sizeof(IndBlock);
        if (filecount != calculatedFilecount || calculatedRemainder != 0) {
            throwException<std::runtime_error>("%s(): IND Directory is malformed! (Invalid entry count)", __FUNCTION__);
        }
        
        size_t imageSize = _imgFile->filesize();
        IndBlock* indPtr = reinterpret_cast<IndBlock*>(directoryData.data() + 2);

        // Note: the first entry is the archive itself, it doesn't count.
        // The number of files is therefore the number of entries - 1.
        // The first relevant entry is partially processed before the loop:
        IndEntry lastEntry;
        IndEntry thisEntry;
        thisEntry.filename = indPtr[1].filename;
        thisEntry.offset = indPtr[1].offset;
        for (int i = 2; i < filecount; i++) {
            lastEntry = thisEntry;
            thisEntry.filename = indPtr[i].filename;
            thisEntry.offset = indPtr[i].offset;
            lastEntry.filesize = thisEntry.offset - lastEntry.offset;
            p_entries.push_back(Entry {
                st::strToWStr(lastEntry.filename),
                lastEntry.offset,
                lastEntry.filesize
            });
        }
        thisEntry.filesize = imageSize - thisEntry.offset;
        p_entries.push_back(Entry {
            st::strToWStr(thisEntry.filename),
            thisEntry.offset,
            thisEntry.filesize
        });

        fillDictionary();
        p_initSuccess = true;

        if (p_owner) {
            delete _indFile;
        }
    }

    const char* ArchiveRCS::archiveTypeName() const
    {
        return "AR2_RCS";
    }
}