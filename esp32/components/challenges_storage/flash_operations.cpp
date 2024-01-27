#include <string.h>
#include "esp_flash.h"
#include "esp_flash_spi_init.h"
#include "driver/spi_common.h"
#include "soc/spi_pins.h"
#include "esp_log.h"
#include "storage_settings.h"
#include "save.h"

static const char *TAG = "flash_operations";

spi_bus_config_t init_spi_bus(void) {
    const spi_bus_config_t bus_config = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = PIN_WP,
        .quadhd_io_num = PIN_HD,
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

void flash_read(esp_flash_t* _flash, uint16_t size) {
    if (size < 1) size = 128;
    uint8_t* buffer = (uint8_t*)malloc(size);
    if (buffer == NULL) {
        ESP_LOGI(TAG, "Failed to allocate memory for buffer!");
        return;
    }
    ESP_ERROR_CHECK(esp_flash_read(_flash, buffer, 0x0, size));
    for (int i = 0; i < size; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
    fflush(stdout);
    free(buffer);
}

void flash_read_at(esp_flash_t* _flash, uint16_t custom_param, unsigned int address) {
    if (custom_param < 1) custom_param = 128;

    const int READSIZE = custom_param;
    uint8_t* buffer = (uint8_t*)malloc(READSIZE);
    if (buffer == NULL) {
        ESP_LOGI(TAG, "Failed to allocate memory for buffer!");
        return;
    }
    ESP_ERROR_CHECK(esp_flash_read(_flash, buffer, address, READSIZE));
    for (int i = 0; i < READSIZE; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
    fflush(stdout);
    free(buffer);
}

void flash_write_flag(esp_flash_t* _flash, unsigned int address) {
    const char* buffer = "FLAG-CTFFLASH24";
    ESP_ERROR_CHECK(esp_flash_write(_flash, buffer, address, strlen(buffer)));
}

