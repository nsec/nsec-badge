#include "calibration.h"

#define QUANTUM_NAMESPACE "quantum"

// Define the structure for calibration data
typedef struct {
    uint8_t calib[6];
    char hashes[6][6];
} CalibrationData;

// Using values for calibration data
CalibrationData calib_data = {
    {0, 0, 0, 0, 0, 0},
    {"00000", "00000", "00000", "00000", "00000", "00000"}
};

// Default values for calibration data
CalibrationData default_calib_data = {
    {0, 0, 0, 0, 0, 0},     // Default calibration values
    {"00000", "00000", "00000", "00000", "00000", "00000"} // Default hashes
};

bool leds_on = true;

void print_nvs_blob() {
    // Define the structure for calibration data
    //typedef struct {
    //    uint8_t calib[6];       // Calibration integers (0 or 1)
    //    char hashes[6][6];      // Corresponding hashes (5 chars + null terminator)
    //} CalibrationData;

    CalibrationData calib_data; // Temporary variable to hold the NVS data
    size_t required_size = sizeof(CalibrationData);

    // Try to get the blob from NVS
    nvs_handle_t nvs_handle;
    esp_err_t err2 = nvs_open(QUANTUM_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err2 != ESP_OK) {
        printf("CHECKING - Failed to open NVS namespace: %s\n", esp_err_to_name(err2));
        return;
    }
    err2 = nvs_get_blob(nvs_handle, "calib_data", &calib_data, &required_size);
    if (err2 == ESP_OK) {
        printf("CHECKING - NVS Calibration Data:\n");
        for (int i = 0; i < 6; i++) {
            printf("  calib[%d]: %d, hash[%d]: %s\n", i, calib_data.calib[i], i, calib_data.hashes[i]);
        }
    } else if (err2 == ESP_ERR_NVS_NOT_FOUND) {
        printf("No calibration data found in NVS.\n");
    } else {
        printf("Error reading NVS blob: %s\n", esp_err_to_name(err2));
    }
}

void clear_nvs_data() {
    nvs_handle_t handle;
    esp_err_t err;

    // Open NVS handle
    err = nvs_open(QUANTUM_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        //printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }

    // Erase the blob from NVS
    err = nvs_erase_key(handle, "calib_data");
    if (err == ESP_OK) {
        printf("NVS data erased successfully.\n");
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        //printf("No NVS data found to erase, initializing defaults.\n");
    } else {
        //printf("Error erasing NVS data: %s\n", esp_err_to_name(err));
    }

    // Write default calibration data back to NVS
    err = nvs_set_blob(handle, "calib_data", &default_calib_data, sizeof(default_calib_data));
    if (err != ESP_OK) {
        //printf("Failed to write default calibration data to NVS: %s\n", esp_err_to_name(err));
    } else {
        //printf("Default calibration data written to NVS.\n");
    }

    // Commit changes
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        //printf("Failed to commit NVS changes: %s\n", esp_err_to_name(err));
    } else {
       // printf("NVS changes committed.\n");
    }

    // Close NVS handle
    nvs_close(handle);
}

void update_nvs()
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(QUANTUM_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        //printf("Failed to open NVS namespace: %s\n", esp_err_to_name(err));
        return;
    }

    // Store the calibration data structure as a single blob
    err = nvs_set_blob(nvs_handle, "calib_data", &calib_data, sizeof(calib_data));
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // If no data found, initialize with defaults
        //printf("No calibration data found in NVS. Initializing with default values...\n");
    } else if (err == ESP_OK) {
        //printf("Calibration data loaded from NVS successfully.\n");
    } else {
        //printf("Error reading NVS: %s\n", esp_err_to_name(err));
    }

    // Commit changes
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        //printf("Failed to commit changes to NVS: %s\n", esp_err_to_name(err));
    }

    nvs_close(nvs_handle);
}

