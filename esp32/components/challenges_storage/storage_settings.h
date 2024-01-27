/*  Original pins used for ESP32_WROOM
    #define HOST_ID  SPI3_HOST
    #define PIN_MOSI SPI3_IOMUX_PIN_NUM_MOSI
    #define PIN_MISO SPI3_IOMUX_PIN_NUM_MISO
    #define PIN_CLK  SPI3_IOMUX_PIN_NUM_CLK
    #define PIN_CS   SPI3_IOMUX_PIN_NUM_CS
    #define PIN_CS2  GPIO_NUM_25
    #define PIN_WP   SPI3_IOMUX_PIN_NUM_WP
    #define PIN_HD   SPI3_IOMUX_PIN_NUM_HD
    #define SPI_DMA_CHAN SPI_DMA_CH_AUTO
*/

#include "driver/gpio.h"

#define SAO1_IO1 GPIO_NUM_15 // white
#define SAO1_IO2 GPIO_NUM_4 // yellow
#define SAO2_IO1 GPIO_NUM_5 // blue
#define SAO2_IO2 GPIO_NUM_6 // green


#define HOST_ID  SPI3_HOST
#define PIN_CS   SAO1_IO1 // white
#define PIN_CLK  SAO1_IO2 // yellow
#define PIN_MOSI SAO2_IO1 // blue
#define PIN_MISO SAO2_IO2 // green
#define PIN_WP   7  // NC on SAO3 TODO maybe not use during init?
#define PIN_HD   12 // NC on SAO3
#define SPI_DMA_CHAN SPI_DMA_CH_AUTO
