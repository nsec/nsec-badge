/* Console example — various system commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "crypto_atecc_cmd.h"

#include "cryptoauthlib.h"
#include "crypto_operations.h"

#include "save.h"

static const char *TAG = "crypto_atecc";
static void print_public_key(uint8_t *pubkey);
static void print_digest(uint8_t* digest, size_t size);
static void print_32(uint8_t* data);
static void print_64(uint8_t* data);

#if CTF_ADDON_ADMIN_MODE

int test_hmac_slot(int slotid) {
    uint8_t digest[ATCA_SHA_DIGEST_SIZE];
    memset(digest, 0, sizeof(digest));
    const char* dataToHash = "abc";
    int ret = 0;
    // for slot 9
    uint8_t start[3] = { 0xca, 0xe9, 0x32 }; // should be the start of the good hash
    if (slotid == 5) {
        // for slot 5
        start[0] = 0x78;
        start[1] = 0x77;
        start[2] = 0x08;
    }

    ret = atcab_sha_hmac(reinterpret_cast<const uint8_t*>(dataToHash), strlen(dataToHash), slotid, digest, SHA_MODE_TARGET_OUT_ONLY);
    if (ret == ATCA_SUCCESS) {
        if (start[0] == digest[0] && start[1] == digest[1] && start[2] == digest[2]) {
            printf("GOOD: HMAC(%d, abc) is correct:\n", slotid);
            print_digest(digest, sizeof(digest));
        } else {
            printf("FAIL: HMAC(%d, abc) is not correct:\n", slotid);
            print_digest(digest, sizeof(digest));
            return 0;
        }
    } else {
        ESP_LOGE(TAG, "FAIL: err atcab_sha_hmac[slotid:%d] ret: %02x", slotid, ret);
        return 0;
    }
    return 1;
}
static int crypto_atecc(int argc, char **argv) {
    uint16_t _select = 1;

    if (argc >= 2) {
        _select = atoi(argv[1]);
    }
    ESP_LOGI(TAG, "Running crypto_atecc %d!", _select);

    uint16_t custom_param = 0;
    if (argc >= 3) {
        custom_param = atoi(argv[2]);
    }

    uint16_t custom_param2 = 0;
    if (argc >= 4) {
        custom_param2 = atoi(argv[3]);
    }

    uint16_t custom_param3 = 0;
    if (argc >= 5) {
        custom_param3 = atoi(argv[4]);
    }

    int ret = 0;
    uint8_t pubkey[ATCA_PUB_KEY_SIZE];

    if (_select == 0) {
        for (int i=0; i <= 4; i++) {
            ESP_LOGI(TAG, "Public key slot %d:", i);
            if (atcab_get_pubkey(i, pubkey) == ATCA_SUCCESS) {
                print_public_key(pubkey);
                print_64(pubkey);
            }
        }
    } else if (_select == 1) {
        uint8_t config_zone[ATCA_ECC_CONFIG_SIZE];
        if (atcab_read_config_zone(config_zone) == ATCA_SUCCESS) {
        for (int i = 0; i < 16; i++) {
            for(int j = 0; j < 8; j++) {
                printf("%02x ", config_zone[i * 8 + j]);
            }
            printf("\n");
            }
        }
    } else if (_select == 2) {
        // crypto 2 9 abc 
        uint8_t digest[ATCA_SHA_DIGEST_SIZE];
        memset(digest, 0, sizeof(digest));
        const char* dataToHash = NULL;
        if (argc == 4) {
            dataToHash = argv[3];
        } else {
            // TODO why [5,7-15] ???
            // Bits 0:3 only are used to select a slot but all 16 bits are used in the HMAC message
            ESP_LOGE(TAG, "err needs 3 params crypto 2 [5,7-15] text");
            return 0;
        }
        int ret = 0;
        // key with all 1s: { 0x08, 0x5b, 0xf5 }

        //uint8_t start[3] = { 0x61, 0x06, 0x75 }; // private key
        //for (int i=65534; i > custom_param; i--) {
            ret = atcab_sha_hmac(reinterpret_cast<const uint8_t*>(dataToHash), strlen(dataToHash), custom_param, digest, SHA_MODE_TARGET_OUT_ONLY);

            //ESP_LOGI(TAG, "atcab_sha_hmac: %02x%02x%02x == %02x%02x%02x ???", start[0], start[1], start[2], digest[0], digest[1], digest[2]);
            if (ret == ATCA_SUCCESS) {
                //if (start[0] == digest[0] && start[1] == digest[1] && start[2] == digest[2]) {
                    ESP_LOGI(TAG, "atcab_sha_hmac[%d]: %02x%02x%02x%02x...", custom_param, digest[0], digest[1], digest[2], digest[3]);
                    print_digest(digest, sizeof(digest));
                    //break;
                //}
            } else {
                ESP_LOGE(TAG, "err atcab_sha_hmac ret: %02x", ret);
            }
            //memset(digest, 0, sizeof(digest));
        //}
    } else if (_select == 3) {
        uint8_t digest[ATCA_SHA_DIGEST_SIZE];
        memset(digest, 0, sizeof(digest));
        uint8_t data[16];
        for(int i=0; i<16; data[i++] = 0xAB);
        int ret = 0;
        ret = atcab_sha(sizeof(data), data, digest);
        if (ret == ATCA_SUCCESS) {
            ESP_LOGI(TAG, "atcab_sha:");
            print_digest(digest, sizeof(digest));
        } else {
            ESP_LOGE(TAG, "err atcab_sha ret: %02x", ret);
        }
        memset(digest, 0, sizeof(digest));

        ESP_LOGI(TAG, "atcab_mac:");
        uint8_t challenge[32];
        for (int i=0; i<32; challenge[i++] = 0x64);
        if (atcab_mac(MAC_MODE_CHALLENGE, custom_param, challenge, digest) == ATCA_SUCCESS)
            print_digest(digest, sizeof(digest));

    } else if (_select == 4) {
        uint8_t rand_out;
        atcab_random(&rand_out);
        ESP_LOGI(TAG, "Today lucky number is: %d", rand_out);
    } else if (_select == 5) {
        uint8_t zone_data[32];
        //                       0 ATCA_ZONE_CONFIG  1 ATCA_ZONE_OTP  2 ATCA_ZONE_DATA 
        if (atcab_read_bytes_zone(custom_param, custom_param2, custom_param3, zone_data, 32) == ATCA_SUCCESS) {
            print_32(zone_data);
        }
    } else if (_select == 6) {
        uint8_t num_in[20];
        for (int i=0; i<20; num_in[i++] = 0x42);
        uint8_t rand_out[32];
        //set any data in TempKey: atcab_nonce()
        if (atcab_nonce_rand(num_in, rand_out) == ATCA_SUCCESS) {
            print_32(rand_out);
            atcab_nonce(rand_out);
        }
    } else if (_select == 7) {
// #define GENDIG_ZONE_CONFIG   Use KeyID to specify any of the four 256-bit blocks of the Configuration zone.
// #define GENDIG_ZONE_OTP      Use KeyID to specify either the first or second 256-bit block of the OTP zone.
// #define GENDIG_ZONE_DATA     Use KeyID to specify a slot in the Data zone or a transport key in the hardware array.
// #define GENDIG_ZONE_SHARED_NONCE KeyID specifies the location of the input value in the message generation.
// #define GENDIG_ZONE_COUNTER      KeyID specifies the monotonic counter ID to be included in the message generation.
// #define GENDIG_ZONE_KEY_CONFIG   KeyID specifies the slot for which the configuration information is to be included in the message generation.
        atcab_gendig(GENDIG_ZONE_DATA, custom_param, NULL, 0);

    } else if (_select == 8) {
        // actual challenge
        uint8_t bad_key[32];
        memset(bad_key, 0, sizeof(bad_key));
        // only the first 16bits will be used
        for (int i=0; i<16; bad_key[i++] = 0x42);
        // load into tempkey
        atcab_nonce(bad_key);

        uint8_t plaintext[17] = "FLAG-JFSDKJFKDSA";
        uint8_t ciphertext[16];
        atcab_aes_encrypt(ATCA_TEMPKEY_KEYID, custom_param, plaintext, ciphertext);
        printf("plaintext:\n");
        print_16(plaintext);
        printf("ciphertext:\n");
        print_16(ciphertext);
    } else if (_select == 999) {
        printf("validating crypto chip...\n");
        bool is_locked = false;
        atcab_is_config_locked(&is_locked);
        if (!is_locked) {
            printf("FAIL: global config zone is not locked!\n");
            return 0;
        } else {
            printf("GOOD: global config zone is locked\n");
        }
        atcab_is_slot_locked(9, &is_locked);
        if (!is_locked) {
            printf("FAIL: slot 9 for HMAC is not locked!\n");
            return 0;
        } else {
            printf("GOOD: slot 9 for HMAC is locked\n");
        }
        atcab_is_slot_locked(5, &is_locked);
        if (!is_locked) {
            printf("FAIL: slot 5 for AESHMAC is not locked!\n");
            return 0;
        } else {
            printf("GOOD: slot 5 for AESHMAC is locked\n");
        }
        atcab_is_slot_locked(2, &is_locked);
        if (!is_locked) { // TODO do we want this one locked or not?
            printf("NEUTRAL: slot 2 for PrivWrite is not locked!\n");
        } else {
            printf("NEUTRAL: slot 2 for PrivWrite is locked\n");
            //return 0;
        }

        if (test_hmac_slot(9) == 0) {
            return 0;
        }
        if (test_hmac_slot(5) == 0) {
            return 0;
        }

    } else if (_select == 9) {
        bool is_locked = false;
        atcab_is_slot_locked(custom_param, &is_locked);
        printf("atcab_is_slot_locked: %s\n", is_locked ? "true" : "false");
        atcab_is_config_locked(&is_locked);
        printf("atcab_is_config_locked: %s\n", is_locked ? "true" : "false");
    } else if (_select == 10) {
        uint8_t public_key[ATCA_PUB_KEY_SIZE];
        if (custom_param == 0)
            atcab_genkey(2, public_key);
        else
            atcab_genkey(custom_param, public_key);
        print_public_key(public_key);
    } else if (_select == 11) {
        uint32_t counter_value;
        if (custom_param == 666) {
            atcab_write_config_counter(0, 0);
            atcab_write_config_counter(1, 0);
        }
        atcab_counter_read(custom_param, &counter_value);
        printf("atcab_counter_read %d: %lu\n", custom_param, counter_value);
        atcab_counter_increment(custom_param, &counter_value);
        printf("++atcab_counter_read %d: %lu\n", custom_param, counter_value);
    } else if (_select == 12) {
        // if (custom_param > 2 || custom_param < 0) {
        //     printf("err: param should be [1-3]\n");
        //     return 0;
        // }
        uint8_t private_key[ATCA_PRIV_KEY_SIZE + 4] = { 0x00, 0x00, 0x00, 0x00, 0x77, 0xd5, 0x96, 0xe7, 0xe8, 0xda, 0xf6, 0xbe, 0x19, 0xce, 0x30, 0x03, 0x78, 0x06, 0x9e, 0xd8, 0x9c, 0x5f, 0xdd, 0xc5, 0xfd, 0xdd, 0x5a, 0x9a, 0x5c, 0x06, 0x99, 0xa0, 0x64, 0x82, 0x69, 0xd6 };
        /*
        uint8_t host_nonce[NONCE_NUMIN_SIZE];
        memset(host_nonce, 0, sizeof(host_nonce));
        for(int i=0; i<NONCE_NUMIN_SIZE; host_nonce[i++] = 0xAB);
        */

        ret = atcab_priv_write(custom_param, private_key, 0, NULL, {0});
        if (ret != ATCA_SUCCESS) {
            printf("Failed to write private key with error: 0x%02X\n", ret);
        } else {
            printf("Private key written successfully.\n");
        }
    } else if (_select == 13) {
        bool is_locked = false;
        ret = atcab_is_data_locked(&is_locked);
        printf("atcab_is_data_locked: %s\n", is_locked ? "true" : "false");
        for(int i=0; i<16; i++) {
            ret = atcab_is_slot_locked(i, &is_locked);
            if (ret != ATCA_SUCCESS) printf("Failed atcab_is_slot_locked with error: 0x%02X\n", ret);
            printf("atcab_is_slot_locked(%d): %s\n", i, is_locked ? "true" : "false");
            ret = atcab_is_private(i, &is_locked);
            if (ret != ATCA_SUCCESS) printf("Failed atcab_is_private with error: 0x%02X\n", ret);
            printf("atcab_is_private(%d): %s\n", i, is_locked ? "true" : "false");
        }
    } else if (_select == 14) {
        uint8_t config_zone[ATCA_ECC_CONFIG_SIZE];
        if (atcab_read_config_zone(config_zone) == ATCA_SUCCESS) {
            // for (int i = 0; i < 16; i++) {
            //     for(int j = 0; j < 8; j++) {
            //         printf("%02x ", config_zone[i * 8 + j]);
            //     }
            //     printf("\n");
            //     }
        }
        atecc608_config_t * pConfig = (atecc608_config_t*)config_zone;
        //ESP_LOGI(TAG, "pConfig->i2c_addr: %02x", pConfig->I2C_Address);
        print_config(pConfig, custom_param);
    } else if (_select == 15) {
        bool is_zone_locked = false;
        if (ATCA_SUCCESS != (ret = atcab_is_locked(LOCK_ZONE_CONFIG, &is_zone_locked))) {
            ESP_LOGE(TAG, " failed\n  ! atcab_is_locked returned %02x", ret);
        }
        if (is_zone_locked) {
            ESP_LOGI(TAG, "Config zone is already locked");
        } else {
            ESP_LOGI(TAG, "Config zone is not locked");
        }
        if (custom_param != 666) {
            printf("this is sensible function, please use 666 as param\n");
            return 0;
        }
        ret = atcab_lock_config_zone();
        if (ret != ATCA_SUCCESS) {
            ESP_LOGE(TAG, "error in locking config zone, ret = %02x", ret);
        } else {
            ESP_LOGI(TAG, "Config zone is now locked.");
        }
    } else if (_select == 16) {
        // * \param[in]  mode      Selects which mode to be used for info command.
        //  * \param[in]  param2    Selects the particular fields for the mode.
        //  * \param[out] out_data  Response from info command (4 bytes). Can be set to
        //  *                       NULL if not required.
        uint8_t info[4];
        // Returns a value of one if an ECC private or public key stored in the key slot specified by param
        // is valid and zero if the key is not valid. For public keys in slots where PubInfo is zero, the
        // information returned by this command is not useful. This information is not meaningful for slots
        // in which KeyType does not indicate a supported ECC curve.
        atcab_info_base(0x01, custom_param, info);
        printf("atcab_info_base: %02x%02x%02x%02x\n", info[0], info[1], info[2], info[3]);
    } else if (_select == 17) {
        uint8_t private_key[ATCA_PRIV_KEY_SIZE] = { 0x77, 0xd5, 0x96, 0xe7, 0xe8, 0xda, 0xf6, 0xbe, 0x19, 0xce, 0x30, 0x03, 0x78, 0x06, 0x9e, 0xd8, 0x9c, 0x5f, 0xdd, 0xc5, 0xfd, 0xdd, 0x5a, 0x9a, 0x5c, 0x06, 0x99, 0xa0, 0x64, 0x82, 0x69, 0xd6 };
        ret = atcab_write_zone(ATCA_ZONE_DATA, custom_param, 0, 0, private_key, ATCA_BLOCK_SIZE);
        if (ret != ATCA_SUCCESS) {
            printf("Failed to write with error: 0x%02X\n", ret);
        } else {
            printf("Data written successfully on slot %d.\n", custom_param);
        }
    } else if (_select == 18) {
        uint8_t ones[ATCA_PRIV_KEY_SIZE] = {0xFF};
        memset(ones, 0xFF, sizeof(ones));
        ret = atcab_write_zone(ATCA_ZONE_DATA, custom_param, 0, 0, ones, ATCA_BLOCK_SIZE);
        if (ret != ATCA_SUCCESS) {
            printf("Failed to write 1s with error: 0x%02X\n", ret);
        } else {
            printf("1s written successfully on slot %d.\n", custom_param);
        }
    } else if (_select == 19) {
        uint8_t readslot[ATCA_BLOCK_SIZE] = {0};
        // If the data zone is unlocked, atcab_read_bytes_zone returns an error
        // so this will never works when atcab_is_slot_locked == FALSE
        ret = atcab_read_bytes_zone(ATCA_ZONE_DATA, custom_param, 0, readslot, sizeof(readslot));
        if (ret != ATCA_SUCCESS) {
            printf("Failed to read with error: 0x%02X\n", ret);
        } else {
            printf("Data read successfully from slot %d: ", custom_param);
            print_digest(readslot, ATCA_BLOCK_SIZE);
        }
    } else if (_select == 20) {
        if (custom_param2 != 999) {
            printf("this is sensible function, please use 999 as param 2\n");
            return 0;
        }
        ret = atcab_lock_data_slot(custom_param);
        if (ret != ATCA_SUCCESS) {
            printf("Failed to lock data slot %d with error: 0x%02X\n", custom_param, ret);
        } else {
            printf("Data slot %d locked successfully.\n", custom_param);
        }
    } else if (_select == 21) {
        // update config zone for slot id 4!
        const uint8_t SLOT_ID = 4;
        uint8_t config_zone[ATCA_ECC_CONFIG_SIZE];
        if (atcab_read_config_zone(config_zone) != ATCA_SUCCESS) {
            printf("Failed to read config zone\n");
            return 0;
        }
// uint32_t SN03;  uint32_t RevNum;  uint32_t SN47;  uint8_t  SN8;  uint8_t  AES_Enable;  uint8_t  I2C_Enable;  uint8_t  Reserved1;
// uint8_t  I2C_Address;  uint8_t  Reserved2;  uint8_t  CountMatch;  uint8_t  ChipMode;  uint16_t SlotConfig[16];  uint8_t  Counter0[8];
// uint8_t  Counter1[8];  uint8_t  UseLock;  uint8_t  VolatileKeyPermission;  uint16_t SecureBoot;  uint8_t  KdflvLoc;  uint16_t KdflvStr;
// uint8_t  Reserved3[9];  uint8_t  UserExtra;  uint8_t  UserExtraAdd;  uint8_t  LockValue;  uint8_t  LockConfig;  uint16_t SlotLocked;
// uint16_t ChipOptions;  uint32_t X509format;  uint16_t KeyConfig[16];
        atecc608_config_t * pConfig = (atecc608_config_t*)config_zone;

        pConfig->SlotConfig[SLOT_ID] &= ~ATCA_SLOT_CONFIG_WRITE_CONFIG_MASK;  // Clear the bits
        pConfig->SlotConfig[SLOT_ID] |= (0x0000 << ATCA_SLOT_CONFIG_WRITE_CONFIG_SHIFT) & ATCA_SLOT_CONFIG_WRITE_CONFIG_MASK;

        pConfig->SlotConfig[SLOT_ID] &= ~ATCA_SLOT_CONFIG_WRITE_KEY_MASK;  // Clear the bits
        pConfig->SlotConfig[SLOT_ID] |= (0x0000 << ATCA_SLOT_CONFIG_WRITE_KEY_SHIFT) & ATCA_SLOT_CONFIG_WRITE_KEY_MASK;

        pConfig->SlotConfig[SLOT_ID] &= ~ATCA_SLOT_CONFIG_IS_SECRET_MASK;  // Clear the bits
        pConfig->SlotConfig[SLOT_ID] |= (0x0000 << ATCA_SLOT_CONFIG_IS_SECRET_SHIFT) & ATCA_SLOT_CONFIG_IS_SECRET_MASK;

        pConfig->SlotConfig[SLOT_ID] &= ~ATCA_SLOT_CONFIG_ENCRYPTED_READ_MASK;  // Clear the bits
        pConfig->SlotConfig[SLOT_ID] |= (0x0000 << ATCA_SLOT_CONFIG_ENCRYPTED_READ_SHIFT) & ATCA_SLOT_CONFIG_ENCRYPTED_READ_MASK;

        pConfig->SlotConfig[SLOT_ID] &= ~ATCA_SLOT_CONFIG_NOMAC_MASK;  // Clear the bits
        pConfig->SlotConfig[SLOT_ID] |= (0x0000 << ATCA_SLOT_CONFIG_NOMAC_SHIFT) & ATCA_SLOT_CONFIG_NOMAC_MASK;

        pConfig->SlotConfig[SLOT_ID] &= ~ATCA_SLOT_CONFIG_READKEY_MASK;  // Clear the bits
        pConfig->SlotConfig[SLOT_ID] |= (0x0000 << ATCA_SLOT_CONFIG_READKEY_SHIFT) & ATCA_SLOT_CONFIG_READKEY_MASK;


        pConfig->KeyConfig[SLOT_ID] &= ~ATCA_KEY_CONFIG_PRIVATE_MASK;  // Clear the bits
        pConfig->KeyConfig[SLOT_ID] |= (0x0000 << ATCA_KEY_CONFIG_PRIVATE_SHIFT) & ATCA_KEY_CONFIG_PRIVATE_MASK;

        pConfig->KeyConfig[SLOT_ID] &= ~ATCA_KEY_CONFIG_LOCKABLE_MASK;  // Clear the bits
        pConfig->KeyConfig[SLOT_ID] |= (0x0001 << ATCA_KEY_CONFIG_LOCKABLE_SHIFT) & ATCA_KEY_CONFIG_LOCKABLE_MASK;

        pConfig->KeyConfig[SLOT_ID] &= ~ATCA_KEY_CONFIG_KEY_TYPE_MASK;  // Clear the bits
        pConfig->KeyConfig[SLOT_ID] |= (0b0111 << ATCA_KEY_CONFIG_KEY_TYPE_SHIFT) & ATCA_KEY_CONFIG_KEY_TYPE_MASK; // 7 == 0111

        print_config(pConfig, SLOT_ID);

        if (atcab_write_config_zone(config_zone) != ATCA_SUCCESS) {
            printf("Failed to write config zone\n");
            return 0;
        } else {
            printf("Config zone written successfully.\n");
        }

    } else if (_select == 30) {
        printf("encrypt flag\n");
        uint8_t plaintext[17] = "FLAG-JFSDKJFKDAB";
        uint8_t encrypted_flag[16] = {0x00};
        encrypt_flag(plaintext, encrypted_flag);
        print_16(encrypted_flag);

        printf("decrypt flag\n");
        uint8_t decrypted_flag[16] = {0x00};
        decrypt_flag(encrypted_flag, decrypted_flag);
        print_16(decrypted_flag);
        printf("decypted flag: %.16s\n", decrypted_flag);

        if (strncmp((char*)plaintext, (char*)decrypted_flag, 16) == 0) {
            printf("SUCCESS: flag is decrypted correctly\n");
        } else {
            printf("ERROR: flag is not decrypted correctly\n");
        }
    }
    return 0;
}

