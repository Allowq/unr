#ifndef _SHA1CALCULATE_H_
#define _SHA1CALCULATE_H_

#pragma once

#include <fstream>
#include "../../utility/Logger.h"

class SHA1_Calculate
{
	static const int max_size_length_buffer = 127;
	static const int number_of_conversions = 262144;

private:
	std::basic_ifstream<uint8_t> inFile;
	int inFileSize;
	uint8_t raw_buffer[max_size_length_buffer];

	inline void clearWBuffert(unsigned int* buffert);
	void innerHash(unsigned int* result, unsigned int* w);
	void locale_calculate(const void* src, const int bytelength, unsigned char* hash);
	inline const unsigned int rol(const unsigned int value, const unsigned int steps);
	void toHexString(const unsigned char* hash, char* hexstring);

public:
	SHA1_Calculate();

	bool get_sha1_file(uint8_t *hash);
	bool get_sha1_for_winrar(const void *buffer, const size_t buffer_size, uint8_t *hash);
	bool get_sha1(const void *buffer, const size_t buffer_size, uint8_t *hash);
	bool set_path_to_file(const char *path_to_file);
	void show_sha1_file();

	virtual ~SHA1_Calculate();
};

#endif
