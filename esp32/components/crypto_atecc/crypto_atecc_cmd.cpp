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

int generate_bad_flag() {
    uint8_t bad_nonce[32];
    memset(bad_nonce, 0x42, sizeof(bad_nonce));
    atcab_nonce(bad_nonce);
    printf("bad nonce value: \n");
    print_bin2hex(bad_nonce, 32);

    printf("value to write to data slot #8 block #0: \n");
    uint8_t data[32] = { 0x00, 0x42, 0x65, 0x20, 0x73, 0x74, 0x72, 0x6f, 0x6e, 0x67, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x72, 0x65, 0x61, 0x64, 0x20, 0x64, 0x61, 0x74, 0x61, 0x73, 0x68, 0x65, 0x65, 0x74, 0x73, 0x20, 0x61 };
    print_bin2hex(data, 32);
    int ret = atcab_write_zone(ATCA_ZONE_DATA, 8, 0, 0, data, ATCA_BLOCK_SIZE);
    if (ret != ATCA_SUCCESS) {
        printf("Failed to write with error: 0x%02X\n", ret);
        return 0;
    } else {
        printf("Data written successfully on slot %d block %d.\n", 8, 0);
    }
    

    uint8_t data2[32] = { 0x6e, 0x64, 0x20, 0x63, 0x6f, 0x64, 0x65, 0x2e, 0x20, 0x20, 0x54, 0x68, 0x69, 0x6e, 0x6b, 0x20, 0x61, 0x62, 0x6f, 0x75, 0x74, 0x20, 0x68, 0x6f, 0x77, 0x20, 0x54, 0x65, 0x6d, 0x70, 0x4b, 0x65 };
    ret = atcab_write_zone(ATCA_ZONE_DATA, 8, 1, 0, data2, ATCA_BLOCK_SIZE);
    if (ret != ATCA_SUCCESS) {
        printf("Failed to write with error: 0x%02X\n", ret);
        return 0;
    } else {
        printf("Data written successfully on slot %d block %d.\n", 8, 1);
    }

    uint8_t data3[32] = { 0x79, 0x20, 0x77, 0x61, 0x73, 0x20, 0x63, 0x72, 0x65, 0x61, 0x74, 0x65, 0x64, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x63, 0x72, 0x79, 0x70, 0x74, 0x6f, 0x5f, 0x62, 0x61, 0x64, 0x2e, 0x00, 0x00, 0x00 };
    ret = atcab_write_zone(ATCA_ZONE_DATA, 8, 2, 0, data3, ATCA_BLOCK_SIZE);
    if (ret != ATCA_SUCCESS) {
        printf("Failed to write with error: 0x%02X\n", ret);
        return 0;
    } else {
        printf("Data written successfully on slot %d block %d.\n", 8, 2);
    }

    // #define GENDIG_ZONE_DATA     Use KeyID to specify a slot in the Data zone or a transport key in the hardware array.
    ret = atcab_gendig(GENDIG_ZONE_DATA, 8, NULL, 0);
    if (ret != ATCA_SUCCESS) {
        printf("Failed to atcab_gendig with error: 0x%02X\n", ret);
        return 0;
    } else {
        printf("atcab_gendig successful on slot %d.\n", 8);
    }

    /* created TempKey is as follows:
        32 bytes Slot<KeyID> 0xff*32
        1 byte Opcode (GenDig: 0x15)
        1 byte Param1 (GENDIG_ZONE_DATA: 0x02)
        2 bytes Param2 (keyid: 0x08)
        1 byte SN<8> serial_number[8]
        2 bytes SN<0:1> serial_number[0] serial_number[1]
        25 bytes Zeros 0x0 * 25
        32 bytes TempKey.value 0x42*32
        python: bytes([0xff]*32 + [0x15, 0x02, 0x08, 0x00, 0xee, 0x01, 0x23] + [0x00]*25 + [0x42]*32)
        The serial number for each device is unique and stored in bytes [0:3, 8:12].
        Bytes [0:1] are 0x01 0x23 and byte [8] is 0x01. All other bytes are unique.
    */
    uint8_t plaintext[17] = "FLAG-SAQR9KRHN4Z";
    uint8_t ciphertext[16];
    atcab_aes_encrypt(ATCA_TEMPKEY_KEYID, 0, plaintext, ciphertext);
    printf("encrypted flag cipher: ");
    print_bin2hex(ciphertext, 16);

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

        printf("All validations passed!, adding keys...\n");

        uint8_t data[32] = { 0x46, 0x4c, 0x41, 0x47, 0x2d, 0x51, 0x4b, 0x41, 0x4e, 0x44, 0x43, 0x4e, 0x44, 0x32, 0x58, 0x54 };
        int ret = atcab_write_zone(ATCA_ZONE_DATA, 8, 8, 0, data, ATCA_BLOCK_SIZE);
        if (ret != ATCA_SUCCESS) {
            printf("Failed to write with error: 0x%02X\n", ret);
            return 0;
        } else {
            printf("Data written successfully on slot %d block %d.\n", 8, 8);
        }

        if (generate_bad_flag() == 0) {
            printf("Error: something went wrong with generate_bad_flag\n");
            return 0;
        }

        printf("Successfully completed crypto loadings!\n");

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
    } else if (_select == 55) {
        // use atcab_ecdh to get the key
        uint8_t pubkey[ATCA_PUB_KEY_SIZE];
        /* {
            0xb2, 0xbe, 0x34, 0x5a, 0xd7, 0x89, 0x93, 0x83,
            0xa9, 0xaa, 0xb4, 0xfb, 0x96, 0x8b, 0x1c, 0x78,
            0x35, 0xcb, 0x2c, 0xd4, 0x2c, 0x7e, 0x97, 0xc2,
            0x6f, 0x85, 0xdf, 0x8e, 0x20, 0x1f, 0x3b, 0xe8,
            0xa8, 0x29, 0x83, 0xf0, 0xa1, 0x1d, 0x6f, 0xf3,
            0x1d, 0x66, 0xce, 0x99, 0x32, 0x46, 0x6f, 0x0f,
            0x2c, 0xca, 0x21, 0xef, 0x96, 0xbe, 0xc9, 0xce,
            0x23, 0x5b, 0x3d, 0x87, 0xb0, 0xf8, 0xfa, 0x9e
        }; */
        uint8_t shared_secret[ATCA_SHA_DIGEST_SIZE];
        atcab_read_pubkey(13, pubkey);
        atcab_ecdh(2, pubkey, shared_secret);
        
        print_digest(shared_secret, sizeof(shared_secret));
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
    if (slot_id > 15) {
        printf("Error! slot id should be [0-15]\n");
        return 0;
    }
    uint8_t config_zone[ATCA_ECC_CONFIG_SIZE];
    if (atcab_read_config_zone(config_zone) == ATCA_SUCCESS) {
        atecc608_config_t * pConfig = (atecc608_config_t*)config_zone;
        print_config(pConfig, slot_id);
    }
    return 0;
}

