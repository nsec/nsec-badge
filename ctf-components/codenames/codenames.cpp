#include "codenames.h"

#define NAMESPACE "codenames"
static const char *TAG = "codenames";

typedef struct {
    uint8_t matrix[16][1];
} CodenamesState;

// Using values for calibration data
CodenamesState codenames_data = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// Using values for calibration data
CodenamesState default_codenames_data = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
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
    err2 = nvs_get_blob(nvs_handle, "calib_data", &codenames_data, &required_size);
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
    err = nvs_get_blob(nvs_handle, "calib_data", &codenames_data, &data_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading calibration data from NVS: %s\n", esp_err_to_name(err));
    }

    nvs_close(nvs_handle);
}

//TODO FUNCTION


int cmd_codenames(int argc, char **argv) {
    if (argc == 1) {
        printf("Codenames command executed\n");
        //TODO: Call function for player to input values
        return 0;
    } else {
        printf("Usage: codenames\n");
        return 0;
    }
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