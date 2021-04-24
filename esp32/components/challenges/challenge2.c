#include <stdio.h>

static int verify(const char *user_code)
{
    // expected user input: f219e6cdb1fa4a48b160d00d61918f93
    // code after transformations: 6a3a3571744549cbdcaee83c754488a9
    unsigned int i = 0;
    unsigned int j = 18;

    // clang-format off
    // code[0]
    if ((user_code[i] ^ 0x50) == '6') {
        i += 31;
        // code[31]
        if ((user_code[i] ^ 0xa) == '9') {
            i -= 10;
            // code[21]
            if ((user_code[i] ^ 0x8) == '8') {
                i -= 1;
                // code[20]
                if ((user_code[i] ^ 0x1) == 'e') {
                    i /= 2;
                    // code[10]
                    if ((user_code[i] ^ 0x52) == '4') {
                        i /= 2;
                        // code[5]
                        if ((user_code[i] ^ 0x3) == '5') {
                            i *= 3;
                            // code[15]
                            if ((user_code[i] ^ 0x5a) == 'b') {
                                i -= 1;
                                // code[14]
                                if ((user_code[i] ^ 0x57) == 'c') {
                                    i ^= 13;
                                    // code[3]
                                    if ((user_code[i] ^ 0x58) == 'a') {
                                        i -= 1;
                                        // code[2]
                                        if ((user_code[i] ^ 0x2) == '3') {
                                            i = 1;
                                            // code[1]
                                            if ((user_code[i] ^ 0x53) == 'a') {
                                                i *= 8;
                                                // code[8]
                                                if ((user_code[i] ^ 0x55) == '7') {
                                                    i *= 2;
                                                    // code[16]
                                                    if ((user_code[i] ^ 0x6) == 'd') {
                                                        i += 5;
                                                        i += 1;
                                                        // code[22]
                                                        if ((user_code[i] ^ 0x3) == '3') {
                                                            i -= 1;
                                                            i += 2;
                                                            // code[23]
                                                            if ((user_code[i] ^ 0x7) == 'c') {
                                                                i += 1;
                                                                // code[24]
                                                                if ((user_code[i] ^ 0x1) == '7') {
                                                                    i += 1;
                                                                    // code[25]
                                                                    if ((user_code[i] ^ 0x4) == '5') {
                                                                        i += 2;
                                                                        // code[27]
                                                                        if ((user_code[i] ^ 0x5) == '4') {
                                                                            i += 2;
                                                                            // code[29]
                                                                            if ((user_code[i] ^ 0x5e) == '8') {
                                                                                i += 1;
                                                                                // code[30]
                                                                                if ((user_code[i] ^ 0x58) == 'a') {
                                                                                    i -= 2;
                                                                                    // code[28]
                                                                                    if ((user_code[i] ^ 0x0) == '8') {
                                                                                        i -= 10;
                                                                                        // code[18]
                                                                                        if ((user_code[j] ^ 0x57) == 'a') {
                                                                                            j ^= 1;
                                                                                            // code[19]
                                                                                            if ((user_code[j] ^ 0x55) == 'e') {
                                                                                                j -= 2;
                                                                                                // code[17]
                                                                                                if ((user_code[j] ^ 0x52) == 'c') {
                                                                                                    j -= 4;
                                                                                                    // code[13];
                                                                                                    if ((user_code[j] ^ 0x58) == '9') {
                                                                                                        j -= 1;
                                                                                                        // code[12]
                                                                                                        if ((user_code[j] ^ 0x0) == '4') {
                                                                                                            j -= 1;
                                                                                                            // code[11]
                                                                                                            if ((user_code[j] ^ 0x54) == '5') {
                                                                                                                j -= 2;
                                                                                                                // code[9]
                                                                                                                if ((user_code[j] ^ 0x5) == '4') {
                                                                                                                    j -= 2;
                                                                                                                    // code[7]
                                                                                                                    if ((user_code[j] ^ 0x55) == '1') {
                                                                                                                        j -= 1;
                                                                                                                        // code[6]
                                                                                                                        if ((user_code[j] ^ 0x54) == '7') {
                                                                                                                            j -= 2;
                                                                                                                            // code[6]
                                                                                                                            if ((user_code[j] ^ 0x56) == '3') {
                                                                                                                                return 1;
                                                                                                                            }
                                                                                                                        }
                                                                                                                    }
                                                                                                                }
                                                                                                            }
                                                                                                        }
                                                                                                    }
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // clang-format on

    return 0;
}

void challenge_RE102(const char *user_code)
{
    if (verify(user_code)) {
        // flag-this_is_a_big_huge_enormous_condition
        const char flag[] =
            "\x64\x6E\x63\x65\x2F\x76\x6A\x6B\x71\x5D\x6B\x71\x5D\x63\x5D\x60"
            "\x6B\x65\x5D\x6A\x77\x65\x67\x5D\x67\x6C\x6D\x70\x6F\x6D\x77\x71"
            "\x5D\x61\x6D\x6C\x66\x6B\x76\x6B\x6D\x6C";

        for (int i = 0; i < 42; ++i) {
            printf("%c", flag[i] ^ 0x2);
        }

        printf("\n");
    } else {
        printf("Nope try again\n");
    }
}