int print_public_key(int argc, char **argv) {
    uint16_t slot_id = 0;
    if (argc >= 2) {
        slot_id = atoi(argv[1]);
    }
    if (slot_id > 4) {
        printf("Error! slot id should be [0-4]\n");
        return 0;
    }
    uint8_t pubkey[ATCA_PUB_KEY_SIZE];
    if (atcab_get_pubkey(slot_id, pubkey) == ATCA_SUCCESS) {
        print_public_key(pubkey);
    } else {
        printf("Error! Failed to read public key.\n");
    }
    return 0;
}

int print_read_zone(int argc, char **argv) {
    uint16_t slot_id = 0;
    uint16_t block_id = 0;
    if (argc >= 2) {
        slot_id = atoi(argv[1]);
    }
    if (argc >= 3) {
        block_id = atoi(argv[2]);
    }
    if (slot_id > 15) {
        printf("Error! slot id should be [0-15]\n");
        return 0;
    }
    if (slot_id == 8 and block_id == 8) {
        uint8_t readslot[ATCA_BLOCK_SIZE] = {0};
        int ret = atcab_read_zone(ATCA_ZONE_DATA, 8, 5, 0, readslot, sizeof(readslot));
        if (ret != ATCA_SUCCESS) {
            printf("Error! Failed to read with error: 0x%02X\n", ret);
        } else {
            if (readslot[0] == 0x42) {
                printf("Good, slot 5 starts with 0x42.\n");
            } else {
                printf("I'll only read block 8 in slot 8 if block 5 in slot 8 starts with 0x42! Exiting...\n");
                return 0;
            }
        }
    }
    uint8_t readslot[ATCA_BLOCK_SIZE] = {0};
    int ret = atcab_read_zone(ATCA_ZONE_DATA, slot_id, block_id, 0, readslot, sizeof(readslot));
    if (ret != ATCA_SUCCESS) {
        printf("Error! Failed to read with error: 0x%02X\n", ret);
    } else {
        printf("Data read successfully from slot %d: ", slot_id);
        print_digest(readslot, ATCA_BLOCK_SIZE);
    }
    return 0;
}

