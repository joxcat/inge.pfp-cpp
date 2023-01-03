#include <memory.h>
#include <string>
#include "pfp/crypto.hpp"
#include "aes.hpp"

Crypto::~Crypto() {
	delete this->key;
	delete &this->ctx;
}

Crypto::Crypto(char * device_id, uint32_t current_time) {
	// TODO: Make this more secure and derive a unique key from the device id and current time
	uint32_t dev_id = strtoul(device_id, nullptr, 10);
	const char * raw_key = std::to_string(current_time % dev_id).c_str();
	int padding = (int)strlen(raw_key) % 16;
	if (padding != 0) padding = 16 - padding;
	const char * key = std::string(raw_key).append(std::string(padding, '0')).c_str();

	Crypto((uint8_t const *)key);
}

Crypto::Crypto(uint8_t const *key) {
  AES_ctx ctx;
	AES_init_ctx(&ctx, key);
	
	this->key = key;
	this->ctx = ctx;
}

char* Crypto::encrypt(char *data) {
	int padding = (int)strlen(data) % 16;
	if (padding != 0) padding = 16 - padding;

	char *encrypted = new char[strlen(data) + padding];
	const char * padded_data = std::string(data).append(std::string(padding, '\0')).c_str();

	AES_ECB_encrypt(&this->ctx, (uint8_t*)padded_data);
	memcpy(encrypted, padded_data, strlen(padded_data));
	return encrypted;
}

char* Crypto::decrypt(char *data) {
	int padding = (int)strlen(data) % 16;
	if (padding != 0) padding = 16 - padding;

	char *decrypted = new char[strlen(data) + padding];
	const char * padded_data = std::string(data).append(std::string(padding, '\0')).c_str();

	AES_ECB_decrypt(&this->ctx, (uint8_t*)padded_data);
	memcpy(decrypted, padded_data, strlen(padded_data));
	return decrypted;
}
