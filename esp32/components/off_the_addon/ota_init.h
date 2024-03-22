#pragma once

#include "driver/gpio.h"

// Off the addon PINs (used in all firmware)
#define SAO3_I01 GPIO_NUM_7
#define ADDON_BLUE_LED SAO3_I01 // blue LED on addon
#define SAO3_I02 GPIO_NUM_8
#define ADDON_DETECT SAO3_I02 // resistance to ground on addon

// CTF ADDON PINs (used in CTF ADDON firmware, defined in all to reserve them)
#define SAO1_IO1 GPIO_NUM_3
#define SAO1_IO2 GPIO_NUM_4
#define SAO2_IO1 GPIO_NUM_5
#define SAO2_IO2 GPIO_NUM_6

// Not used by the CTF ADDON
#define SAO4_IO1 GPIO_NUM_9
#define SAO4_IO2 GPIO_NUM_10

// SPI configuration for the external flash on CTF ADDON
#define HOST_ID  SPI3_HOST
#define PIN_CS   SAO1_IO1
#define PIN_CLK  SAO1_IO2
#define PIN_MOSI SAO2_IO1
#define PIN_MISO SAO2_IO2
/* Those two PINs are only used for SPI quad mode, which we don't use
#define PIN_WP // connected to ground on the addon
#define PIN_HD // user will connect to any VCC
*/
#define SPI_DMA_CHAN SPI_DMA_CH_AUTO

// OTA configuration
#define NSEC_CONF_PARTITION ESP_PARTITION_SUBTYPE_APP_FACTORY
#define NSEC_CTF_PARTITION ESP_PARTITION_SUBTYPE_APP_OTA_0
#define NSEC_OTA_PARTITION ESP_PARTITION_SUBTYPE_APP_OTA_1
#define NSEC_OTA_ID (NSEC_OTA_PARTITION - ESP_PARTITION_SUBTYPE_APP_OTA_MIN)


void ota_init(void);
esp_flash_t* init_ext_flash();

#ifdef __cplusplus
extern "C" {
#endif

void register_ota_cmd(void);

#ifdef __cplusplus
}
#endif
