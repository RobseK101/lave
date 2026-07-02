// xpf-respacker.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <cstdio>
#include <fstream>
#include <string>
#include "ren/Stringtools.hpp"
#include <vector>
#include <unordered_map>
#include <utility>
#include <filesystem>

#define XRP_BUILDER
#include "xrp_def.h"

namespace fs = std::filesystem;

struct PrebuildEntry
{
    fs::path filepath;
    std::string filename;
    size_t filesize;
    size_t blockCount;
};

enum class CaseSensitivityPolicy
{
    CaseSensitive, CaseInsensitive
};

struct PacklistOptions
{
    CaseSensitivityPolicy ciPolicy = CaseSensitivityPolicy::CaseSensitive;
};

struct PacklistContent
{
    std::vector<PrebuildEntry> entries = {};
    std::unordered_map<std::string, size_t> dictionary = {};
    size_t filenameListSize = 0;
};

inline std::string implementCiPolicy(const std::string& _input, CaseSensitivityPolicy _policy)
{
    return (_policy == CaseSensitivityPolicy::CaseInsensitive) ? ren::st::toUpper(_input) : _input;
}

inline bool readBoolean(const std::string& _value)
{
    if (ren::st::toUpper(_value) == "TRUE" || atoi(_value.c_str())) {
        return true;
    }
    else {
        return false;
    }
}

PacklistOptions readPacklistOptionsSection(std::istream& _listIn, std::ostream& _logOut)
{
    PacklistOptions result = {};
    _logOut << "[Options begin]" << std::endl;
    while (!_listIn.eof()) {
        std::string linebufferRaw;
        std::getline(_listIn, linebufferRaw);
        std::string linebufferClean = ren::st::trim(linebufferRaw, xrp::trimDefault);
        if (linebufferClean.empty()) {
            continue;
        }
        std::string linebufferUpper = ren::st::toUpper(linebufferClean);
        if (linebufferUpper == xrp::endSign) {
            _logOut << "[Options end]" << std::endl;
            return result;
        }

        auto keyValuePair = ren::st::splitKeyValuePair(linebufferClean, '=', xrp::trimDefault);
        std::string keyUpper = ren::st::toUpper(keyValuePair.first);
        if (keyUpper == xrp::optionsCaseInsensitive) {
            result.ciPolicy = readBoolean(keyValuePair.second) ? CaseSensitivityPolicy::CaseInsensitive : CaseSensitivityPolicy::CaseSensitive;
            continue;
        }
    }
    throw std::runtime_error(__FUNCTION__ "() syntax error: reached end of list file without options end marker ([END]).");
}

PacklistContent readPacklistFilesSection(std::istream& _listIn, const fs::path& _listfileDirectory, PacklistOptions _options, std::ostream& _logOut)
{
    size_t entryCounter = 0;
    PacklistContent result = { {}, {}, 0 };
    _logOut << "[File list begin]" << std::endl;
    while (!_listIn.eof()) {
        std::string linebufferRaw;
        std::getline(_listIn, linebufferRaw);
        std::string linebufferClean = ren::st::trim(linebufferRaw, xrp::trimDefault);
        if (linebufferClean.empty()) {
            continue;
        }
        std::string linebufferUpper = ren::st::toUpper(linebufferClean);
        if (linebufferUpper == xrp::endSign) {
            _logOut << "[File list end]" << std::endl;
            return result;
        }
        entryCounter++;

        _logOut.width(3);
        _logOut.fill(' ');
        _logOut << "Entry no " << entryCounter << ": ";

        // Create an absolut path from relative ones, based on the location of the listfile
        fs::path currentPathRaw = fs::path(linebufferClean);
        fs::path currentPathClean;
        if (currentPathRaw.is_relative()) {
            currentPathClean = _listfileDirectory;
            currentPathClean /= currentPathRaw;
            currentPathClean = fs::canonical(currentPathClean);
        }
        else {
            currentPathClean = fs::canonical(currentPathRaw);
        }

        try {
            if (!fs::exists(currentPathClean) || !fs::is_regular_file(currentPathClean)) {
                _logOut << currentPathClean << " is not a valid file." << std::endl;
                continue;
            }
        }
        catch (const std::exception& e) {
            _logOut << "Exception checking filepath " << currentPathClean << ": " << e.what() << std::endl;
            continue;
        }

        size_t filesize = 0;
        try {
            filesize = fs::file_size(currentPathClean);
        }
        catch (const std::exception& e) {
            _logOut << "Exception while retrieving file size of " << currentPathClean << ": " << e.what() << std::endl;
            continue;
        }

        std::string filename = currentPathClean.filename().string();
        std::string searchFilename = implementCiPolicy(filename, _options.ciPolicy);
        auto it = result.dictionary.find(searchFilename);
        if (it != result.dictionary.end()) {
            _logOut << "Cannot insert " << currentPathClean << ": filename \"" << searchFilename << "\" already exists! Skipping..." << std::endl;
            continue;
        }
        result.dictionary.insert({ searchFilename, result.entries.size() });
        result.entries.push_back({ currentPathClean, filename, filesize, ren::calculateBlockCount<xrp::blockSize>(filesize) });
        result.filenameListSize += (filename.size() + 1);

        _logOut << "Inserting " << currentPathClean << " as \"" << searchFilename << '\"' << std::endl;
        _logOut.width(0);
    }
    throw std::runtime_error(__FUNCTION__ "() syntax error: reached end of list file without file list end marker ([END]).");
}

