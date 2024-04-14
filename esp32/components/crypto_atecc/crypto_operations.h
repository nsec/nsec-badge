#pragma once

void print_16(uint8_t* data);
void encrypt_flag(uint8_t flag[16], uint8_t (&encrypted_flag)[16]);
void decrypt_flag(uint8_t encrypted_flag[16], uint8_t (&flag)[16]);
void print_config(atecc608_config_t * pConfig, uint16_t custom_param);
