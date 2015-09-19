#include "SHA1_Calculate.h"


SHA1_Calculate::SHA1_Calculate() : inFileSize(0) {
}

// Sets the first 16 integers in the buffert to zero.
// Used for clearing the W buffert.
inline void SHA1_Calculate::clearWBuffert(unsigned int* buffert) {
	for (int pos = 16; --pos >= 0;)
	{
		buffert[pos] = 0;
	}
}

bool SHA1_Calculate::get_sha1_file(unsigned char *hash) {
	if (inFile.is_open()) {
		if (hash) {
			int size = 0;
			
			if (inFileSize > max_size_length_buffer)
				size = max_size_length_buffer;
			else
				size = inFileSize;

			try {
				inFile.read(raw_buffer, size);
				locale_calculate(&raw_buffer[0], size, hash);
				return true;
			}
			catch (...) {
				throw("Catch error in SHA1_Calculate::get_sha1_file(). Calculate SHA1 failed with error.");
			}
		} else
			throw("Catch error in SHA1_Calculate::get_sha1_file(). Pointer to input buffer is NULL.");
		
	} else
		throw("Catch error in SHA1_Calculate::get_sha1_file(). File with raw text not opened.");

	return false;
}

bool SHA1_Calculate::get_sha1(const void *buffer, const size_t buffer_size, unsigned char *hash) {
	if (buffer) {
		int size = 0;

		if (buffer_size > max_size_length_buffer)
			size = max_size_length_buffer;
		else
			size = buffer_size;

		try {
			locale_calculate(buffer, size, hash);
			return true;
		}
		catch (...) {
			throw("Catch error in SHA1_Calculate::get_sha1_file(). Calculate SHA1 failed with error.");
		}
	}
	else
		throw("Catch error in SHA1_Calculate::get_sha1(). Pointer to input buffer is NULL.");

	return false;
}

bool SHA1_Calculate::get_sha1_for_winrar(const void *buffer, const size_t buffer_size, uint8_t *hash) {
	if (buffer) {
		if (!hash) 
			hash = new uint8_t[20];
		uint8_t *hash_after = new uint8_t[20];

		int size = 0, index = 0;

		try {
			locale_calculate(buffer, buffer_size, hash);

			while (index < number_of_conversions - 1) {
				memcpy(hash_after, hash, sizeof(hash_after));
				locale_calculate(hash, sizeof(hash), hash_after);
				index++;
			}
			memcpy(hash, hash_after, sizeof(hash_after));

			/*
			char hex_string[41];
			toHexString(hash, hex_string);
			std::cout << "SHA1: " << hex_string << std::endl;
			getchar();
			*/

			if (hash_after) {
				delete[] hash_after;
				hash_after = NULL;
			}

			return true;
		}
		catch (...) {
			if (hash_after) {
				delete[] hash_after;
			}
			throw("Catch error in SHA1_Calculate::get_sha1_file(). Calculate SHA1 failed with error.");
		}
	}
	else
		throw("Catch error in SHA1_Calculate::get_sha1(). Pointer to input buffer is NULL.");

	return false;
}

void SHA1_Calculate::innerHash(unsigned int* result, unsigned int* w)
{
	unsigned int a = result[0];
	unsigned int b = result[1];
	unsigned int c = result[2];
	unsigned int d = result[3];
	unsigned int e = result[4];

	int round = 0;

#define sha1macro(func,val) \
				{ \
                const unsigned int t = rol(a, 5) + (func) + e + val + w[round]; \
				e = d; \
				d = c; \
				c = rol(b, 30); \
				b = a; \
				a = t; \
				}

	while (round < 16)
	{
		sha1macro((b & c) | (~b & d), 0x5a827999)
			++round;
	}
	while (round < 20)
	{
		w[round] = rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
		sha1macro((b & c) | (~b & d), 0x5a827999)
			++round;
	}
	while (round < 40)
	{
		w[round] = rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
		sha1macro(b ^ c ^ d, 0x6ed9eba1)
			++round;
	}
	while (round < 60)
	{
		w[round] = rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
		sha1macro((b & c) | (b & d) | (c & d), 0x8f1bbcdc)
			++round;
	}
	while (round < 80)
	{
		w[round] = rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
		sha1macro(b ^ c ^ d, 0xca62c1d6)
			++round;
	}

#undef sha1macro

	result[0] += a;
	result[1] += b;
	result[2] += c;
	result[3] += d;
	result[4] += e;
}