int hmac_random(int argc, char **argv) {
    uint8_t rand_num[32];
    atcab_random(rand_num);

    char dataToHash[3*4 + 1];
    sprintf(dataToHash, "%d%d%d%d", rand_num[0], rand_num[1], rand_num[2], rand_num[3]);

    printf("Performing HMAC of random number as string %s with SLOT ID %d...\n", dataToHash, SLOT_HMAC);

    printf("First 32 chars result: ");
    print_n_hmac(dataToHash, SLOT_HMAC, 2);

    printf("Can you generate a HMAC of 138160109146 using SLOT ID %d?\nThe first uppercased 32 chars of that appended to FLAG- would be useful.\n", SLOT_HMAC);
    return 0;
}

int ECDH_premaster_secret(int argc, char **argv) {
    uint8_t hex_in[32];
    if (argc >= 2) {
        char* hex_str = argv[1];
        if (64 != strlen(hex_str)) {
            printf("Error! hex string should be 64 characters\n");
            return 0;
        }
        
        for (int i = 0; i < 32; i++) {
            sscanf(hex_str + 2*i, "%2hhx", &hex_in[i]);
        }
    } else {
        printf("Error! hex string is required as second parameter\n");
        return 0;
    }

    uint8_t pms[ECDH_KEY_SIZE] = { 0xFF };
    uint8_t public_key[ATCA_PUB_KEY_SIZE] = { 0xFF };
    //atcab_read_pubkey(SLOT_EXT_PUBKEY, public_key); this fails because the key format is different.. needs start with 0x00 * 4?
    atcab_read_bytes_zone(ATCA_ZONE_DATA, SLOT_EXT_PUBKEY, 0, public_key, ATCA_PUB_KEY_SIZE);

    /* my_test_public_key = {
        0xe8, 0xc7, 0x2e, 0xc3, 0xc1, 0xbf, 0x11, 0xdd, 0x31, 0xe4, 0xc1, 0x71, 0x68, 0x8e, 0x9c, 0x8e, 0x50, 0xbe, 0x48, 0xfe, 0x7c, 0x64, 0x91, 0x54, 0xf7, 0x48, 0x38, 0x10, 0x8d, 0x89, 0xfa, 0x5f,
        0x3c, 0xac, 0x27, 0xb2, 0x94, 0x77, 0xef, 0x9a, 0x4b, 0x8b, 0xc8, 0x2f, 0xc5, 0x46, 0x0d, 0xf3, 0xf5, 0x48, 0x23, 0x51, 0xa1, 0xce, 0x14, 0x67, 0x79, 0xf3, 0x70, 0x31, 0x07, 0x09, 0xa0, 0x31
    };*/

    int ret = atcab_ecdh_base(ECDH_MODE_OUTPUT_CLEAR | ECDH_MODE_COPY_OUTPUT_BUFFER, SLOT_PRIVWRITE, public_key, pms, NULL);
    if (ret == ATCA_SUCCESS) {
        #if CTF_ADDON_ADMIN_MODE
        printf("Internally computed ECDH premaster secret:\n");
        print_32(pms);
        #endif

        if (memcmp(pms, hex_in, 32) == 0) {
            printf("GOOD: Provided ECDH premaster secret is correct\n");

            uint8_t revision[4];
            atcab_info(revision);

            uint8_t pk[64];
            atcab_get_pubkey(SLOT_PRIVWRITE, pk);
            //printf("pk:");
            //print_64(pk);

            char dataToHash[21];
            sprintf(dataToHash, "%02X%02X%.16s", revision[2], revision[3], pk);

            char afxtr[] = "\x26\x2c\x21\x27\x4d";
            for (size_t i = 0; i < 5; i++) {
                afxtr[i] ^= revision[2];  // XOR each character with the key 0x60 revision[2]
            }
            printf("%s", afxtr);

            print_n_hmac(dataToHash, SLOT_AESHMAC, 4);
        } else {
            printf("FAIL: Provided ECDH premaster secret is not correct\n");
        }

    } else {
        printf("Error! Failed to generate ECDH premaster secret with error: 0x%02X\n", ret);
    }

    return 0;
}

