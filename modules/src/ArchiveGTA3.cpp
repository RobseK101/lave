#include <ren/ArchiveGTA3.h>
#include <ren/Stringtools.hpp>

namespace ren
{
    ArchiveGTA3::ArchiveGTA3(FileInputHandle *_imgFile, FileInputHandle *_dirFile, bool _takeOwnership) : 
        ArchiveNamedEntries(_imgFile, _takeOwnership)
    {
        if (_dirFile == nullptr) {
            throwException<std::invalid_argument>("%s(): A nullptr is not allowed for initialization!", __FUNCTION__);
        }

        // read in the directory
        std::vector<char> directoryData(_dirFile->filesize());
        _dirFile->read(0, directoryData.data(), directoryData.size());

        // basic integrity check and file count determination:
        int filecount = directoryData.size() / sizeof(DirBlock);
        int filecountRemainder = directoryData.size() % sizeof(DirBlock);
        if (filecountRemainder != 0) {
            throwException<std::runtime_error>("%s(): DIR Directory is malformed! (oversized by %d bytes)", __FUNCTION__, filecountRemainder);
        }

        DirBlock* dirPtr = reinterpret_cast<DirBlock*>(directoryData.data());
        for (int i = 0; i < filecount; i++) {
            p_entries.push_back(Entry {
                st::strToWStr(dirPtr[i].filename),
                dirPtr[i].blockOffset * imgBlockSize,
                dirPtr[i].blockCount * imgBlockSize
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