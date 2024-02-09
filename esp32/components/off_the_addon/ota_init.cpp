#include <string.h>
#include "esp_flash.h"
#include "esp_flash_spi_init.h"
#include "driver/spi_common.h"
#include "soc/spi_pins.h"
#include "esp_log.h"
#include "ota_init.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "rom/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_console.h"

#include "ota_actions.h"

static const char *TAG = "ota_init";
static esp_flash_t* flash = NULL;

spi_bus_config_t init_spi_bus(void);
esp_flash_t* init_ext_flash(void);
void blink_blueled_task(void *pvParameter);
void blink_blueled_ntime_task(void *pvParameter);

void ota_init() {
    gpio_pad_select_gpio(ADDON_DETECT);
    gpio_set_direction(ADDON_DETECT, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ADDON_DETECT, GPIO_PULLUP_ONLY);

    if (gpio_get_level(ADDON_DETECT) == 0) {
        ESP_LOGI(TAG, "CTF Addon detected");
        gpio_pad_select_gpio(ADDON_BLUE_LED);
        gpio_set_direction(ADDON_BLUE_LED, GPIO_MODE_OUTPUT);
        gpio_set_level(ADDON_BLUE_LED, 1);
        flash = init_ext_flash();
        if (flash != NULL) {
            // Use this to load the flash from OTA 0 at provisionning
            // storage_read_from_ota(0, flash);
            // return;
            if(write_flash_to_ota(flash)) {
                // Addon plugged in and OTA already flashed
                int times = 3;
                xTaskCreate(blink_blueled_ntime_task, "Blink blue LED task", 1024, &times, 2, NULL);
            } else {
                int ms_delay = 500;
                xTaskCreate(blink_blueled_task, "Blink blue LED task", 1024, &ms_delay, 2, NULL);
            }
        } else {
            int ms_delay = 1000;
            xTaskCreate(blink_blueled_task, "Blink blue LED task", 1024, &ms_delay, 2, NULL);
        }
    } else {
        ESP_LOGI(TAG, "CTF Addon not detected");
    }
}

void register_ota_cmd(void) {
    std::string firmware_select_string = get_firmware_select_string();
    if(firmware_select_string != "[conf]") {
        // Will show the command only if we find more than the conf firmware
        const esp_console_cmd_t cmd = {
            .command = "firmware_select",
            .help = "Select which firmware to run\n",
            .hint = firmware_select_string.c_str(),
            .func = &ota_cmd,
            .argtable = NULL,        
        };
        ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
    }
}

void blink_blueled_ntime_task(void *pvParameter) {
    int n = *(int*)pvParameter;
    // make sure the addon is detected before we loop and blink the blue LED
    if (gpio_get_level(ADDON_DETECT) == 0) {
        for (int i = 0; i < n; i++) {
            gpio_set_level(ADDON_BLUE_LED, 0);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            gpio_set_level(ADDON_BLUE_LED, 1);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
}

void blink_blueled_task(void *pvParameter) {
    int ms_delay = *(int*)pvParameter; 
    if (ms_delay == 0) {
        ms_delay = 100;
    }
    // make sure the addon is detected before we loop and blink the blue LED
    if (gpio_get_level(ADDON_DETECT) == 0) {
        while (1) {
            gpio_set_level(ADDON_BLUE_LED, 0);
            vTaskDelay(ms_delay / portTICK_PERIOD_MS);
            gpio_set_level(ADDON_BLUE_LED, 1);
            vTaskDelay(ms_delay / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
}

void error_blink() {

}

spi_bus_config_t init_spi_bus(void) {
    const spi_bus_config_t bus_config = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK
    };
    // Initialize the SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(HOST_ID, &bus_config, SPI_DMA_CHAN));
    return bus_config;
}

esp_flash_t* init_ext_flash() {
    int chip_select = PIN_CS;
    spi_bus_config_t bus_config = init_spi_bus();

    const esp_flash_spi_device_config_t device_config = {
        .host_id = HOST_ID,
        .cs_io_num = chip_select,
        // SPI_FLASH_SLOWRD = 0, ///< Data read using single I/O, some limits on speed
        // SPI_FLASH_FASTRD, ///< Data read using single I/O, no limit on speed
        // SPI_FLASH_DOUT,   ///< Data read using dual I/O
        // SPI_FLASH_DIO,    ///< Both address & data transferred using dual I/O
        // SPI_FLASH_QOUT,   ///< Data read using quad I/O
        // SPI_FLASH_QIO,    ///< Both address & data transferred using quad I/O
        .io_mode = SPI_FLASH_FASTRD,

        // Workaround to go with the patch https://github.com/nsec/nsec-badge-dev/commit/bc7d0ba8efd510505f9f0ae2035eb2fab3aa7a5e
        // This might be needed only if you are using 2 SPI devices and/or when the PINs are not iomuxable
        //.cs_id = 1337, 

        .freq_mhz = 20,
    };

    ESP_LOGI(TAG, "Initializing external SPI Flash at %dMHz", device_config.freq_mhz);
    /*ESP_LOGI(TAG, "Pin assignments:");
    ESP_LOGI(TAG, "MOSI: %2d   MISO: %2d   SCLK: %2d   CS: %2d",
        bus_config.mosi_io_num, bus_config.miso_io_num,
        bus_config.sclk_io_num, device_config.cs_io_num
    );*/

    // Add device to the SPI bus
    esp_flash_t* ext_flash;
    ESP_ERROR_CHECK(spi_bus_add_flash_device(&ext_flash, &device_config));

    // Probe the Flash chip and initialize it
    esp_err_t err = esp_flash_init(ext_flash);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize external Flash: %s (0x%x)", esp_err_to_name(err), err);
        return NULL;
    }

    // Print out the ID and size
    uint32_t id;
    ESP_ERROR_CHECK(esp_flash_read_id(ext_flash, &id));
    ESP_LOGI(TAG, "Initialized external Flash, size=%lu KB, size=%lu bits, ID=0x%x", ext_flash->size / 1024, ext_flash->size, (int)id);

    return ext_flash;
}
