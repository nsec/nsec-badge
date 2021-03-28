#include "esp_console.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "buttons.h"
#include "cmd.h"
#include "graphics.h"
#include "rpg.h"

static bool mount_spiffs()
{
    esp_vfs_spiffs_conf_t conf = {.base_path = "/spiffs",
                                  .partition_label = NULL,
                                  .max_files = 8,
                                  .format_if_mount_failed = false};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    return ret == ESP_OK;
}

extern "C" void app_main(void)
{
    if (!mount_spiffs())
        abort();

    initialize_nvs();
    nsec_buttons_init();
    graphics_start();
    xTaskCreate(console_task, "console task", 4096, NULL, 3, NULL);

    run_main_scene();
}
