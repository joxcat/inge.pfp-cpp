#include <functional>

#include "pfp/binarytools.hpp"

uint32_t uint8_arr_to_uint32(uint8_t *buffer) {
	return buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
}

void write_uint32_to_uint8_arr(uint8_t *dest, uint32_t src) {
	dest[0] = (src >> 24) & 0xFF;
	dest[1] = (src >> 16) & 0xFF;
	dest[2] = (src >> 8) & 0xFF;
	dest[3] = src & 0xFF;
}

bool check_arr_size(uint8_t *buffer, int required_size) {
	if (buffer == nullptr) {
		return false;
	}

	for (int i = 0; i < required_size; i++) {
		if (&buffer[i] == nullptr) {
			return false;
		}
	}

	return true;
}

// Reader
IncrementalReader::IncrementalReader(uint8_t *src, int offset) {
	this->src = src;
	this->offset = offset;
}
IncrementalReader &IncrementalReader::read(uint8_t *dest) {
	*dest = this->src[this->offset];
	this->offset += 1;
	return *this;
}
IncrementalReader &IncrementalReader::read(uint32_t *dest) {
	*dest = uint8_arr_to_uint32(this->src + this->offset);
	this->offset += 4;
	return *this;
}
IncrementalReader &IncrementalReader::read(uint8_t *dest, int length) {
	for (int i = 0; i < length; i++) {
		dest[this->offset + i] = this->src[i];
	}
	this->offset += length;
	return *this;
}

uint8_t read_uint8(uint8_t *src, int offset) {
	uint8_t result;
	IncrementalReader reader(src, offset);
	reader.read(&result);
	return result;
}
uint32_t read_uint32(uint8_t *src, int offset) {
	uint32_t result;
	IncrementalReader reader(src, offset);
	reader.read(&result);
	return result;
}
void read_array(uint8_t *src, int offset, uint8_t *dest, int length) {
	IncrementalReader reader(src, offset);
	reader.read(dest, length);
}


// Writer
IncrementalWriter::IncrementalWriter(uint8_t *dest, int offset) {
	this->dest = dest;
	this->offset = offset;
}
IncrementalWriter &IncrementalWriter::write(uint8_t src) {
	this->dest[this->offset] = src;
	this->offset += 1;
	return *this;
};
IncrementalWriter &IncrementalWriter::write(uint32_t src) {
	write_uint32_to_uint8_arr(this->dest + this->offset, src);
	this->offset += 4;
	return *this;
}
IncrementalWriter &IncrementalWriter::write(uint8_t *src, int length) {
	for (int i = 0; i < length; i++) {
		this->dest[this->offset + i] = src[i];
	}
	this->offset += length;
	return *this;
} 

IncrementalWriter write_uint8(uint8_t *dest, int offset, uint8_t src) {
	IncrementalWriter writer(dest, offset);
	writer.write(src);
	return writer;
}
IncrementalWriter write_uint32(uint8_t *dest, int offset, uint32_t src) {
	IncrementalWriter writer(dest, offset);
	writer.write(src);
	return writer;
}
IncrementalWriter write_array(uint8_t *dest, int offset, uint8_t *src, int length) {
	IncrementalWriter writer(dest, offset);
	writer.write(src, length);
	return writer;
}