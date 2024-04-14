#include "cryptoauthlib.h"

// Slot to configure for SHA-HMAC access and Write key to
#define SLOT_HMAC 9

// Slot to store PrivWrite key
#define SLOT_PRIVWRITE 2

void print_16(uint8_t* data) {
    for (int i = 0; i < 2; i++) {
        for(int j = 0; j < 8; j++) {
            printf("%02x ", data[i * 8 + j]);
        }
        printf("\n");
    }
}

void encrypt_flag(uint8_t flag[16], uint8_t (&encrypted_flag)[16]) {
    ATCA_STATUS ret = atcab_aes_encrypt(SLOT_HMAC, 0, flag, encrypted_flag);
    if (ret != ATCA_SUCCESS) {
        printf("Error encrypting flag: %02x\n", ret);
        return;
    }
    char hex_str[sizeof(encrypted_flag)*3+1];
    size_t hex_size = sizeof(hex_str);
    ret = atcab_bin2hex(encrypted_flag, 16, hex_str, &hex_size);
    if (ret != ATCA_SUCCESS) {
        printf("Error converting binary data to hex: %02x\n", ret);
    } else {
        printf("Encryptdflag: %s\n", hex_str);
    }
}

void decrypt_flag(uint8_t encrypted_flag[16], uint8_t (&flag)[16])  {
    ATCA_STATUS ret = atcab_aes_decrypt(SLOT_HMAC, 0, encrypted_flag, flag);
    if (ret != ATCA_SUCCESS) {
        printf("Error decrypting flag: %02x\n", ret);
    }
    char hex_str[sizeof(flag)*3+1];
    size_t hex_size = sizeof(hex_str);
    ret = atcab_bin2hex(flag, 16, hex_str, &hex_size);
    if (ret != ATCA_SUCCESS) {
        printf("Error converting binary data to hex: %02x\n", ret);
    } else {
        printf("Decryptdflag: %s\n", hex_str);
    }
}