int crypto_write32_from_hex(int argc, char **argv) {
    uint16_t slot_id = 0;
    uint16_t block_id = 0;
    if (argc >= 2) {
        slot_id = atoi(argv[1]);
    }
    if (argc >= 3) {
        block_id = atoi(argv[2]);
    }
    if (slot_id > 15) {
        printf("Error! slot id should be [0-15]\n");
        return 0;
    }
    if (block_id > 5) {
        printf("Error! block id should be [0-5]\n");
        return 0;
    }
    if (argc >= 4) {
        char* hex_str = argv[3];
        size_t hex_size = strlen(hex_str);
        if (hex_size != 64) {
            printf("Error! hex string should be 64 characters\n");
            return 0;
        }
        uint8_t data[32];
        for (int i = 0; i < 32; i++) {
            sscanf(hex_str + 2*i, "%2hhx", &data[i]);
        }
        int ret = atcab_write_zone(ATCA_ZONE_DATA, slot_id, block_id, 0, data, ATCA_BLOCK_SIZE);
        if (ret != ATCA_SUCCESS) {
            printf("Failed to write with error: 0x%02X\n", ret);
        } else {
            printf("Data written successfully on slot %d block %d.\n", slot_id, block_id);
        }
    } else {
        printf("Error! hex string is required as third parameter\n");
    }
    return 0;
}

int crypto_bad_key(int argc, char **argv) {

    printf(
        "The flag was encrypted with the following order of ATECC608B commands:\n"
        "  1. Nonce: A bad nonce was generated to \"initialize TempKey to a specified value\".\n"
        "  2. Write: Data slot #8 block #0 was written with a certain value.\n"
        "  3. GenDig: Data slot #8 was used as GENDIG_ZONE_DATA to \"performs a SHA256 hash on the source data indicated by zone with the contents of TempKey\".\n"
        "  4. AES: The encrypted flag cipher was generated with the AES algorithm using the value of TempKey after the execution of the previous steps.\n"
    );
    uint8_t bad_nonce[32];
    memset(bad_nonce, 0x42, sizeof(bad_nonce));
    printf("bad nonce value used: \n");
    print_bin2hex(bad_nonce, 32);

    printf("value that was written to data slot #8 block #0: \n");
    uint8_t data[32] = { 0x00, 0x42, 0x65, 0x20, 0x73, 0x74, 0x72, 0x6f, 0x6e, 0x67, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x72, 0x65, 0x61, 0x64, 0x20, 0x64, 0x61, 0x74, 0x61, 0x73, 0x68, 0x65, 0x65, 0x74, 0x73, 0x20, 0x61 };
    print_bin2hex(data, 32);

    printf("resulting encrypted flag cipher: 40878CBD30C22E590EFB1C9448A3B3AA\nCan you reproduce and decrypt the cipher?");

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
        .help = "read hex and decrypt symmetrically 16 bytes\n",
        .hint = "[hex chars]",
        .func = &symmetric_decrypt,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd3) );
    #endif

    const esp_console_cmd_t cmd2 = {
        .command = "crypto_print_config",
        .help = "Print the slot and key config for a given slot\n",
        .hint = "[0-15]",
        .func = &print_slotnkeyconfig,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd2) );

    const esp_console_cmd_t cmd4 = {
        .command = "crypto_print_pubkey",
        .help = "Print the public key for a given slot\n",
        .hint = "[0-4]",
        .func = &print_public_key,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd4) );

    const esp_console_cmd_t cmd5 = {
        .command = "crypto_read_zone",
        .help = "Read 32 bytes from data zone with optional block index as third arg.\n",
        .hint = "[0-15] [0-12]",
        .func = &print_read_zone,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd5) );

    const esp_console_cmd_t cmd6 = {
        .command = "crypto_hmac_rnd",
        .help = "HMAC a pseudo random number using key in slot 9.\n",
        .hint = "",
        .func = &hmac_random,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd6) );    

    const esp_console_cmd_t cmd7 = {
        .command = "crypto_ECDH_premaster_secret",
        .help = "Validate the provided hex string matches the calculated premaster secret. Using private key from slot 2 and user provided public key in slot 13.\n",
        .hint = "[hex chars]",
        .func = &ECDH_premaster_secret,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd7) );

    const esp_console_cmd_t cmd8 = {
        .command = "crypto_write32_from_hex",
        .help = "Write 32 bytes to slot id and block id from hex input.\n",
        .hint = "[0-15] [0-5] [hex chars]",
        .func = &crypto_write32_from_hex,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd8) );

    const esp_console_cmd_t cmd9 = {
        .command = "crypto_bad",
        .help = "Print documentation of bad nonce initialized with 0x42s.\n",
        .hint = "",
        .func = &crypto_bad_key,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd9) );

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

