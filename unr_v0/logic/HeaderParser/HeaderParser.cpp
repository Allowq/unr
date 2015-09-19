#include "HeaderParser.h"

HeaderParser::HeaderParser() : inFileSize(0), data_block_size(DATA_BLOCK_SIZE) {
	memset(&mark_header, 0, sizeof(mark_header));
	memset(&main_header, 0, sizeof(main_header));
	memset(&block_header, 0, sizeof(block_header));
	memset(&data_block, 0, sizeof(data_block));
}

bool HeaderParser::block_is_ended() {
	if (block_header.HEAD_CRC == 0x3DC4 && block_header.HEAD_TYPE == 0x7B) 
		return true;
	
	return false;
}

const char * HeaderParser::byte_to_binary(int x)
{
	static char b[9];
	b[0] = '\0';

	int z;
	for (z = 128; z > 0; z >>= 1)
	{
		strcat(b, ((x & z) == z) ? "1" : "0");
	}

	return b;
}

bool HeaderParser::get_archive_open_flag() {
	return inFile.is_open();
}

bool HeaderParser::parse_local_blocks(uint64_t &sizeValue) {
	uint32_t blockCounter = 0;
	while (inFile) {
		switch (read_local_block(sizeValue)) {
		case 2:
			show_local_block(blockCounter, sizeValue);
			continue;
		case 0:
			return false;
		case 1:
			printf("\n\nParsing WinRAR archive ended.\n");
		}
		break;
	}
	printf("\nShowing struct of headers for WinRAR archive has been ended.\n");
	//printf("Parsing size data - %d", sizeValue); 
	getchar();
	return true;
}

void HeaderParser::print_bits(size_t const size, void const * const ptr) {
    uint8_t *b = (uint8_t*) ptr;
    uint8_t byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = b[i] & (1<<j);
            byte >>= j;
            printf("%u", byte);
        }
    }
    puts("");
}

uint8_t HeaderParser::read_local_block(uint64_t &sizeValue) {
	if (sizeValue + 7 <= inFileSize) {
		/*
		inFile.read(data_block, sizeof(block_header));
		memcpy(&block_header, data_block, sizeof(block_header));
		*/

		inFile.read(data_block, 7);
		memcpy(&block_header, data_block, 7);
		if (block_is_ended()) {
			sizeValue += 7;
			return 1;
		}

		inFile.seekg(-7, std::ios::cur);
		inFile.read(data_block, sizeof(block_header));
		memcpy(&block_header, data_block, sizeof(block_header));
		sizeValue += sizeof(block_header);

		return 2;
	}

	return 0;
}

bool HeaderParser::set_path_to_archive(const char *path_to_archive) {
	std::ios_base::iostate exceptionMask = inFile.exceptions() | std::ios::failbit;
	try {
		inFile.open(path_to_archive, std::ios_base::ate | std::ios_base::_Nocreate | std::ios_base::binary);
		if (inFile.is_open() && inFile.good()) {
			std::streampos length = inFile.tellg();
			inFileSize = length;
			inFile.seekg(0, std::ios::beg);
			if (length > 0 && length < UINT64_MAX)
				return true;
			else {
				inFile.close();
				inFile.clear();
			}
		}
		else if (inFile.fail() || inFile.bad()) {
			Logger::instance().log("Catch error in HeaderParser::set_path_to_archive()", Logger::kLogLevelError);
			Logger::instance().log(strerror(errno), Logger::kLogLevelError);
		}
	}
	catch (std::ios_base::failure& e) {
		Logger::instance().log("Catch error in HeaderParser::set_path_to_archive()", Logger::kLogLevelError);
		Logger::instance().log(e.what(), Logger::kLogLevelError);
	}

	return false;
}

void HeaderParser::show_archive_headers() {
	if (inFile.is_open()) {
		uint64_t tempSize = 0;
		if (!show_mark_header(tempSize)) {
			throw("Catch error in HeaderParser::show_archive_headers(). Bad mark header struct.");
			return;
		}

		if (!show_main_header(tempSize)) {
			throw("Catch error in HeaderParser::show_archive_headers(). Bad main header struct.");
			return;
		}

		if (!parse_local_blocks(tempSize))
			throw("Catch error in HeaderParser::show_archive_headers(). Bad local header struct.");
	}
	else
		throw("Catch error in HeaderParser::show_archive_headers(). WinRAR archive not opened.");
}

void HeaderParser::show_block_flags(bool &salt_is_set, bool &ext_time_is_set) {
	printf("Head CRC: 0x%04x\n", block_header.HEAD_CRC);
	uint8_t tempByte = 0x00;
	printf("Flags[0]: ");
	tempByte = (block_header.HEAD_FLAGS >> 8) & 0xFF;
	print_bits(sizeof(uint8_t), &tempByte);
	printf("Flags[1]: ");
	print_bits(sizeof(uint8_t), &block_header.HEAD_FLAGS);
	if (block_header.HEAD_FLAGS & 0x400)
		salt_is_set = true;
	if (block_header.HEAD_FLAGS & 0x1000)
		ext_time_is_set = true;
}