void print_config(atecc608_config_t * pConfig, uint16_t custom_param) {

// pConfig
// uint32_t SN03;  uint32_t RevNum;  uint32_t SN47;  uint8_t  SN8;  uint8_t  AES_Enable;  uint8_t  I2C_Enable;  uint8_t  Reserved1;
// uint8_t  I2C_Address;  uint8_t  Reserved2;  uint8_t  CountMatch;  uint8_t  ChipMode;  uint16_t SlotConfig[16];  uint8_t  Counter0[8];
// uint8_t  Counter1[8];  uint8_t  UseLock;  uint8_t  VolatileKeyPermission;  uint16_t SecureBoot;  uint8_t  KdflvLoc;  uint16_t KdflvStr;
// uint8_t  Reserved3[9];  uint8_t  UserExtra;  uint8_t  UserExtraAdd;  uint8_t  LockValue;  uint8_t  LockConfig;  uint16_t SlotLocked;
// uint16_t ChipOptions;  uint32_t X509format;  uint16_t KeyConfig[16];

// print the value of AES_Enable
    printf("AES_Enable: 0x%02x\n", pConfig->AES_Enable);

/* General Purpose Slot Config (Not ECC Private Keys) */
// #define ATCA_SLOT_CONFIG_READKEY_SHIFT          (0)
// #define ATCA_SLOT_CONFIG_READKEY_MASK           (0x0Fu << ATCA_SLOT_CONFIG_READKEY_SHIFT)
// #define ATCA_SLOT_CONFIG_READKEY(v)             (ATCA_SLOT_CONFIG_READKEY_MASK & (v << ATCA_SLOT_CONFIG_READKEY_SHIFT))
// #define ATCA_SLOT_CONFIG_NOMAC_SHIFT            (4)
// #define ATCA_SLOT_CONFIG_NOMAC_MASK             (0x01u << ATCA_SLOT_CONFIG_NOMAC_SHIFT)
// #define ATCA_SLOT_CONFIG_LIMITED_USE_SHIFT      (5)
// #define ATCA_SLOT_CONFIG_LIMITED_USE_MASK       (0x01u << ATCA_SLOT_CONFIG_LIMITED_USE_SHIFT)
// #define ATCA_SLOT_CONFIG_ENCRYPTED_READ_SHIFT   (6)
// #define ATCA_SLOT_CONFIG_ENCRYPTED_READ_MASK    (0x01u << ATCA_SLOT_CONFIG_ENCRYPTED_READ_SHIFT)
// #define ATCA_SLOT_CONFIG_IS_SECRET_SHIFT        (7)
// #define ATCA_SLOT_CONFIG_IS_SECRET_MASK         (0x01u << ATCA_SLOT_CONFIG_IS_SECRET_SHIFT)
// #define ATCA_SLOT_CONFIG_WRITE_KEY_SHIFT        (8)
// #define ATCA_SLOT_CONFIG_WRITE_KEY_MASK         (0x0Fu << ATCA_SLOT_CONFIG_WRITE_KEY_SHIFT)
// #define ATCA_SLOT_CONFIG_WRITE_KEY(v)           (ATCA_SLOT_CONFIG_WRITE_KEY_MASK & (v << ATCA_SLOT_CONFIG_WRITE_KEY_SHIFT))
// #define ATCA_SLOT_CONFIG_WRITE_CONFIG_SHIFT     (12)
// #define ATCA_SLOT_CONFIG_WRITE_CONFIG_MASK      (0x0Fu << ATCA_SLOT_CONFIG_WRITE_CONFIG_SHIFT)
// #define ATCA_SLOT_CONFIG_WRITE_CONFIG(v)        (ATCA_SLOT_CONFIG_WRITE_CONFIG_MASK & (v << ATCA_SLOT_CONFIG_WRITE_CONFIG_SHIFT))
    printf("SlotConfig[%d]: 0x%04x\n", custom_param, pConfig->SlotConfig[custom_param]);

#if CTF_ADDON_ADMIN_MODE
    uint16_t write_config = pConfig->SlotConfig[custom_param];
    write_config &= ATCA_SLOT_CONFIG_WRITE_CONFIG_MASK;
    write_config >>= ATCA_SLOT_CONFIG_WRITE_CONFIG_SHIFT;
    printf("\tWriteConfig: %d\n", write_config);

    uint16_t write_key_id = pConfig->SlotConfig[custom_param];
    write_key_id &= ATCA_SLOT_CONFIG_WRITE_KEY_MASK;
    write_key_id >>= ATCA_SLOT_CONFIG_WRITE_KEY_SHIFT;
    printf("\tWriteKey: %d\n", write_key_id);

    uint16_t is_secret = pConfig->SlotConfig[custom_param];
    is_secret &= ATCA_SLOT_CONFIG_IS_SECRET_MASK;
    is_secret >>= ATCA_SLOT_CONFIG_IS_SECRET_SHIFT;
    printf("\tIsSecret: %d\n", is_secret);

    uint16_t encrypt_read = pConfig->SlotConfig[custom_param];
    encrypt_read &= ATCA_SLOT_CONFIG_ENCRYPTED_READ_MASK;
    encrypt_read >>= ATCA_SLOT_CONFIG_ENCRYPTED_READ_SHIFT;
    printf("\tencrypt_read: %d\n", encrypt_read);

    uint16_t no_mac = pConfig->SlotConfig[custom_param];
    no_mac &= ATCA_SLOT_CONFIG_NOMAC_MASK;
    no_mac >>= ATCA_SLOT_CONFIG_NOMAC_SHIFT;
    printf("\tno_mac: %d\n", no_mac);

    uint16_t readkey = pConfig->SlotConfig[custom_param];
    readkey &= ATCA_SLOT_CONFIG_READKEY_MASK;
    readkey >>= ATCA_SLOT_CONFIG_READKEY_SHIFT;
    printf("\treadkey: %d\n", readkey);
#endif

/* Key Config */
// #define ATCA_KEY_CONFIG_OFFSET(x)               (96UL + (x) * 2)
// #define ATCA_KEY_CONFIG_PRIVATE_SHIFT           (0)
// #define ATCA_KEY_CONFIG_PRIVATE_MASK            (0x01u << ATCA_KEY_CONFIG_PRIVATE_SHIFT)
// #define ATCA_KEY_CONFIG_PUB_INFO_SHIFT          (1)
// #define ATCA_KEY_CONFIG_PUB_INFO_MASK           (0x01u << ATCA_KEY_CONFIG_PUB_INFO_SHIFT)
// #define ATCA_KEY_CONFIG_KEY_TYPE_SHIFT          (2)
// #define ATCA_KEY_CONFIG_KEY_TYPE_MASK           (0x07u << ATCA_KEY_CONFIG_KEY_TYPE_SHIFT)
// #define ATCA_KEY_CONFIG_KEY_TYPE(v)             (ATCA_KEY_CONFIG_KEY_TYPE_MASK & (v << ATCA_KEY_CONFIG_KEY_TYPE_SHIFT))
// #define ATCA_KEY_CONFIG_LOCKABLE_SHIFT          (5)
// #define ATCA_KEY_CONFIG_LOCKABLE_MASK           (0x01u << ATCA_KEY_CONFIG_LOCKABLE_SHIFT)
// #define ATCA_KEY_CONFIG_REQ_RANDOM_SHIFT        (6)
// #define ATCA_KEY_CONFIG_REQ_RANDOM_MASK         (0x01u << ATCA_KEY_CONFIG_REQ_RANDOM_SHIFT)
// #define ATCA_KEY_CONFIG_REQ_AUTH_SHIFT          (7)
// #define ATCA_KEY_CONFIG_REQ_AUTH_MASK           (0x01u << ATCA_KEY_CONFIG_REQ_AUTH_SHIFT)
// #define ATCA_KEY_CONFIG_AUTH_KEY_SHIFT          (8)
// #define ATCA_KEY_CONFIG_AUTH_KEY_MASK           (0x0Fu << ATCA_KEY_CONFIG_AUTH_KEY_SHIFT)
// #define ATCA_KEY_CONFIG_AUTH_KEY(v)             (ATCA_KEY_CONFIG_AUTH_KEY_MASK & (v << ATCA_KEY_CONFIG_AUTH_KEY_SHIFT))
// #define ATCA_KEY_CONFIG_PERSIST_DISABLE_SHIFT   (12)
// #define ATCA_KEY_CONFIG_PERSIST_DISABLE_MASK    (0x01u << ATCA_KEY_CONFIG_PERSIST_DISABLE_SHIFT)
// #define ATCA_KEY_CONFIG_RFU_SHIFT               (13)
// #define ATCA_KEY_CONFIG_RFU_MASK                (0x01u << ATCA_KEY_CONFIG_RFU_SHIFT)
// #define ATCA_KEY_CONFIG_X509_ID_SHIFT           (14)
// #define ATCA_KEY_CONFIG_X509_ID_MASK            (0x03u << ATCA_KEY_CONFIG_X509_ID_SHIFT)
// #define ATCA_KEY_CONFIG_X509_ID(v)              (ATCA_KEY_CONFIG_X509_ID_MASK & (v << ATCA_KEY_CONFIG_X509_ID_SHIFT))
    printf("KeyConfig[%d]: 0x%04x\n", custom_param, pConfig->KeyConfig[custom_param]);

#if CTF_ADDON_ADMIN_MODE
    uint16_t cfgprivate = pConfig->KeyConfig[custom_param];
    cfgprivate &= ATCA_KEY_CONFIG_PRIVATE_MASK;
    cfgprivate >>= ATCA_KEY_CONFIG_PRIVATE_SHIFT;
    printf("\tprivate: %d\n", cfgprivate);

    uint16_t pub_info = pConfig->KeyConfig[custom_param];
    pub_info &= ATCA_KEY_CONFIG_PUB_INFO_MASK;
    pub_info >>= ATCA_KEY_CONFIG_PUB_INFO_SHIFT;
    printf("\tpub_info: %d\n", pub_info);

    uint16_t key_type = pConfig->KeyConfig[custom_param];
    key_type &= ATCA_KEY_CONFIG_KEY_TYPE_MASK;
    key_type >>= ATCA_KEY_CONFIG_KEY_TYPE_SHIFT;
    printf("\tkey_type: %d\n", key_type);

    uint16_t req_auth = pConfig->KeyConfig[custom_param];
    req_auth &= ATCA_KEY_CONFIG_REQ_AUTH_MASK;
    req_auth >>= ATCA_KEY_CONFIG_REQ_AUTH_SHIFT;
    printf("\treq_auth: %d\n", req_auth);

    uint16_t req_random = pConfig->KeyConfig[custom_param];
    req_random &= ATCA_KEY_CONFIG_REQ_RANDOM_MASK;
    req_random >>= ATCA_KEY_CONFIG_REQ_RANDOM_SHIFT;
    printf("\treq_random: %d\n", req_random);

    uint16_t lockable = pConfig->KeyConfig[custom_param];
    lockable &= ATCA_KEY_CONFIG_LOCKABLE_MASK;
    lockable >>= ATCA_KEY_CONFIG_LOCKABLE_SHIFT;
    printf("\tlockable: %d\n", lockable);

    bool is_locked = false;
    atcab_is_slot_locked(custom_param, &is_locked);
    printf("SlotLocked(%d): %s\n", custom_param, is_locked ? "true" : "false");

#endif

}
