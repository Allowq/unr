#ifndef _CRC32b_CALCULATE_H_
#define _CRC32b_CALCULATE_H_

#pragma once

#include <fstream>
#include "CRC32b_Consts.h"
#include "../../utility/Logger.h"

class CRC32bCalculate
{
	static const uint32_t max_size_of_part = 0x0000FFFF;

private:
	std::basic_ifstream<uint8_t> inFile;
	size_t inFileSize;

	uint8_t raw_buffer[max_size_of_part];

	uint_least32_t calculate_crc32(const void *buf, size_t len, uint_least32_t previous_crc = 0xFFFFFFFF);

public:
	CRC32bCalculate();

	bool get_crc_file(uint_least32_t &crc);
	bool get_crc(uint8_t *buffer, size_t buffer_size, uint_least32_t &crc);
	bool set_path_to_file(const char *path_to_archive);
	void show_crc_file();

	virtual ~CRC32bCalculate();
};


/*
	Name  : CRC-32
	Poly  : 0x04C11DB7    x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
	Init  : 0xFFFFFFFF
	Revert: true
	XorOut: 0xFFFFFFFF
	Check : 0xCBF43926 ("123456789")
	MaxLen: 268 435 455 байт (2 147 483 647 бит) - обнаружение одинарных, двойных, пакетных и всех нечетных ошибок
	*/

/*
	Generate CRC Table
	// CRC_Polynom - значение полинома CRC
	// CRCTable[0x100] - таблица
	for (x = 0; x < 0x100; x++) {
	_crc = x;
	for (y = 0; y < 8; y++) {
	_crc = (_crc & (1<<31)) ? ((_crc << 1) ^ CRC_Polynom) : (_crc << 1);
	}
	CRCTable[x] = _crc;
	}
	*/

#endif