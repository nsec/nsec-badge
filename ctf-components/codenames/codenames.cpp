#include "codenames.h"


#define PIN_CLOCK     GPIO_NUM_2  // input (server -> client)
#define PIN_S2C_DATA  GPIO_NUM_7  // input (server -> client)
#define PIN_C2S_DATA  GPIO_NUM_6  // output (client -> server)

#define BIT_DELAY_US  30

#define CODENAMES_NAMESPACE "codenames"
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
    uint8_t key[25];
} CodenamesState;

// Using values for codenames key data
CodenamesState codenames_data = {
    .key = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};


void print_nvs_blob_codenames() {

    CodenamesState codenames_data; // Temporary variable to hold the NVS data
    size_t required_size = sizeof(CodenamesState);

    // Try to get the blob from NVS
    nvs_handle_t nvs_handle;
    esp_err_t err2 = nvs_open(CODENAMES_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err2 != ESP_OK) {
        ESP_LOGE(TAG, "CHECKING - Failed to open NVS namespace: %s\n", esp_err_to_name(err2));
        return;
    }
    err2 = nvs_get_blob(nvs_handle, "codenames_data", &codenames_data, &required_size);
    if (err2 == ESP_OK) {
        printf("CHECKING - NVS Key Data:\n");
        for (int i = 0; i < 25; i++) {
            printf("  %d : [%d] \n", i, codenames_data.key[i]);
        }
    } else if (err2 == ESP_ERR_NVS_NOT_FOUND) {
        printf("No key data found in NVS.\n");
    } else {
        printf("Error reading NVS blob: %s\n", esp_err_to_name(err2));
    }
}

void update_nvs_codenames()
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(CODENAMES_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s\n", esp_err_to_name(err));
        return;
    }

    // Store the codenames key data structure as a single blob
    err = nvs_set_blob(nvs_handle, "codenames_data", &codenames_data, sizeof(codenames_data));
    if (err == ESP_OK) {
        //printf("Codenames key data loaded from NVS successfully.\n");
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
    esp_err_t err = nvs_open(CODENAMES_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s\n", esp_err_to_name(err));
        return;
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        update_nvs_codenames();
        //exit get_nvs() since it will have a different nvs handle and fail
        return;
    }

    // Retrieve the codenames key data structure
    size_t data_size = sizeof(codenames_data);
    err = nvs_get_blob(nvs_handle, "codenames_data", &codenames_data, &data_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading key from NVS: %s\n", esp_err_to_name(err));
    }

    nvs_close(nvs_handle);
}

void clear_nvs_codenames(){
    nvs_handle_t handle;
    esp_err_t err;

     // Open NVS handle
     err = nvs_open(CODENAMES_NAMESPACE, NVS_READWRITE, &handle);
     if (err != ESP_OK) {
         ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
         return;
     }
 
     // Erase the blob from NVS
     err = nvs_erase_key(handle, "codenames_data");
     if (err == ESP_OK) {
         printf("Key erased successfully.\n");
     } else if (err == ESP_ERR_NVS_NOT_FOUND) {
         //printf("No NVS data found to erase, initializing defaults.\n");
     } else {
         //printf("Error erasing NVS data: %s\n", esp_err_to_name(err));
     }
}

void reset_nvs_codenames(){
    for(int i=0; i<25; i++){
        codenames_data.key[i] = 0;
    }
    update_nvs_codenames();
}


int validate(char* c){
    char *endptr;
    long i = strtol(c, &endptr, 10);
    
    if (*endptr != '\0' ) {
        return 1;
    }
    codenames_data.key[i] = 1;
    update_nvs_codenames();

    return 0;
}

// DOCK PART ###################################################################################

static void bus_init(void)
{
    // CLOCK + S2C_DATA as inputs
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_CLOCK) | (1ULL << PIN_S2C_DATA),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    // C2S_DATA as output
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << PIN_C2S_DATA);
    gpio_config(&io_conf);
    // default low
    gpio_set_level(PIN_C2S_DATA, 0);
}


