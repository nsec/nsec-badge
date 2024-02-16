#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "nvs_flash.h"
#include "console.h"
#include "save.h"
#include "ota_init.h"


#if CONFIG_NSEC_BUILD_ADDON
    #include "challenges_storage.h"
    #include "crypto_atecc_init.h"
#endif

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
    fflush(stdout);
    Save::load_save();
    #if CONFIG_NSEC_BUILD_ADDON
        challenges_storage_init();
        crypto_atecc_init();
    #endif

    xTaskCreate(console_task, "console task", 4096, NULL, 3, NULL);
}