void SHA1_Calculate::locale_calculate(const void* src, const int bytelength, unsigned char* hash) {
	// Init the result array.
	unsigned int result[5] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };

	// Cast the void src pointer to be the byte array we can work with.
	const unsigned char* sarray = (const unsigned char*)src;

	// The reusable round buffer
	unsigned int w[80];

	// Loop through all complete 64byte blocks.
	const int endOfFullBlocks = bytelength - 64;
	int endCurrentBlock;
	int currentBlock = 0;

	while (currentBlock <= endOfFullBlocks)
	{
		endCurrentBlock = currentBlock + 64;

		// Init the round buffer with the 64 byte block data.
		for (int roundPos = 0; currentBlock < endCurrentBlock; currentBlock += 4)
		{
			// This line will swap endian on big endian and keep endian on little endian.
			w[roundPos++] = (unsigned int)sarray[currentBlock + 3]
				| (((unsigned int)sarray[currentBlock + 2]) << 8)
				| (((unsigned int)sarray[currentBlock + 1]) << 16)
				| (((unsigned int)sarray[currentBlock]) << 24);
		}
		innerHash(result, w);
	}

	// Handle the last and not full 64 byte block if existing.
	endCurrentBlock = bytelength - currentBlock;
	clearWBuffert(w);
	int lastBlockBytes = 0;
	for (; lastBlockBytes < endCurrentBlock; ++lastBlockBytes)
	{
		w[lastBlockBytes >> 2] |= (unsigned int)sarray[lastBlockBytes + currentBlock] << ((3 - (lastBlockBytes & 3)) << 3);
	}
	w[lastBlockBytes >> 2] |= 0x80 << ((3 - (lastBlockBytes & 3)) << 3);
	if (endCurrentBlock >= 56)
	{
		innerHash(result, w);
		clearWBuffert(w);
	}
	w[15] = bytelength << 3;
	innerHash(result, w);

	// Store hash in result pointer, and make sure we get in in the correct order on both endian models.
	for (int hashByte = 20; --hashByte >= 0;)
	{
		hash[hashByte] = (result[hashByte >> 2] >> (((3 - hashByte) & 0x3) << 3)) & 0xff;
	}
}

// Rotate an integer value to left.
inline const unsigned int SHA1_Calculate::rol(const unsigned int value, const unsigned int steps) {
	return ((value << steps) | (value >> (32 - steps)));
}

bool SHA1_Calculate::set_path_to_file(const char *path_to_file) {
	std::ios_base::iostate exceptionMask = inFile.exceptions() | std::ios::failbit;
	try {
		inFile.open(path_to_file, std::ios_base::ate | std::ios_base::_Nocreate | std::ios_base::binary);
		if (inFile.is_open() && inFile.good()) {
			std::streampos length = inFile.tellg();
			inFileSize = int(length);
			inFile.seekg(0, std::ios::beg);
			if (length > 0 && length < UINT64_MAX)
				return true;
			else {
				inFile.close();
				inFile.clear();
			}
		}
		else if (inFile.fail() || inFile.bad()) {
			Logger::instance().log("Catch error in SHA1_Calculate::set_path_to_file()", Logger::kLogLevelError);
			Logger::instance().log(strerror(errno), Logger::kLogLevelError);
		}
	}
	catch (std::ios_base::failure& e) {
		Logger::instance().log("Catch error in SHA1_Calculate::set_path_to_file()", Logger::kLogLevelError);
		Logger::instance().log(e.what(), Logger::kLogLevelError);
	}

	return false;
}

void SHA1_Calculate::show_sha1_file() {
	if (inFile.is_open()) {
		uint8_t *hash = new uint8_t[20];
		if (hash) {
			int size = 0;

			if (inFileSize > max_size_length_buffer)
				size = max_size_length_buffer;
			else
				size = inFileSize;

			try {
				inFile.read(raw_buffer, size);
				locale_calculate(&raw_buffer[0], size, hash);
				char hex_string[41];
				toHexString(hash, hex_string);
				std::cout << "SHA1: " << hex_string << std::endl;
				getchar();
			}
			catch (...) {
				throw("Catch error in SHA1_Calculate::show_sha1_file(). Calculate SHA1 failed with error.");
			}
		}
		else
			throw("Catch error in SHA1_Calculate::show_sha1_file(). Pointer to input buffer is NULL.");

	}
	else
		throw("Catch error in SHA1_Calculate::show_sha1_file(). File with raw text not opened.");
}

void SHA1_Calculate::toHexString(const unsigned char* hash, char* hexstring) {
	const char hexDigits[] = { "0123456789abcdef" };

	for (int hashByte = 20; --hashByte >= 0;)
	{
		hexstring[hashByte << 1] = hexDigits[(hash[hashByte] >> 4) & 0xf];
		hexstring[(hashByte << 1) + 1] = hexDigits[hash[hashByte] & 0xf];
	}
	hexstring[40] = 0;
}

SHA1_Calculate::~SHA1_Calculate() {
}
