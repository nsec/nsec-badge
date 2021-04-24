#include <stdio.h>

static const int ARRAY_LEN = 32;

static int verify(const char *user_code)
{
    int code[ARRAY_LEN];

    // expected user input: 11a922186c46496eb1317c128b361720
    code[21] = 99;
    code[16] = 98;
    code[10] = 52;
    code[18] = 51;
    code[24] = 56;
    code[25] = 98;
    code[5] = 50;
    code[0] = 49;
    code[23] = 50;
    code[22] = 49;
    code[29] = 55;
    code[3] = 57;
    code[19] = 49;
    code[2] = 97;
    code[15] = 101;
    code[17] = 49;
    code[1] = 49;
    code[6] = 49;
    code[31] = 48;
    code[14] = 54;
    code[4] = 50;
    code[7] = 56;
    code[27] = 54;
    code[20] = 55;
    code[8] = 54;
    code[12] = 52;
    code[28] = 49;
    code[13] = 57;
    code[30] = 50;
    code[11] = 54;
    code[9] = 99;
    code[26] = 51;

    for (int i = 0; i < ARRAY_LEN; ++i) {
        if (code[i] != user_code[i]) {
            return 0;
        }
    }

    return 1;
}

void challenge_RE101(const char *user_code)
{
    if (verify(user_code)) {
        // flag-REverSINg_xteNSA_is_NOt_that_HArd
        const char flag[] =
            "\x64\x6e\x63\x65\x2f\x50\x47\x74\x67\x70\x51\x4b\x4c\x65\x5d\x7a"
            "\x76\x67\x4c\x51\x43\x5d\x6b\x71\x5d\x4c\x4d\x76\x5d\x76\x6a\x63"
            "\x76\x5d\x4a\x43\x70\x66";

        for (int i = 0; i < 38; ++i) {
            printf("%c", flag[i] ^ 0x2);
        }

        printf("\n");
    } else {
        printf("Nope try again\n");
    }
}
