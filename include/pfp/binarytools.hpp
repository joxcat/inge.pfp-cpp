#ifndef __PFP_BINARYTOOLS_HPP__
#define __PFP_BINARYTOOLS_HPP__

#include <sys/_stdint.h>

uint32_t uint8_arr_to_uint32(uint8_t *buffer);
void write_uint32_to_uint8_arr(uint8_t *dest, uint32_t src);

bool check_arr_size(uint8_t *buffer, int required_size);

class IncrementalReader {
	private:
		uint8_t *src;
	public:
		int offset;
		IncrementalReader(uint8_t *src, int offset);
		IncrementalReader &read(uint8_t *dest);
		IncrementalReader &read(uint32_t *dest);
		IncrementalReader &read(uint8_t *dest, int length);
};

uint8_t read_uint8(uint8_t *src, int offset);
uint32_t read_uint32(uint8_t *src, int offset);
void read_array(uint8_t *src, int offset, uint8_t *dest, int length);

class IncrementalWriter {
	private:
		uint8_t *dest;
	public:
		int offset;
		IncrementalWriter(uint8_t *dest, int offset);
		IncrementalWriter &write(uint8_t src);
		IncrementalWriter &write(uint32_t src);
		IncrementalWriter &write(uint8_t *src, int length);
};
IncrementalWriter write_uint8(uint8_t *dest, int offset, uint8_t src);
IncrementalWriter write_uint32(uint8_t *dest, int offset, uint32_t src);
IncrementalWriter write_array(uint8_t *dest, int offset, uint8_t src, int length);

#endif // __PFP_BINARYTOOLS_HPP__