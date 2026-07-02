#include <ren/ArchiveXRP.h>
#include <vector>
#include <ren/Stringtools.hpp>

namespace ren
{
    ArchiveXRP::ArchiveXRP(FileInputHandle* _archive, bool _takeOwnership) : 
        ArchiveNamedEntries(_archive, _takeOwnership)
    {
        size_t inputFilesize = _archive->filesize();
        if (inputFilesize == 0) {
            throwException<std::runtime_error>("%s(): File is empty.", __FUNCTION__);
        }
        if (inputFilesize % xrp::blockSize) {
            throwException<std::runtime_error>("%s(): File is not a valid XPF resource package (missing block alignment).", __FUNCTION__);
        }

        xrp::Footer footer;
        _archive->seek(-(offset_t)sizeof(xrp::Footer), end);
        _archive->read(&footer, sizeof(xrp::Footer));

        if (footer.fourCC != xrp::fourCC || footer.dictionaryBlockCount == 0) {
            throwException<std::runtime_error>("%s(): File is not a valid XPF resource package (invalid footer).", __FUNCTION__);
        }

        size_t dictionarySize = footer.dictionaryBlockCount * xrp::blockSize;
        std::vector<char> dictionaryData(dictionarySize);

        _archive->seek(-(offset_t)dictionarySize, end);
        _archive->read(dictionaryData.data(), dictionarySize);

        xrp::DictionaryHeader header;
        std::memcpy(&header, &dictionaryData[0], sizeof(xrp::DictionaryHeader));

        std::vector<xrp::Entry> dictionaryEntries(header.entryCount);
        std::memcpy(dictionaryEntries.data(), &dictionaryData[header.entriesArrayOffset], dictionaryEntries.size() * sizeof(xrp::Entry));
        const char* namelist = &dictionaryData[header.namelistOffset];

        size_t archiveSize = header.archiveBlockCount * xrp::blockSize;
        if (archiveSize + dictionarySize > inputFilesize) {
            throwException<std::runtime_error>("%s(): Underflow of the archive starting offset (Archive was calculated to be bigger than the input file).", __FUNCTION__);
        }

        size_t archiveBaseOffset = inputFilesize - (archiveSize + dictionarySize);

        for (size_t i = 0; i < dictionaryEntries.size(); i++) {
            p_entries.push_back(Entry {
                st::strToWStr(namelist + dictionaryEntries[i].nameOffset), 
                (int)(archiveBaseOffset + dictionaryEntries[i].startBlock * xrp::blockSize), 
                (int)dictionaryEntries[i].size
            });
        }
        fillDictionary();
        p_initSuccess = true;
    }

    const char* ArchiveXRP::archiveTypeName() const
    {
        return "XRP";
    }
}