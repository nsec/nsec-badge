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
#include "ota_write.h"

static const char *TAG = "ota_init";
static esp_flash_t* flash = NULL;

spi_bus_config_t init_spi_bus(void);
esp_flash_t* init_ext_flash(void);

void check_ctf_addon() {

}

void ota_init() {
    gpio_pad_select_gpio(ADDON_DETECT);
    gpio_set_direction(ADDON_DETECT, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ADDON_DETECT, GPIO_PULLUP_ONLY);

    int lvl = gpio_get_level(ADDON_DETECT);
    if (lvl == 0) {
        ESP_LOGI(TAG, "CTF Addon detected");
        gpio_pad_select_gpio(ADDON_BLUE_LED);
        gpio_set_direction(ADDON_BLUE_LED, GPIO_MODE_OUTPUT);
        gpio_set_level(ADDON_BLUE_LED, 1);
        flash = init_ext_flash();
        if (flash != NULL) {
            // storage_read_from_ota(0, flash);
            // return;

            // Check if OTA partition already has the firmware
            esp_partition_subtype_t subtype = NSEC_OTA_PARTITION;
            const esp_partition_t *ota_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, subtype, NULL);
            if (ota_partition != NULL) {
                size_t size = ota_partition->size;
                ESP_LOGI(TAG, "Found OTA partition %d! size: %d", NSEC_OTA_ID, size);
                esp_app_desc_t desc;
                esp_err_t err = esp_ota_get_partition_description(ota_partition, &desc);
                if (err != ESP_OK) {
                    ESP_LOGI(TAG, "No ota_%d partition firmware detected: %s (0x%x)", NSEC_OTA_ID, esp_err_to_name(err), err);
                } else {
                    ESP_LOGI(TAG, "OTA %d firmware project: %s", NSEC_OTA_ID, desc.project_name);
                    ESP_LOGI(TAG, "OTA %d firmware version: %s", NSEC_OTA_ID, desc.version);
                }
                if (strcmp(desc.project_name, "nsec-ctf-addon") == 0) {
                    ESP_LOGI(TAG, "OTA %d already populated with nsec-ctf-addon firmware, skipping OTA write", NSEC_OTA_ID);
                    return;
                }
            } else {
                ESP_LOGE(TAG, "Failed to find ota_%d partition. Most likely due to wrong firmware loaded.", NSEC_OTA_ID);
                return;
            }
            ESP_LOGI(TAG, "Writing to OTA partition %d from external flash...", NSEC_OTA_ID);
            write_flash_to_ota(flash, ota_partition);
        }
    } else {
        ESP_LOGI(TAG, "CTF Addon not detected");
    }
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
    ESP_LOGI(TAG, "Pin assignments:");
    ESP_LOGI(TAG, "MOSI: %2d   MISO: %2d   SCLK: %2d   CS: %2d",
        bus_config.mosi_io_num, bus_config.miso_io_num,
        bus_config.sclk_io_num, device_config.cs_io_num
    );

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
