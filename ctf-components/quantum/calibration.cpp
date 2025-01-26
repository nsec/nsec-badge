#include "calibration.h"


int cmd_calibrate(int argc, char **argv) {
    if (argc == 2) {
        
        // Validate if the input is a valid number
        char* endptr;
        long val = strtol(argv[1], &endptr, 10);
        if (!(*endptr == '\0')) {
            printf("Error: Invalid input. Please choose a calibration between 1 and 2.\n");
            return 0;
        }

        int calibration = std::strtol(argv[1], nullptr, 10);  // Convert string to integer

        if (calibration == 1) {
            printf("Initializing First Calibration Set... \n\n");
            char *input1a;
            char *input1b;
            char *input1c;
            badge_ssd1306_clear();

            //TODO add explanation Text paragraph on what needs to be submitted

            //TODO add storage validation for whether the calibration has already been done
            
            //If 1a is not completed
            badge_print_text(0, "Calibrate 1a: ", 14, false);
            input1a = linenoise("Input Calibrate 1a hash: ");
            if (input1a == nullptr) {
                printf("Error: Unable to read input\n");
                return 0;
            }
            // Validate 28fef057b852b45ff475b37d9918bf61
            if (strcmp(input1a, "28fef057b852b45ff475b37d9918bf61") == 0) {
                printf("Correct!\n\n");
                badge_print_text(0, "Calibrate 1a: O", 15, false);
                //TODO set memory and turn on LED
            } else {
                printf("Error: Invalid input. Please provide the correct hash.\n");
                badge_print_text(0, "Calibrate 1a: X", 15, false);
                linenoiseFree(input1a);
                return 0;
            }

            //If 1b is not completed
            badge_print_text(1, "Calibrate 1b: ", 14, false);
            input1b = linenoise("Input Calibrate 1b hash: ");
            if (input1b == nullptr) {
                printf("Error: Unable to read input\n");
                return 0;
            }
            // Validate 28fef057b852b45ff475b37d9918bf61
            if (strcmp(input1b, "28fef057b852b45ff475b37d9918bf61") == 0) {
                printf("Correct!\n\n");
                badge_print_text(1, "Calibrate 1b: O", 15, false);
                //TODO set memory and turn on LED
            } else {
                printf("Error: Invalid input. Please provide the correct hash.\n");
                badge_print_text(1, "Calibrate 1b: X", 15, false);
                linenoiseFree(input1a);
                linenoiseFree(input1b);
                return 0;
            }

            //If 1b is not completed
            badge_print_text(2, "Calibrate 1c: ", 14, false);
            input1c = linenoise("Input Calibrate 1c hash: ");
            if (input1c == nullptr) {
                printf("Error: Unable to read input\n");
                return 0;
            }
            // Validate 28fef057b852b45ff475b37d9918bf61
            if (strcmp(input1c, "28fef057b852b45ff475b37d9918bf61") == 0) {
                printf("Correct!\n\n");
                badge_print_text(2, "Calibrate 1c: O", 15, false);
                //TODO set memory and turn on LED
            } else {
                printf("Error: Invalid input. Please provide the correct hash.\n");
                badge_print_text(2, "Calibrate 1c: X", 15, false);
                linenoiseFree(input1a);
                linenoiseFree(input1b);
                linenoiseFree(input1c);
                return 0;
            }

            //At this point all three are validated, print flag format
            std::string str1a = input1a;
            std::string str1b = input1b;
            std::string str1c = input1c;

            std::string flag = "FLAG-" + str1a.substr(0, 5) + str1b.substr(0, 5) + str1c.substr(0, 5);

            linenoiseFree(input1a);
            linenoiseFree(input1b);
            linenoiseFree(input1c);

            printf("Congratulations, the first set of calibrations is correct!\n");
            printf("Your flag is: %s\n", flag.c_str());

            badge_print_textbox(3,  0, const_cast<char*>(flag.c_str()), 16, 16, false, 100);
            badge_print_textbox(3,  0, const_cast<char*>(flag.c_str()), 16, 20, false, 6);
        }
        //TODO - Calibration 2 set of validation
        else if (calibration == 2) {
            printf("Test Calibration 2\n");
            badge_ssd1306_clear();
            badge_print_text(0, "Calibration 2", 13, false);
        }
        else {
            printf("Error: Bad calibration input\n");
        }
        
        SmartLed leds(LED_WS2812B, nsec::board::neopixel::count, nsec::board::neopixel::ctrl_pin, 0);
        leds[12] = Rgb{0, 255, 0};  
        leds[13] = Rgb{255, 0, 0}; 
        leds[14] = Rgb{255, 0, 0};  
        leds[15] = Rgb{255, 0, 0};  
        leds[16] = Rgb{255, 0, 0};
        leds[17] = Rgb{255, 0, 0};   
        leds.show();
        leds.wait();

        return 0;

    } else {
        printf("Usage: calibrate <1-2>\n");
        return 0;
    }
}

void register_calibrate_cmd(void) {
        const esp_console_cmd_t cmd = {
            .command = "calibrate",
            .help = "Calibrate the Quantum Krackin' Device\n",
            .hint = "<1-2>",
            .func = &cmd_calibrate,
            .argtable = NULL,
        };
        ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
    }

