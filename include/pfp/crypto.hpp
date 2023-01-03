#ifndef __PFP_CRYPTO_HPP__
#define __PFP_CRYPTO_HPP__

#include "aes.hpp"
#include "MicroBit.h"

class Crypto {
    private:
        AES_ctx ctx;

    public:
        uint8_t const *key;
        Crypto(char * device_id, uint32_t current_time);
        Crypto(uint8_t const *key);
        ~Crypto();
        char* encrypt(char *data);
        char* decrypt(char *data);
};

#endif // __PFP_CRYPTO_HPP__