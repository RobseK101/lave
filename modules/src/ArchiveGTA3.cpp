#include <ren/ArchiveGTA3.h>
#include <ren/Stringtools.hpp>
#include <cstdint>

namespace ren
{
    ArchiveGTA3::ArchiveGTA3(FileInputHandle *_imgFile, FileInputHandle *_dirFile, bool _takeOwnership) : 
        ArchiveNamedEntries(_imgFile, _takeOwnership)
    {
        uint32_t entryCount;
        std::vector<DirBlock> directory;
        if (_dirFile == nullptr) {
            if (_imgFile->filesize() < 8) {
                throwException<std::invalid_argument>("%s(): No DIR file was given and IMG archive is malformed.", __FUNCTION__);
            }
            FourCC fourCC;
            _imgFile->read(0, &fourCC, sizeof(FourCC));
            _imgFile->read(&entryCount, sizeof(uint32_t));
            if (fourCC != ver2Mark) {
                throwException<std::invalid_argument>("%s(): No DIR file was given and this is not a Ver2 IMG file.", __FUNCTION__);
            }
            if (_imgFile->filesize() - _imgFile->tell() < entryCount * sizeof(DirBlock)) {
                throwException<std::invalid_argument>("%s(): No DIR file was given and IMG archive is malformed.", __FUNCTION__);
            }
            directory.resize(entryCount);
            _imgFile->read(directory.data(), entryCount * sizeof(DirBlock));
        }
        else {
            // basic integrity check and file count determination:
            entryCount = _dirFile->filesize() / sizeof(DirBlock);
            int entryCountRemainder = _dirFile->filesize() % sizeof(DirBlock);
            if (entryCountRemainder != 0) {
                throwException<std::runtime_error>("%s(): DIR Directory is malformed! (oversized by %u bytes)", __FUNCTION__, entryCountRemainder);
            }
            directory.resize(entryCount);
            _dirFile->read(0, directory.data(), entryCount * sizeof(DirBlock));
        }

        for (size_t i = 0; i < directory.size(); i++) {
            p_entries.push_back(Entry {
                st::strToWStr(directory[i].filename),
                directory[i].blockOffset * imgBlockSize,
                directory[i].blockCount * imgBlockSize
            });
        }
        fillDictionary();
        p_initSuccess = true;
        if (p_owner) {
            delete _dirFile;
        }
    }

    const char* ArchiveGTA3::archiveTypeName() const 
    {
        return "GTA3VC_IMG";
    }
}