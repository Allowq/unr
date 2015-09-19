#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_

#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <stdint.h>

#include "../../utility/Logger.h"
#include "Queue.h"
#include "Tree.h"

class Huffman
{
	static const uint32_t max_size_of_part = 0x0000FFFF;

	size_t inFileSize;
	std::ifstream inFile;

	uint8_t raw_buffer[max_size_of_part];

	bool verbose_mode;

	
	unsigned char huf_read(std::ifstream & infile);
	void huf_write(unsigned char i, std::ofstream & outfile);
	size_t Huffman::huf_write(uint8_t i, uint8_t *buffer, size_t size);

public:
	Huffman();

	void encode_into_file(std::string ifile, std::string ofile, bool verbose);
	bool encode_from_file(uint8_t *data, size_t *size);
	void decode_into_file(std::string ifile, std::string ofile, bool verbose);
	bool set_path_to_file(const char *path_to_archive);

	virtual ~Huffman();
};

#endif