fs::path firstFittingFilepath(const fs::path& _preferredPath, bool _fileMayExist, size_t _tryCount)
{
    fs::path pathExtensionRemoved = _preferredPath;
    pathExtensionRemoved.replace_extension();
    fs::path currentPath = _preferredPath;
    for (size_t i = 0; i < _tryCount; i++) {
        try {
            if (!fs::exists(currentPath)) {
                return currentPath;
            }
            if (_fileMayExist && fs::is_regular_file(currentPath)) {
                return currentPath;
            }
        }
        catch (...) { }
        currentPath = pathExtensionRemoved;
        currentPath += std::to_string(i);
        currentPath += _preferredPath.extension();
    }
    std::string msg = "Could not find an appropriate filename on the basis of \"";
    msg += _preferredPath.string();
    msg += "\".";
    throw std::out_of_range(msg);
}

int main(int argc, char** argv)
{
    std::ofstream logfile("xrp.log", std::ios::app | std::ios::ate);
    if (!logfile.is_open()) {
        std::printf("ERROR: Could not create logfile. Aborting now. Press ENTER to continue...\n");
        char buffer[32] = {};
        fgets(buffer, sizeof(buffer) - 1, stdin);
        return 0;
    }

    logfile << "=== XRP Resource Packer: session start ===" << std::endl;

    if (argc < 2) {
        logfile << "No path to a packing list was provided. Exiting..." << std::endl;
        return 0;
    }

    std::string packlistFilepathString = argv[1];
    fs::path packlistFilepath = fs::path(packlistFilepathString);
    if (!fs::exists(packlistFilepath) || !fs::is_regular_file(packlistFilepath)) {
        logfile << packlistFilepathString << " is not a valid file! Exiting..." << std::endl;
        return 0;
    }
    std::ifstream listfile(packlistFilepathString, std::ios::beg);
    if (!listfile.is_open()) {
        logfile << packlistFilepathString << " could not be opened! Exiting..." << std::endl;
        return 0;
    }

    fs::path basePath = fs::canonical(packlistFilepath.parent_path());

    std::string linebufferRaw;
    std::string linebufferClean;
    std::getline(listfile, linebufferRaw);
    linebufferClean = ren::st::trim(linebufferRaw, "\t ");
    if (ren::st::stricmp(linebufferClean, xrp::packerIdentifier) != 0) {
        logfile << '\"' << packlistFilepathString << "\" is not a proper packing list. Exiting..." << std::endl;
        return 0;
    }

    PacklistOptions packlistOptions = {};
    PacklistContent packlistContent = {};
    bool fileListRead = false;

    try {
        while (!listfile.eof()) {
            std::getline(listfile, linebufferRaw);
            linebufferClean = ren::st::trim(linebufferRaw, xrp::trimDefault);
            if (linebufferClean.empty()) {
                continue;
            }
            std::string linebufferUpper = ren::st::toUpper(linebufferClean);
            if (!fileListRead && linebufferUpper == xrp::optionsStartSign) {
                packlistOptions = readPacklistOptionsSection(listfile, logfile);
                continue;
            }
            if (linebufferUpper == xrp::listStartSign) {
                packlistContent = readPacklistFilesSection(listfile, basePath, packlistOptions, logfile);
                fileListRead = true;
                break;
            }
        }
    }
    catch (const std::exception& e) {
        logfile << "Caught an exception while parsing \"" << packlistFilepathString << "\": " << e.what() << "\nExiting..." << std::endl;
        return 0;
    }
    listfile.close();

    std::vector<char> filenameList;
    filenameList.resize(ren::align<xrp::listAlignment>(packlistContent.filenameListSize));
    logfile << '\n' << "Creating filename list. Size = " << filenameList.size() << "; raw size = " << packlistContent.filenameListSize << '\n' << std::endl;

    std::vector<xrp::Entry> entryList(packlistContent.entries.size());

    // Prepare the output file stream
    std::ofstream outputFile;
    bool outputSet = false;
    try {
        if (argc >= 3) {
            fs::path binaryPath = fs::path(argv[2]);
            if (fs::exists(binaryPath) && fs::is_regular_file(binaryPath)) {
                size_t binaryFilesize = fs::file_size(binaryPath);
                fs::resize_file(binaryPath, ren::align<xrp::blockSize>(binaryFilesize));
                outputFile.open(binaryPath.string(), std::ios::app | std::ios::ate | std::ios::binary);
                if (!outputFile.is_open()) {
                    throw std::runtime_error("Could not open executable file.");
                }
                outputSet = true;
                logfile << "Appending executable file " << binaryPath << "..." << std::endl;
            }
        }
        if (!outputSet) {
            fs::path outputFilename = packlistFilepath;
            outputFilename.replace_extension(fs::path(xrp::packerFilenameExtension));
            outputFilename = firstFittingFilepath(outputFilename, true, 10);
            outputFile.open(outputFilename.string(), std::ios::trunc | std::ios::beg | std::ios::binary);
            if (!outputFile.is_open()) {
                throw std::runtime_error("Could not create packer output file.");
            }
            logfile << "Creating new packer output file " << outputFilename << "..." << std::endl;
        }
    }
    catch (const std::exception& e) {
        logfile << "Exception while preparing output: " << e.what() << ". Exiting..." << std::endl;
        return 0;
    }

    size_t currentBlockPosition = 0;
    size_t currentFilenamePosition = 0;
    try {
        for (size_t i = 0; i < packlistContent.entries.size(); i++) {
            const PrebuildEntry& currentPrebuildEntry = packlistContent.entries[i];
            std::ifstream currentFile(currentPrebuildEntry.filepath, std::ios::ate | std::ios::binary);
            if (!currentFile.is_open()) {
                throw std::runtime_error(std::string("Error opening file \"") + currentPrebuildEntry.filepath.string() + "\".");
            }

            size_t currentFilesize = currentFile.tellg();
            entryList[i] = { (uint32_t)currentBlockPosition, (uint32_t)currentFilesize, (uint32_t)currentFilenamePosition };

            currentFile.seekg(0, std::ios::beg);
            size_t fileBlockCount = ren::calculateBlockCount<xrp::blockSize>(currentFilesize);
            std::vector<char> fileData(fileBlockCount * xrp::blockSize);
            currentFile.read(fileData.data(), currentFilesize);
            outputFile.write(fileData.data(), fileData.size());
            currentBlockPosition += fileBlockCount;

            std::memcpy(&filenameList[currentFilenamePosition], currentPrebuildEntry.filename.c_str(), currentPrebuildEntry.filename.size());
            currentFilenamePosition += (currentPrebuildEntry.filename.size() + 1);            
        }
    }
    catch (const std::exception& e) {
        logfile << "Exception while building packer output: " << e.what() << ". Number of blocks written: " << currentBlockPosition << ". Exiting..." << std::endl;
        return 0;
    }

    xrp::DictionaryHeader dictHeader = {
        currentBlockPosition,
        xrp::entryArrayDefaultOffset,
        entryList.size(),
        ren::align<xrp::listAlignment>(xrp::entryArrayDefaultOffset + entryList.size() * sizeof(xrp::Entry)),
        filenameList.size(),
        (packlistOptions.ciPolicy == CaseSensitivityPolicy::CaseInsensitive ? xrp::OF_CaseInsensitive : 0)
    };

    size_t dictionaryBlockCount = ren::calculateBlockCount<xrp::blockSize>(
        dictHeader.namelistOffset + dictHeader.namelistSize + sizeof(xrp::Footer)
    );

    xrp::Footer footer = { dictionaryBlockCount, xrp::fourCC };

    std::vector<char> dictionaryData(dictionaryBlockCount* xrp::blockSize);
    std::memcpy(&dictionaryData[0], &dictHeader, sizeof(dictHeader));
    std::memcpy(&dictionaryData[dictHeader.entriesArrayOffset], entryList.data(), entryList.size() * sizeof(xrp::Entry));
    std::memcpy(&dictionaryData[dictHeader.namelistOffset], filenameList.data(), filenameList.size());
    std::memcpy(dictionaryData.data() + dictionaryData.size() - sizeof(footer), &footer, sizeof(footer));

    outputFile.write(dictionaryData.data(), dictionaryData.size());
    logfile << "Dictionary written.\n" << dictHeader.archiveBlockCount + footer.dictionaryBlockCount << " blocks ("
        << (dictHeader.archiveBlockCount + footer.dictionaryBlockCount) * xrp::blockSize << " bytes) written." << std::endl;

    logfile << "=== XRP Resource Packer: session end ===\n" << std::endl;

    return 0;
}

