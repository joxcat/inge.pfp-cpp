#ifndef __PFP_BINARYTOOLS_HPP__
#define __PFP_BINARYTOOLS_HPP__

#include <sys/_stdint.h>

uint32_t uint8_arr_to_uint32(uint8_t *buffer);
void uint32_to_uint8_arr(uint8_t *dest, uint32_t src);

#endif // __PFP_BINARYTOOLS_HPP__