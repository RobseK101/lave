#include <ren/ArchiveRCS.h>
#include <vector>
#include <cstdint>
#include <cstring>
#include <utility>
#include <ren/Stringtools.hpp>
#include <ren/Exceptions.hpp>

namespace ren
{
    unsigned int decimalDigitCount(int _number)
    {
        unsigned int digitCount = 0;
        do {
            digitCount++;
            _number /= 10;
        } while (_number); 
        return digitCount;
    }

    std::pair<ArchiveRCS::Version, int> ArchiveRCS::checkInd(const std::vector<char>& _directoryData)
    {
        if (_directoryData.size() < 2) {
            return { VERSION_INVALID, 0 };
        }
        uint16_t fileCount;
        std::memcpy(&fileCount, &_directoryData[0], 2);
        if (2 + sizeof(IndBlockUnnamed) * static_cast<int>(fileCount) == _directoryData.size()) {
            return { VERSION_UNNAMED, static_cast<int>(fileCount) };
        }
        else if (2 + sizeof(IndBlock) * static_cast<int>(fileCount) == _directoryData.size()) {
            return { VERSION_NAMED, static_cast<int>(fileCount) };
        }
        else {
            return { VERSION_INVALID, 0 };
        }
    }

    ArchiveRCS::ArchiveRCS(FileInputHandle *_imgFile, FileInputHandle *_indFile, bool _takeOwnership) : 
        ArchiveNamedEntries(_imgFile, _takeOwnership)
    {
        if (_indFile == nullptr) {
            throwException<std::invalid_argument>("%s(): No IND file was provided!", __FUNCTION__);
        }

        // read in the directory
        std::vector<char> directoryData(_indFile->filesize());
        _indFile->read(0, directoryData.data(), directoryData.size());

        std::pair<Version, int> indProperties = checkInd(directoryData);
        size_t imageSize = _imgFile->filesize();
        switch (indProperties.first) {
            case VERSION_UNNAMED:
            {
                if (indProperties.second < 1) {
                    throwException<std::invalid_argument>("%s(): Actual file cound must not be zero!", __FUNCTION__);
                }
                std::vector<IndBlockUnnamed> entryArray(indProperties.second);
                std::memcpy(entryArray.data(), &directoryData[2], sizeof(IndBlockUnnamed) * entryArray.size());
                directoryData.clear();

                p_entries.reserve(entryArray.size());
                unsigned int filenameDigitCount = decimalDigitCount(entryArray.size());
                IndEntryUnnamed lastEntry;
                IndEntryUnnamed thisEntry;
                thisEntry.offset = entryArray[0].offset;
                for (size_t i = 1; i < entryArray.size(); i++) {
                    lastEntry = thisEntry;
                    thisEntry.offset = entryArray[i].offset;
                    lastEntry.filesize = thisEntry.offset - lastEntry.offset;
                    std::wstring lastEntryFilename = unnamedBaseName + st::toStr<std::wstring>(i - 1, filenameDigitCount);
                    p_entries.push_back(Entry{
                        std::move(lastEntryFilename),
                        lastEntry.offset,
                        lastEntry.filesize
                        });
                }
                thisEntry.filesize = imageSize - thisEntry.offset;
                std::wstring thisEntryFilename = unnamedBaseName + st::toStr<std::wstring>(entryArray.size() - 1, filenameDigitCount);
                p_entries.push_back(Entry{
                    std::move(thisEntryFilename),
                    thisEntry.offset,
                    thisEntry.filesize
                    });
                break;
            }
            case VERSION_NAMED:
            {
                if (indProperties.second < 2) {
                    throwException<std::invalid_argument>("%s(): Actual file cound must not be zero!", __FUNCTION__);
                }
                std::vector<IndBlock> entryArray(indProperties.second);
                std::memcpy(entryArray.data(), &directoryData[2], sizeof(IndBlock) * entryArray.size());
                directoryData.clear();

                // Note: the first entry is the archive itself, it doesn't count.
                // The number of files is therefore the number of entries - 1.
                // The first relevant entry is partially processed before the loop:
                p_entries.reserve(entryArray.size() - 1);
                unsigned int filenameDigitCount = decimalDigitCount(entryArray.size() - 1);
                IndEntry lastEntry;
                IndEntry thisEntry;
                thisEntry.filename = entryArray[1].filename;
                thisEntry.offset = entryArray[1].offset;
                for (size_t i = 2; i < entryArray.size(); i++) {
                    lastEntry = thisEntry;
                    thisEntry.filename = entryArray[i].filename;
                    thisEntry.offset = entryArray[i].offset;
                    lastEntry.filesize = thisEntry.offset - lastEntry.offset;
                    p_entries.push_back(Entry{
                        st::strToWStr(lastEntry.filename),
                        lastEntry.offset,
                        lastEntry.filesize
                        });
                }
                thisEntry.filesize = imageSize - thisEntry.offset;
                p_entries.push_back(Entry{
                    st::strToWStr(thisEntry.filename),
                    thisEntry.offset,
                    thisEntry.filesize
                    });
                break;
            }
            default:
            {
                throwException<std::runtime_error>("%s(): IND Directory is malformed!", __FUNCTION__);
            }
        }

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