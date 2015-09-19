#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <cstdio>
#include <cstdint>
#include <string>

#include "utility\getopt.h"
#include "utility\Logger.h"

#include "logic\HeaderParser\HeaderParser.h"
#include "logic\CRC32b\CRC32b_Calculate.h"
#include "logic\SHA1\SHA1_Calculate.h"
#include "logic\Huffman\Huffman.h"

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

#endif