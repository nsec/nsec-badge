#include "save.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <stdio.h>

#define STORAGE_NAMESPACE "storage"

SaveData Save::save_data = {
    .neopixel_brightness = 25,
    .neopixel_mode = 0,
    .neopixel_is_on = true,
    .neopixel_color = 0xff00ff,
};

esp_err_t Save::write_save()
{
    nvs_handle_t my_handle;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_blob(my_handle, "save", reinterpret_cast<void *>(&save_data),
                       sizeof(save_data));

    if (err != ESP_OK) {
        return err;
    }

    err = nvs_commit(my_handle);
    if (err != ESP_OK) {
        return err;
    }

    nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t Save::load_save()
{
    nvs_handle_t my_handle;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE("save", "nvs open save failed");
        return err;
    }

    size_t save_size = sizeof(save_data);
    err = nvs_get_blob(my_handle, "save", reinterpret_cast<void *>(&save_data),
                       &save_size);
    if (err != ESP_OK) {
        return err;
    }

    nvs_close(my_handle);
    return ESP_OK;
}
