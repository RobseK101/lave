#pragma once
#include <cstdint>
#include "ren/Utility.hpp"

namespace xrp
{
	// Fixed block size of 2048 bytes to make this work fast on a 
	// wide range of filesystems.
	constexpr size_t blockSize = 0x800;
	constexpr uint32_t fourCC = ren::makeFourCC("XRP\0");

	struct Footer
	{
		uint32_t dictionaryBlockCount;
		uint32_t fourCC;
	};

	struct DictionaryHeader
	{
		uint32_t archiveBlockCount;
		uint32_t entriesArrayOffset;
		uint32_t entryCount;
		uint32_t namelistOffset;
		uint32_t namelistSize; // Count of chars including last zero
		uint32_t optionsFlags;
	};

	constexpr uint32_t OF_CaseInsensitive = 0x1;

	struct Entry
	{
		uint32_t startBlock;
		uint32_t size;
		uint32_t nameOffset;
	};

#ifdef XRP_BUILDER
	constexpr size_t listAlignment = 8;

	constexpr uint32_t entryArrayDefaultOffset = 32;

	constexpr char packerIdentifier[] = "[XRPLIST]";
	constexpr char optionsStartSign[] = "[OPTIONS]";
	constexpr char listStartSign[] = "[FILES]";
	constexpr char endSign[] = "[END]";

	constexpr char optionsCaseInsensitive[] = "CASE INSENSITIVE";

	constexpr char trimDefault[] = "\t ";

	constexpr char packerFilenameExtension[] = ".xrp";
#endif
}