static void cn_read_bits(char *outBuf, size_t numBits)
{
    for (size_t i = 0; i < numBits; i++)
    {
        // Wait for clock to go HIGH
        while (gpio_get_level(PIN_CLOCK) == 0) {
            vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
        }
        // Read the data line
        int bitVal = gpio_get_level(PIN_S2C_DATA);
        // Wait for clock to go LOW
        while (gpio_get_level(PIN_CLOCK) == 1) {
            vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
        }
        outBuf[i] = bitVal ? '1' : '0';
    }
    outBuf[numBits] = '\0';
}

static void cn_send_bits(const char *bits)
{
    size_t len = strlen(bits);
    for (size_t i = 0; i < len; i++)
    {
        bool bitVal = (bits[i] == '1');
        gpio_set_level(PIN_C2S_DATA, bitVal ? 1 : 0);

        // The server toggles the clock, so we wait for one full cycle each bit
        // Wait for clock HIGH
        while (gpio_get_level(PIN_CLOCK) == 0) {
            vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
        }
        // Wait for clock LOW
        while (gpio_get_level(PIN_CLOCK) == 1) {
            vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
        }
    }
    // Drive it low after done
    gpio_set_level(PIN_C2S_DATA, 0);
}

static void get_key(char *key_chars)
{
    for (int i = 0; i < 25; i++) {
        key_chars[i] = (char)codenames_data.key[i];
    }
    key_chars[25] = '\0'; 
}

// DOCK PART ###################################################################################


int cmd_codenames(int argc, char **argv) {
    char* input_val;
    

    if (argc == 1) {
        
        printf("Usage: codenames\n");
        for (int i = 0; i < 20; i++) {
            printf("Question %d: %s\n", i, questions[i]);
        }
        printf("\n");
        for (int i = 0; i < 25; i++) {
            printf("Answer %s\n", answers[i]);
        }
        return 0;

    }else if(argc == 2 ){
        char *endptr;
        int question_index = strtol(argv[1], &endptr, 10);
        
        if (*endptr != '\0') {
            if(strcmp(endptr, "--all") == 0){
                get_nvs_codenames();
                for (int i = 0; i < 20; i++) {
                    input_val = linenoise(questions[i]);
                    if(validate(input_val)){
                        return 1;
                    }
                }
                printf("\n\nEND\n\n");
            } else if(strcmp(endptr, "--clear") == 0){
                reset_nvs_codenames();
            } else if(strcmp(endptr, "--dock-ready") == 0){
                get_nvs_codenames();
                printf("Setting badge for dock connection\n");
                bus_init();
                char initdata[2];
                cn_read_bits(initdata, 1);
                char key[25+1];
                get_key(key);
                cn_send_bits(key);
            } else if(strcmp(endptr, "--show-questions") == 0){
                for (int i = 0; i < 20; i++) {
                    printf("Question %d: %s\n", i, questions[i]);
                }
                printf("\n");
            } else if(strcmp(endptr, "--show-answers") == 0){
                for (int i = 0; i < 25; i++) {
                    printf("Answer %s\n", answers[i]);
                }
                printf("\n");
            } else {
                printf("Invalid question number. Please choose a number between 0 and 19 - or --all to run all 20 questions.\n");
                print_nvs_blob_codenames();
                return 1;
            }

        }  else if (question_index >= 0 && question_index <= 19) {
            get_nvs_codenames();
            input_val = linenoise(questions[question_index]);
            if(validate(input_val)){
                return 1;
            }
        } else {
            printf("Invalid question number. Please choose a number between 0 and 19 - or --all to run all 20 questions. \n");
            return 0;
        }
    }
    return 0;
}

void register_codenames_cmd(void) {
    const esp_console_cmd_t cmd = {
        .command = "codenames",
        .help = "This challenge set a key on your badge. \n"
                "Answer every questions with the proper answer's index \n"
                "and the key will be set properly\n"
                "<0-19>     - Answer a specific question by its index \n"
                "--all      - Answer all 20 questions \n"
                "--clear    - Reset key data \n"
                "--dock-ready       - Put the badge in a state ready to share the key \n"
                "--show-questions   - Display questions and their index \n"
                "--show-answers     - Display answers and their index \n",
        .hint = "[<0-19> | --all | --clear | --dock-ready | --show-questions | --show-answers] \n",
        .func = &cmd_codenames,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}