void get_nvs()
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(QUANTUM_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        //printf("Failed to open NVS namespace: %s\n", esp_err_to_name(err));
        return;
    }

    // Retrieve the calibration data structure
    size_t data_size = sizeof(calib_data);
    err = nvs_get_blob(nvs_handle, "calib_data", &calib_data, &data_size);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        //printf("Calibration data not found. Using default values.\n");
        update_nvs();
    } else if (err != ESP_OK) {
        //printf("Error reading calibration data from NVS: %s\n", esp_err_to_name(err));
    } else {
        //printf("Calibration data retrieved successfully.\n");
    }

    nvs_close(nvs_handle);
}

void update_leds()
{
    SmartLed leds(LED_WS2812B, nsec::board::neopixel::count, nsec::board::neopixel::ctrl_pin, 0);
    for (int i = 0; i < 6; i++) {
        if (calib_data.calib[i] == 0) {
            leds[12 + i] = Rgb{128, 0, 0};
        } else if (calib_data.calib[i] == 1) {
            leds[12 + i] = Rgb{0, 128, 0};
        } else{
            leds[12 + i] = Rgb{0, 0, 0};
        }
    }

    leds.show();
    leds.wait();
}

void toggle_leds() {
    leds_on = !leds_on; // Toggle the state
    
    if (leds_on) {
        update_leds();
    } else {
        SmartLed leds(LED_WS2812B, nsec::board::neopixel::count, nsec::board::neopixel::ctrl_pin, 0);
        for (int i = 0; i < 6; i++) {
            leds[12 + i] = Rgb{0, 0, 0};
        }
        leds.show();
        leds.wait();
    }
}

void calibrate_1()
{
    printf("Initializing First Calibration Set... \n\n");
    
    char *input_hash;
    
    badge_ssd1306_clear();

    //TODO add storage validation for whether the calibration has already been done

    get_nvs();
    update_leds();

    //TODO add explanation Text paragraph on what needs to be submitted

    if(calib_data.calib[0] != 1)
    {
        //If 1a is not completed
        badge_print_text(0, "Calibrate 1a: ", 14, false);
        input_hash = linenoise("Input Calibrate 1a hash: ");
        if (input_hash == nullptr) {
            printf("Error: Unable to read input\n");
            return;
        }
        // Validate 28fef057b852b45ff475b37d9918bf61
        if (strcmp(input_hash, "28fef057b852b45ff475b37d9918bf61") == 0) {
            printf("Correct!\n\n");
            badge_print_text(0, "Calibrate 1a: O", 15, false);
            calib_data.calib[0] = 1;
            strncpy(calib_data.hashes[0], input_hash, 5);
            calib_data.hashes[0][5] = '\0';
            update_nvs();
            get_nvs();
            update_leds();
            linenoiseFree(input_hash);
        } else {
            printf("Error: Invalid input. Please provide the correct hash.\n");
            badge_print_text(0, "Calibrate 1a: X", 15, false);
            update_leds();
            linenoiseFree(input_hash);
            return;
        }
    } else {
        printf("Previously Correct Calibration 1a Detected!\n\n");
        badge_print_text(0, "Calibrate 1a: O", 15, false);
    }

    if(calib_data.calib[1] != 1)
    {   
        //If 1b is not completed
        badge_print_text(1, "Calibrate 1b: ", 14, false);
        input_hash = linenoise("Input Calibrate 1b hash: ");
        if (input_hash == nullptr) {
            printf("Error: Unable to read input\n");
            return;
        }
        // Validate 28fef057b852b45ff475b37d9918bf61
        if (strcmp(input_hash, "28fef057b852b45ff475b37d9918bf61") == 0) {
            printf("Correct!\n\n");
            badge_print_text(1, "Calibrate 1b: O", 15, false);
            calib_data.calib[1] = 1;
            strncpy(calib_data.hashes[1], input_hash, 5);
            calib_data.hashes[1][5] = '\0';
            update_nvs();
            get_nvs();
            update_leds();
            linenoiseFree(input_hash);
        } else {
            printf("Error: Invalid input. Please provide the correct hash.\n");
            badge_print_text(1, "Calibrate 1b: X", 15, false);        
            update_leds();
            linenoiseFree(input_hash);
            return;
        }
    } else {
        printf("Previously Correct Calibration 1b Detected!\n\n");
        badge_print_text(1, "Calibrate 1b: O", 15, false);
    }

    if(calib_data.calib[2] != 1)
    {   
        //If 1c is not completed
        badge_print_text(2, "Calibrate 1c: ", 14, false);
        input_hash = linenoise("Input Calibrate 1c hash: ");
        if (input_hash == nullptr) {
            printf("Error: Unable to read input\n");
            return;
        }
        // Validate 28fef057b852b45ff475b37d9918bf61
        if (strcmp(input_hash, "28fef057b852b45ff475b37d9918bf61") == 0) {
            printf("Correct!\n\n");
            badge_print_text(2, "Calibrate 1c: O", 15, false);
            calib_data.calib[2] = 1;
            strncpy(calib_data.hashes[2], input_hash, 5);
            calib_data.hashes[2][5] = '\0';
            update_nvs();
            get_nvs();
            update_leds();
            linenoiseFree(input_hash);
        } else {
            printf("Error: Invalid input. Please provide the correct hash.\n");
            badge_print_text(2, "Calibrate 1c: X", 15, false);
            update_leds();
            linenoiseFree(input_hash);
            return;
        }
    } else {
        printf("Previously Correct Calibration 1c Detected!\n\n");
        badge_print_text(2, "Calibrate 1c: O", 15, false);
    }

    std::string flag = "FLAG-" +
                std::string(calib_data.hashes[0]) +
                std::string(calib_data.hashes[1]) +
                std::string(calib_data.hashes[2]);

    printf("Your flag is: %s\n", flag.c_str());
    if(calib_data.calib[0] == 1 && calib_data.calib[1] == 1 && calib_data.calib[2] == 1)
    {
        printf("Congratulations, the first set of calibrations is correct!\n");
    } 
    else
    {
        printf("Oops... Calibrations are not correct, try again\n");
    }
        
    badge_print_textbox(3,  0, const_cast<char*>(flag.c_str()), 16, 16, false, 100);
    badge_print_textbox(3,  0, const_cast<char*>(flag.c_str()), 16, 20, false, 6);
}

