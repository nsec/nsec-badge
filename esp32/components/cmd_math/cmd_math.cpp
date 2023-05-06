#include "cmd_math.h"

void xor_decrypt(char* str) {
    int key = 0x4f;
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        str[i] = str[i] ^ key; // XOR each character with the key
    }
    str[len] = '\0';
}

int random_operation(int min, int max) {
    int result = 0;
    srand(time(NULL)); // Seed the random number generator with current time
    int num1 = rand() % (max - min + 1) + min; // Generate random number between min and max
    int num2 = rand() % (max - min + 1) + min; // Generate another random number between min and max
    int op = rand() % 3; // Generate a random integer between 0 and 
    switch (op) {
        case 0:
            printf("%d + %d = ", num1, num2);
            result = num1 + num2;
            break;
        case 1:
            printf("%d - %d = ", num1, num2);
            result = num1 - num2;
            break;
        case 2:
            printf("%d * %d = ", num1, num2);
            result = num1 * num2;
            break;
    }
    return result;
}

static int math_command(int argc, char **argv) {

    fflush(stdin);
    int success = 0;
    for (int i = 0; i < 1024; i++) {
        
        int result = random_operation(1, 10); // Generate random operation with numbers between 1 and 10
        int user_result;
        scanf("%d", &user_result); // Ask user for input
        if (user_result == result) {
            printf("Correct!\n");
            success++;
        } else {
            printf("Incorrect. The correct answer is %d.\n", result);
            return 0;
        }
    }
    if (success==1024) {
        char message[] = {9, 3, 14, 8, 98, 2, 123, 59, 39, 8, 124, 33, 126, 58, 107, 0};
        xor_decrypt(message);
        printf("%s\n", message);
    }
    return 0;
}

void register_math(void)
{
    const esp_console_cmd_t cmd = {
        .command = "math",
        .help = "Practice math!",
        .hint = NULL,
        .func = &math_command,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}  