void HeaderParser::show_local_block(uint32_t &blockCounter, uint64_t &sizeValue) {
	printf("\nShow local block #%d:\n\n", blockCounter);

	uint8_t *fileName = new uint8_t[block_header.NAME_SIZE+1];
	inFile.read(fileName, block_header.NAME_SIZE);
	sizeValue += block_header.NAME_SIZE;

	fileName[block_header.NAME_SIZE] = '\0';
	switch (block_header.ATTR){
	case 0x20:
		printf("Name of file: %s\n", fileName);
		break;
	case 0x10:
		printf("Name of directory: %s\n", fileName);
		break;
	}
	printf("Name size: %d byte\n", block_header.NAME_SIZE);
	printf("Head CRC: 0x%04x\n", block_header.HEAD_CRC);

	bool salt_is_set = false, ext_time_is_set = false;
	show_block_flags(salt_is_set, ext_time_is_set);

	printf("File size before compression: %08d byte\n", block_header.UNP_SIZE);

	printf("Operating system used for archiving: ");
	switch (block_header.HOST_OS)
	{
	case 0: 
		printf("MS-DOS\n");
		break;
	case 1: 
		printf("OS/2\n");
		break;
	case 2: 
		printf("Windows 32bit\n");
		break;
	case 3: 
		printf("UNIX\n");
		break;
	case 4: 
		printf("Mac OS\n");
		break;
	case 5: 
		printf("BeOS\n");
		break;
	default: 
		printf("Unknown system\n");
		break;
	}

	printf("File CRC: 0x%08x\n", block_header.FILE_CRC);
	printf("FTIME: %08d\n", block_header.FTIME);
	printf("Unpacking version: 0x%02d\n", block_header.UNP_VER);
	printf("Compression method: ");
	switch (block_header.METHOD){
	case 0x30: 
		printf("storing/uncompressed\n");
		break;
	case 0x31: 
		printf("fastest compression\n");
		break;
	case 0x32: 
		printf("fast compression\n");
		break;
	case 0x33: 
		printf("normal compression\n");
		break;
	case 0x34: 
		printf("good compression\n");
		break;
	case 0x35: 
		printf("best compression\n");
		break;
	default: 
		printf("unknown compression\n");
		break;
	}

	if (ext_time_is_set) {
		inFile.seekg(5, std::ios::cur);
		sizeValue += 5;
	}
	if (salt_is_set) {
		inFile.seekg(8, std::ios::cur);
		sizeValue += 8;
	}

	printf("File size after compression: %08d byte\n", block_header.PACK_SIZE);

	if (sizeof(data_block) >= block_header.PACK_SIZE) {
		printf("\nFile buffer:\n");
		inFile.read(data_block, block_header.PACK_SIZE);
		for (uint32_t i = 0; i < block_header.PACK_SIZE; i++) {
			printf("%s - 0x%x\n", byte_to_binary(data_block[i]), data_block[i] & 0xFF);
		}
	} else
		inFile.seekg(block_header.PACK_SIZE, std::ios::cur);

	sizeValue += block_header.PACK_SIZE;

	blockCounter++;
}

bool HeaderParser::show_main_header(uint64_t &sizeValue) {
	if (sizeValue + sizeof(main_header) <= inFileSize) {
		inFile.read(data_block, sizeof(main_header));
		memcpy(&main_header, data_block, sizeof(main_header));
		sizeValue += sizeof(main_header);

		printf("\nShow main headers block:\n");
		printf("HEAD CRC - 0x%04x\n", main_header.HEAD_CRC);
		printf("HEAD FLGS - 0x%04x\n", main_header.HEAD_FLAGS);
		printf("HEAD SIZE - 0x%04x\n", main_header.HEAD_SIZE);
		printf("HEAD TYPE - 0x%02x\n", main_header.HEAD_TYPE);
		printf("RESERVED1 - 0x%04x\n", main_header.RESERVED1);
		printf("RESERVED2 - 0x%08x\n", main_header.RESERVED2);
		getchar();
		return true;
	}

	return false;
}

bool HeaderParser::show_mark_header(uint64_t &sizeValue) {
	if (inFileSize >= sizeof(mark_header)) {
		inFile.read(data_block, sizeof(mark_header));
		memcpy(&mark_header, data_block, sizeof(mark_header));
		sizeValue += sizeof(mark_header);

		system("cls");
		printf("Show mark headers block:\n");
		printf("HEAD CRC - 0x%04x\n", mark_header.HEAD_CRC);
		printf("HEAD FLAGS - 0x%02x\n", mark_header.HEAD_FLAGS);
		printf("HEAD SIZE - 0x%04x\n", mark_header.HEAD_SIZE);
		printf("HEAD TYPE - 0x%02x\n", mark_header.HEAD_TYPE);
		getchar();
		return true;
	}

	return false;
}

HeaderParser::~HeaderParser(void) {
}
