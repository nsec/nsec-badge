#pragma once

#include "driver/gpio.h"

#define SAO1_IO1 GPIO_NUM_15 // white
#define SAO1_IO2 GPIO_NUM_4 // yellow

#define SAO2_IO1 GPIO_NUM_5 // blue
#define SAO2_IO2 GPIO_NUM_6 // green

#define SAO3_I01 GPIO_NUM_7
#define ADDON_BLUE_LED SAO3_I01 // blue LED on addon

#define SAO3_I02 GPIO_NUM_12
#define ADDON_DETECT SAO3_I02 // resistance to ground on addon

#define SAO4_IO1 GPIO_NUM_17
#define SAO4_IO2 GPIO_NUM_18

#define HOST_ID  SPI3_HOST
#define PIN_CS   SAO1_IO1 // white
#define PIN_CLK  SAO1_IO2 // yellow
#define PIN_MOSI SAO2_IO1 // blue
#define PIN_MISO SAO2_IO2 // green
/* Those two PINs are only used for SPI quad mode, which we don't use
#define PIN_WP // connected to ground on the addon
#define PIN_HD // user will connect to any VCC
*/
#define SPI_DMA_CHAN SPI_DMA_CH_AUTO

#define NSEC_CONF_PARTITION ESP_PARTITION_SUBTYPE_APP_FACTORY

#define NSEC_CTF_PARTITION ESP_PARTITION_SUBTYPE_APP_OTA_0

#define NSEC_OTA_PARTITION ESP_PARTITION_SUBTYPE_APP_OTA_1
#define NSEC_OTA_ID (NSEC_OTA_PARTITION - ESP_PARTITION_SUBTYPE_APP_OTA_MIN)


void ota_init(void);


#ifdef __cplusplus
extern "C" {
#endif

void register_ota_cmd(void);

#ifdef __cplusplus
}
#endif
