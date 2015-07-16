#ifndef _HEADER_PARSER_H_
#define _HEADER_PARSER_H_

#pragma once

#include <fstream>
#include <iostream>
#include <cstdio>
#include "../utility/Logger.h"
#include "define_winrar_headers.h"

class HeaderParser
{
#define DATA_BLOCK_SIZE 65536

	std::basic_ifstream<uint8_t> inFile; 
	uint64_t inFileSize;

	MARK_HEADER mark_header;
	MAIN_HEADER main_header;
	BLOCK_HEADER block_header;

	uint8_t data_block[DATA_BLOCK_SIZE];
	uint16_t data_block_size;

	bool block_is_ended();
	uint8_t read_local_block(uint64_t &sizeValue);
	bool parse_local_blocks(uint64_t &sizeValue);
	void print_bits(size_t const size, void const * const ptr);
	void show_block_flags(bool &salt_is_set, bool &ext_time_is_set);
	void show_local_block(uint32_t &blockCounter, uint64_t &sizeValue);
	bool show_main_header(uint64_t &sizeValue);
	bool show_mark_header(uint64_t &sizeValue);
	
public:
	explicit HeaderParser();

	bool get_archive_open_flag();
	bool set_path_to_archive(const char *path_to_archive);
	void show_archive_headers();

	virtual ~HeaderParser(void);
};

#endif