int cmd_calibrate(int argc, char **argv) {
    if (argc == 2) {
        
        esp_log_level_set("gpio", ESP_LOG_WARN);

        // Check if the input is "clear"
        if (strcmp(argv[1], "clear") == 0) {
            // Call the NVS clear function
            printf("Clearing calibration data...\n");
            clear_nvs_data();
            printf("Calibration data reset to defaults.\n");
            get_nvs();
            update_leds();
            esp_log_level_set("gpio", ESP_LOG_INFO);
            return 0;
        }

        if (strcmp(argv[1], "toggle") == 0) {
            toggle_leds();
            esp_log_level_set("gpio", ESP_LOG_INFO);
            return 0;
        }

        // Validate if the input is a valid number
        char* endptr;
        long val = strtol(argv[1], &endptr, 10);
        if (!(*endptr == '\0')) {
            printf("Error: Invalid input. Please choose a calibration between 1 and 2.\n");
            return 0;
        }

        int calibration = std::strtol(argv[1], nullptr, 10);  // Convert string to integer

        if (calibration == 1) {
            calibrate_1();
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
        esp_log_level_set("gpio", ESP_LOG_INFO);
        return 0;

    } else {
        printf("Usage: calibrate <1-2>\n");
        return 0;
    }
}

void register_calibrate_cmd(void) {
        const esp_console_cmd_t cmd = {
            .command = "calibrate",
            .help = " <1-2> - calibration challenges\n clear - reset calibration progress\n toggle - toggle LED progress on/off\n Calibrate the Quantum Kracken Device\n",
            .hint = "<1-2> | clear | toggle",
            .func = &cmd_calibrate,
            .argtable = NULL,
        };
        ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
    }

