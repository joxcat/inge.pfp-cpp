#include "pfp/binarytools.hpp"

uint32_t uint8_arr_to_uint32(uint8_t *buffer) {
	return buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
}

void uint32_to_uint8_arr(uint8_t *dest, uint32_t src) {
	dest[0] = (src >> 24) & 0xFF;
	dest[1] = (src >> 16) & 0xFF;
	dest[2] = (src >> 8) & 0xFF;
	dest[3] = src & 0xFF;
}