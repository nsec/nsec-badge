#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "console.h"

#define MOUNT_PATH "/spiffs"

static bool mount_spiffs() {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = MOUNT_PATH,
        .partition_label = NULL,
        .max_files = 8,
        .format_if_mount_failed = false
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    return ret == ESP_OK;
}

static void initialize_nvs(void) {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

extern "C" void app_main(void) {

    initialize_nvs();
    mount_spiffs();

    /* Print chip information */
    uint32_t flash_size;
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }
    fflush(stdout);

	xTaskCreate(console_task, "console task", 4096, NULL, 3, NULL);
}
