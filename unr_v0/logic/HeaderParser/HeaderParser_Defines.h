#ifndef _DEFINE_WINRAR_HEADERS_H_
#define _DEFINE_WINRAR_HEADERS_H_

#pragma pack(push, 1)
typedef struct MARK_HEAD_WINRAR {
	uint16_t HEAD_CRC;
	uint8_t HEAD_TYPE;
	uint16_t HEAD_FLAGS;
	uint16_t HEAD_SIZE;
} MARK_HEADER;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct MAIN_HEAD_WINRAR {
	uint16_t HEAD_CRC;
	uint8_t HEAD_TYPE;
	uint16_t HEAD_FLAGS;
	uint16_t HEAD_SIZE;
	uint16_t RESERVED1;
	uint32_t RESERVED2;
} MAIN_HEADER;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct BLOCK_HEADER_WINRAR {
	uint16_t HEAD_CRC;
	uint8_t HEAD_TYPE;
	uint16_t HEAD_FLAGS;
	uint16_t HEAD_SIZE;
	uint32_t PACK_SIZE;
	uint32_t UNP_SIZE;
	uint8_t HOST_OS;
	uint32_t FILE_CRC;
	uint32_t FTIME;
	uint8_t UNP_VER;
	uint8_t METHOD;
	uint16_t NAME_SIZE;
	uint32_t ATTR;
} BLOCK_HEADER;
#pragma pack(pop)

#endif