#endif

static void print_32(uint8_t* data) {
        for (int i = 0; i < 4; i++) {
            for(int j = 0; j < 8; j++) {
                printf("%02x ", data[i * 8 + j]);
            }
            printf("\n");
        }
}

static void print_64(uint8_t* data) {
        for (int i = 0; i < 4; i++) {
            for(int j = 0; j < 16; j++) {
                printf("%02x ", data[i * 16 + j]);
            }
            printf("\n");
        }
}

static void print_digest(uint8_t* digest, size_t size) {
    for(int i = 0; i < size; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");
}

int print_slotnkeyconfig(int argc, char **argv) {
    uint16_t slot_id = 0;
    if (argc >= 2) {
        slot_id = atoi(argv[1]);
    }
    if (slot_id > 9) {
        printf("Error! slot id should be [1-9]\n");
        return 0;
    }
    uint8_t config_zone[ATCA_ECC_CONFIG_SIZE];
    if (atcab_read_config_zone(config_zone) == ATCA_SUCCESS) {
        atecc608_config_t * pConfig = (atecc608_config_t*)config_zone;
        print_config(pConfig, slot_id);
    }
    return 0;
}

#if CTF_ADDON_ADMIN_MODE
int symmetric_decrypt(int argc, char **argv) {
    if (argc >= 2) {
        uint8_t cipher[16];
        char* hex_str = argv[1];
        size_t hex_size = strlen(hex_str);
        if (hex_size != 32) {
            printf("Error! hex string should be 32 characters\n");
            return 0;
        }
        for (int i = 0; i < 16; i++) {
            sscanf(hex_str + 2*i, "%2hhx", &cipher[i]);
        }
        uint8_t plaintext[16];
        decrypt_flag(cipher, plaintext);
        print_bin2hex(plaintext, 16);
    } else {
        printf("Error! hex string is required as first parameter\n");
    }
    return 0;
}
#endif

void register_crypto_atecc(void) {
    if (Save::save_data.raw_spi_mode) {
        //ESP_LOGI(TAG, "crypto_atecc is disabled in raw spi mode");
        return;
    }
    #if CTF_ADDON_ADMIN_MODE
    const esp_console_cmd_t cmd = {
        .command = "crypto",
        .help = "Run the crypto test\n",
        .hint = "[1-30]",
        .func = &crypto_atecc,
        .argtable = NULL,        
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );

    const esp_console_cmd_t cmd3 = {
        .command = "admin_symmetric_decrypt",
        .help = "[ATECC608B] read hex and decrypt symmetrically 16 bytes\n",
        .hint = "[hex chars]",
        .func = &symmetric_decrypt,
        .argtable = NULL,        
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd3) );
    #endif

    const esp_console_cmd_t cmd2 = {
        .command = "crypto_print_config",
        .help = "[ATECC608B] Print the slot and key config for a given slot\n",
        .hint = "[0-9]",
        .func = &print_slotnkeyconfig,
        .argtable = NULL,        
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd2) );



}

static const uint8_t public_key_x509_header[] = {
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A,
    0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04
};

static void print_public_key(uint8_t *pubkey) {
    // this is crap!
    uint8_t buf[128];
    uint8_t * tmp;
    size_t buf_len = sizeof(buf);
    memset(buf, 0, sizeof(buf));

    /* Calculate where the raw data will fit into the buffer */
    tmp = buf + sizeof(buf) - ATCA_PUB_KEY_SIZE - sizeof(public_key_x509_header);

    /* Copy the header */
    memcpy(tmp, public_key_x509_header, sizeof(public_key_x509_header));

    /* Copy the key bytes */
    memcpy(tmp + sizeof(public_key_x509_header), pubkey, ATCA_PUB_KEY_SIZE);

    /* Convert to base 64 */
    (void)atcab_base64encode(tmp, ATCA_PUB_KEY_SIZE + sizeof(public_key_x509_header), (char*)buf, &buf_len);

    /* Add a null terminator */
    buf[buf_len] = '\0';

    /* Print out the key */
    ESP_LOGI(TAG, "\n-----BEGIN PUBLIC KEY-----\n%s\n-----END PUBLIC KEY-----", buf);
}

