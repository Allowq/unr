#include "CRC32b_Calculate.h"

CRC32bCalculate::CRC32bCalculate() : inFileSize(0) {
}

uint_least32_t CRC32bCalculate::calculate_crc32(const void *buf, size_t len, uint_least32_t previous_crc) {
	uint32_t* current = (uint32_t*)buf;
	uint_least32_t crc = ~previous_crc; // same as previousCrc32 ^ 0xFFFFFFFF

	// process eight bytes at once
	while (len >= 8)
	{
		uint_least32_t one = *current++ ^ crc;
		uint_least32_t two = *current++;
		crc = crc32b_table[7][one & 0xFF] ^
			crc32b_table[6][(one >> 8) & 0xFF] ^
			crc32b_table[5][(one >> 16) & 0xFF] ^
			crc32b_table[4][one >> 24] ^
			crc32b_table[3][two & 0xFF] ^
			crc32b_table[2][(two >> 8) & 0xFF] ^
			crc32b_table[1][(two >> 16) & 0xFF] ^
			crc32b_table[0][two >> 24];
		len -= 8;
	}

	uint8_t* currentChar = (uint8_t*)current;
	// remaining 1 to 7 bytes (standard CRC table-based algorithm)
	while (len--)
		crc = (crc >> 8) ^ crc32b_table[0][(crc & 0xFF) ^ *currentChar++];

	return ~crc; // same as crc ^ 0xFFFFFFFF
}

bool CRC32bCalculate::get_crc_file(uint_least32_t &crc) {
	if (inFile.is_open()) {
		try {
			crc = 0x00;
			size_t pos = 0, size = 0;

			while (inFile && pos < inFileSize) {
				if (inFileSize - pos > max_size_of_part)
					size = max_size_of_part;
				else
					size = inFileSize - pos;

				inFile.read(raw_buffer, size);
				crc = calculate_crc32(raw_buffer, size, crc);

				pos += size;
			}
			return true;
		}
		catch (...) {
			throw("Catch error in CRC32Calculate::get_crc(). Calculate crc32 failed with error.");
		}
	}
	else
		throw("Catch error in CRC32Calculate::get_crc(). File with raw text not opened.");

	return false;
}

bool CRC32bCalculate::get_crc(uint8_t *buffer, size_t buffer_size, uint_least32_t &crc) {
	if (buffer && buffer_size > 0) {
		crc = 0x00;
		size_t pos = 0, size = 0;

		while (pos < buffer_size) {
			if (buffer_size - pos > max_size_of_part)
				size = max_size_of_part;
			else
				size = buffer_size - pos;

			memcpy(raw_buffer, buffer + pos, size);
			crc = calculate_crc32(raw_buffer, size, crc);

			pos += size;
		}
		return true;

	} else
		throw("Catch error in CRC32Calculate::get_crc(). Input buffer not initialized.");

	return false;
}

bool CRC32bCalculate::set_path_to_file(const char *path_to_file) {
	std::ios_base::iostate exceptionMask = inFile.exceptions() | std::ios::failbit;
	try {
		inFile.open(path_to_file, std::ios_base::ate | std::ios_base::_Nocreate | std::ios_base::binary);
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
			Logger::instance().log("Catch error in CRC32Calculate::set_path_to_file()", Logger::kLogLevelError);
			Logger::instance().log(strerror(errno), Logger::kLogLevelError);
		}
	}
	catch (std::ios_base::failure& e) {
		Logger::instance().log("Catch error in CRC32Calculate::set_path_to_file()", Logger::kLogLevelError);
		Logger::instance().log(e.what(), Logger::kLogLevelError);
	}

	return false;
}

void CRC32bCalculate::show_crc_file() {
	if (inFile.is_open()) {
		try {
			uint_least32_t crc = 0x00;
			size_t pos = 0, size = 0;

			while (inFile && pos < inFileSize) {
				if (inFileSize - pos > max_size_of_part)
					size = max_size_of_part;
				else
					size = inFileSize - pos;

				inFile.read(raw_buffer, size);
				crc = calculate_crc32(raw_buffer, size, crc);

				pos += size;
			}
			std::cout << "CRC32: 0x" << std::uppercase << std::hex << crc << std::endl;
		}
		catch (...) {
			throw("Catch error in CRC32Calculate::show_crc(). Calculate crc32 failed with error.");
		}
	} else
		throw("Catch error in CRC32Calculate::show_crc(). File with raw text not opened.");
}

CRC32bCalculate::~CRC32bCalculate() {
}
