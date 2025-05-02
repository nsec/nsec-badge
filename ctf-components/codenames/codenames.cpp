#include "codenames.h"

#define NAMESPACE "codenames"
static const char *TAG = "codenames";

static const char *questions[] = {
    "What hobby involves communicating via radio waves as a non-professional?",
    "What device both transmits and receives radio signals?",
    "What is used to radiate and receive radio waves?",
    "What unit is used to measure the rate of radio wave oscillations?",
    "What term describes how radio waves travel through the atmosphere?",
    "What unique identifier is assigned to each amateur radio station?",
    "What card is exchanged as confirmation of a radio contact?",
    "What mode uses dots and dashes for communication?",
    "What voice mode uses a single sideband of a signal?",
    "What voice mode uses frequency variation for modulation?",
    "What digital mode uses a teleprinter for communication?",
    "What system reports real-time location and data via radio?",
    "What technique transmits still images over radio?",
    "What is a popular digital mode for weak signal communication?",
    "What is the reception of meteorological images via radio called?",
    "What term refers to long-distance radio communication?",
    "What device re-transmits signals to extend range?",
    "What segment of the radio spectrum is used for communication?",
    "What term refers to radio communication activity?",
    "What type of radio waves are used for long-distance communication?"
};

static const char *answers[] = {
    "0. PSK63",
    "1. UHF (Ultra high frequency)",
    "2. Transceiver",
    "3. DX (Long distance)",
    "4. CW (Morse code)",
    "5. Microphone",
    "6. SSTV (Slow Scan Television)",
    "7. Repeater",
    "8. VHF (Very high frequency)",
    "9. Call sign",
    "10. Band",
    "11. FM (Frequency modulation)",
    "12. Weather fax",
    "13. Antenna",
    "14. APRS (Automatic Packet Reporting System)",
    "15. Traffic",
    "16. SWR (Standing wave ratio)",
    "17. SSB (Single sideband)",
    "18. RTTY (Radioteletype)",
    "19. QSL card",
    "20. Amplifier",
    "21. Amateur radio",
    "22. Shortwave",
    "23. Propagation",
    "24. Hertz"
};

typedef struct {
    uint8_t key[25][1];
} CodenamesState;

// Using values for calibration data
CodenamesState codenames_data = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// Using values for calibration data
CodenamesState default_codenames_data = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};


void print_nvs_blob_codenames() {

    CodenamesState codenames_data; // Temporary variable to hold the NVS data
    size_t required_size = sizeof(CodenamesState);

    // Try to get the blob from NVS
    nvs_handle_t nvs_handle;
    esp_err_t err2 = nvs_open(NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err2 != ESP_OK) {
        ESP_LOGE(TAG, "CHECKING - Failed to open NVS namespace: %s\n", esp_err_to_name(err2));
        return;
    }
    err2 = nvs_get_blob(nvs_handle, "codenames_data", &codenames_data, &required_size);
    if (err2 == ESP_OK) {
        printf("CHECKING - NVS Calibration Data:\n");
        //for (int i = 0; i < 6; i++) {
        //    printf("  codena[%d]: %d, hash[%d]: %s\n", i, calib_data.calib[i], i, calib_data.hashes[i]);
        //}
        printf("TBD - To fill out\n");
    } else if (err2 == ESP_ERR_NVS_NOT_FOUND) {
        printf("No calibration data found in NVS.\n");
    } else {
        printf("Error reading NVS blob: %s\n", esp_err_to_name(err2));
    }
}

void update_nvs_codenames()
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s\n", esp_err_to_name(err));
        return;
    }

    // Store the calibration data structure as a single blob
    err = nvs_set_blob(nvs_handle, "codenames_data", &codenames_data, sizeof(codenames_data));
    if (err == ESP_OK) {
        //printf("Calibration data loaded from NVS successfully.\n");
    } else {
        ESP_LOGE(TAG, "Error reading NVS: %s\n", esp_err_to_name(err));
    }

    // Commit changes
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit changes to NVS: %s\n", esp_err_to_name(err));
    }

    nvs_close(nvs_handle);
}

void get_nvs_codenames()
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s\n", esp_err_to_name(err));
        return;
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        update_nvs_codenames();
        //exit get_nvs() since it will have a different nvs handle and fail
        return;
    }

    // Retrieve the calibration data structure
    size_t data_size = sizeof(codenames_data);
    err = nvs_get_blob(nvs_handle, "codenames_data", &codenames_data, &data_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading calibration data from NVS: %s\n", esp_err_to_name(err));
    }

    nvs_close(nvs_handle);
}


//TODO FUNCTION
int validate(char* c){
    printf("INPUT: %s\n\n",c);
    char *endptr;
    long i = strtol(c, &endptr, 10);
    
    if (*endptr != '\0' ) {
        return 1;
    }
    codenames_data.key[i][0] = 1;
    update_nvs_codenames();

    return 0;
}


int cmd_codenames(int argc, char **argv) {
    char* input_val;

    if (argc == 1) {
        printf("Usage: codenames\n");
        for (int i = 0; i < 20; i++) {
            printf("Question %d: %s\n", i, questions[i]);
        }
        printf("\n");
        for (int i = 0; i < 20; i++) {
            printf("Answer %s\n", answers[i]);
        }
        return 0;

    }else if(argc == 2 ){
        char *endptr;
        long question_index = strtol(argv[1], &endptr, 10);
        
        if (*endptr != '\0' && strcmp(endptr, "--all") == 0) {
            for (int i = 0; i < 20; i++) {
                input_val = linenoise(questions[i]);
                if(validate(input_val)){
                    return 1;
                }
            }

        } else if (question_index < 0 || question_index > 19) {
            printf("Invalid question number. Please choose a number between 0 and 19 - or --all to run all 20 questions.\n");
            return 1;
        } else {
            input_val = linenoise(questions[question_index]);
            if(validate(input_val)){
                return 1;
            }
        }
     } else {
        printf("Usage: codenames\n");
    }
    return 0;
}

void register_codenames_cmd(void) {
    const esp_console_cmd_t cmd = {
        .command = "codenames",
        .help = " tbd\n",
        .hint = "tbd",
        .func = &cmd_codenames,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}