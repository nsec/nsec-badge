#pragma once

#include "cryptoauthlib.h"

void print_16(uint8_t* data);
void print_16str(uint8_t* data);
void print_bin2hex(uint8_t* data, size_t size);
void encrypt_flag(uint8_t flag[16], uint8_t (&encrypted_flag)[16]);
void decrypt_flag(uint8_t encrypted_flag[16], uint8_t (&flag)[16]);
void print_config(atecc608_config_t * pConfig, uint16_t custom_param);
