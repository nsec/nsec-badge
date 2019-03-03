#include <device/nrf52832_peripherals.h>

// SPI

#define SPI_ENABLED 1

/// SPI0 for flash

#define SPI0_ENABLED 1
#define SPI0_USE_EASY_DMA 0

/// SPI1 unused
#define SPI1_ENABLED 0

/// SPI2 for display/ssd1306

#define SPI2_ENABLED 1
#define SPI2_USE_EASY_DMA 0

// UART to STM32
#define UART_ENABLED 1
#define UART_EASY_DMA_SUPPORT 0
#define UART_LEGACY_SUPPORT 1

#define UART_DEFAULT_CONFIG_HWFC NRF_UART_HWFC_DISABLED
#define UART_DEFAULT_CONFIG_PARITY NRF_UART_PARITY_EXCLUDED
#define UART_DEFAULT_CONFIG_BAUDRATE NRF_UART_BAUDRATE_115200
#define UART_DEFAULT_CONFIG_IRQ_PRIORITY APP_IRQ_PRIORITY_LOW

#define UART0_ENABLED 1
#define UART0_CONFIG_USE_EASY_DMA 0

// CLI
#define NRF_CLI_ENABLED 1
#define NRF_CLI_UART_ENABLED 1
#define NRF_CLI_BUILD_IN_CMDS_ENABLED 1
#define NRF_CLI_ARGC_MAX 6
#define NRF_CLI_ECHO_STATUS 1
#define NRF_CLI_CMD_BUFF_SIZE 64
#define NRF_CLI_PRINTF_BUFF_SIZE 64
#define NRF_CLI_UART_CONFIG_INFO_COLOR 0
#define NRF_CLI_UART_CONFIG_DEBUG_COLOR 0
#define NRF_CLI_UART_CONFIG_LOG_ENABLED 0
#define NRF_CLI_UART_CONFIG_LOG_LEVEL 0
#define NRF_CLI_VT100_COLORS_ENABLED 1
#define NRF_CLI_HISTORY_ENABLED 1
#define NRF_CLI_HISTORY_ELEMENT_SIZE 64
#define NRF_CLI_HISTORY_ELEMENT_COUNT 16

#define NRF_QUEUE_ENABLED 1

#define CRC32_ENABLED 1

// PWM
#define PWM_ENABLED 1
#define PWM2_ENABLED 1

// Enable the power driver
#define POWER_ENABLED 1

// DCDC regulator is installed and can be enabled
#define POWER_CONFIG_DEFAULT_DCDCEN 1

// 0-7, 0,1,4,5 are reserved for SoftDevice
#define POWER_CONFIG_IRQ_PRIORITY 7
#define POWER_CONFIG_SOC_OBSERVER_PRIO 0
#define POWER_CONFIG_STATE_OBSERVER_PRIO 0
#define NRF_SPI_DRV_MISO_PULLUP_CFG 0 //0 -> no pull, may need to be changed.
#define APP_SCHEDULER_ENABLED 1
#define APP_TIMER_ENABLED 1
#define APP_GPIOTE_ENABLED 1
#define GPIOTE_ENABLED 1
#define GPIOTE_CONFIG_IRQ_PRIORITY 3
#define GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS 4
#define BLE_DIS_ENABLED 1
#define PEER_MANAGER_ENABLED 1
#define FDS_ENABLED 1
#define FDS_OP_QUEUE_SIZE 4
#define FDS_CHUNK_QUEUE_SIZE 8
#define FDS_MAX_USERS 8
#define FDS_VIRTUAL_PAGES 3
#define FDS_VIRTUAL_PAGE_SIZE 1024
#define FSTORAGE_ENABLED 1
#define FS_QUEUE_SIZE 4
#define FS_OP_MAX_RETRIES 3
#define FS_MAX_WRITE_SIZE_WORDS 256
#define NRF_FSTORAGE_ENABLED 1
#define APP_TIMER_CONFIG_RTC_FREQUENCY 0
#define APP_TIMER_CONFIG_OP_QUEUE_SIZE 16
#define APP_TIMER_CONFIG_IRQ_PRIORITY 2  // highest priority not reserved for SoftDevice.

// BUTTON_ENABLED  - app_button - buttons handling module
#define BUTTON_ENABLED 1

//---------Soft device defines---------
#define NRF_SDH_STATE_OBSERVER_PRIO_LEVELS 2 //Total number of priority levels for state observers.
#define NRF_SDH_REQ_OBSERVER_PRIO_LEVELS 2 //Total number of priority levels for request observers.
#define NRF_SDH_ENABLED 1
#define NRF_SDH_BLE_ENABLED 1
#define NRF_SDH_CLOCK_LF_SRC 0 // NRF_CLOCK_LF_SRC_RC
#define NRF_SDH_CLOCK_LF_RC_CTIV 16
#define NRF_SDH_CLOCK_LF_RC_TEMP_CTIV 2
#define NRF_SDH_CLOCK_LF_XTAL_ACCURACY 1 //not used for NRF_CLOCK_LF_SRC_RC
#define CLOCK_CONFIG_SOC_OBSERVER_PRIO 0
#define CLOCK_CONFIG_STATE_OBSERVER_PRIO 0
#define NRF_SDH_SOC_ENABLED 1
#define NRF_SDH_SOC_OBSERVER_PRIO_LEVELS 4
#define NRF_SDH_SOC_STACK_OBSERVER_PRIO 0
#define NRF_SDH_STACK_OBSERVER_PRIO_LEVELS 2
#define NRF_SDH_BLE_STACK_OBSERVER_PRIO 0
#define NRF_SDH_BLE_VS_UUID_COUNT 1
#define NRF_SDH_BLE_GATTS_ATTR_TAB_SIZE 1408
#define NRF_SDH_BLE_SERVICE_CHANGED 0
#define NRF_SDH_BLE_GATT_MAX_MTU_SIZE 23
#define NRF_SECTION_ITER_ENABLED 1
#define BLE_LBS_BLE_OBSERVER_PRIO 2
#define NRF_SDH_BLE_OBSERVER_PRIO_LEVELS 4
#define NRF_SDH_BLE_PERIPHERAL_LINK_COUNT 1
#define NRF_SDH_BLE_CENTRAL_LINK_COUNT 1
#define NRF_SDH_BLE_TOTAL_LINK_COUNT 1 //Maximum number of total concurrent connections using the default configuration.
#define NRF_SDH_BLE_GAP_EVENT_LENGTH 3 //The time set aside for this connection on every connection interval in 1.25 ms units.

// Defines related to logging
#define NRF_LOG_ENABLED 1
#define NRF_CLI_LOG_BACKEND 1
#define NRF_LOG_BACKEND_RTT_ENABLED 1
#define NRF_LOG_BACKEND_RTT_TEMP_BUFFER_SIZE 64
#define SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS 2
#define SEGGER_RTT_CONFIG_MAX_NUM_DOWN_BUFFERS 2
#define SEGGER_RTT_CONFIG_BUFFER_SIZE_DOWN 16
#define SEGGER_RTT_CONFIG_BUFFER_SIZE_UP 512
#define SEGGER_RTT_CONFIG_DEFAULT_MODE 0
#define NRF_LOG_DEFAULT_LEVEL 4 //debug
#define NRF_LOG_BUFSIZE 1024
#define NRF_LOG_DEFERRED 0
#define NRF_LOG_ALLOW_OVERFLOW 1
#define NRF_LOG_MSGPOOL_ELEMENT_COUNT 8
#define NRF_LOG_MSGPOOL_ELEMENT_SIZE 20
#define NRF_LOG_USES_COLORS 1
#define NRF_LOG_COLOR_DEFAULT 0
#define NRF_LOG_ERROR_COLOR 2
#define NRF_LOG_WARNING_COLOR 4
#define NRF_STRERROR_ENABLED 1  //Library for converting error code to string.
#define NRF_BALLOC_ENABLED 1 //Required for logging
#define NRF_FPRINTF_ENABLED 1

// Defines related to module logging
#define COMMON_CONFIG_LOG_ENABLED 0  //Enables logging in the module.
#define COMMON_CONFIG_LOG_LEVEL 3  //Default Severity level 0-Off 1-Error 2-Warning 3-Info 4-Debug
#define SAADC_CONFIG_LOG_ENABLED 0  //Enables logging in the SAADC module.
#define SAADC_CONFIG_LOG_LEVEL 3
#define NRF_SDH_BLE_LOG_ENABLED 0  //Enable logging in SoftDevice handler (BLE) module.
#define NRF_SDH_BLE_LOG_LEVEL 3
#define NRF_SDH_LOG_ENABLED 0  //Enable logging in SoftDevice handler module.
#define NRF_SDH_LOG_LEVEL 3
#define NRF_SDH_SOC_LOG_ENABLED 0  //Enable logging in SoftDevice handler (SoC) module.
#define NRF_SDH_SOC_LOG_LEVEL 3

// BLE related defines
#define NRF_BLE_GATT_BLE_OBSERVER_PRIO 1
#define NRF_SD_BLE_API_VERSION 5
#define NRF_BLE_GATT_ENABLED 1
//Initiating and executing a connection parameters negotiation procedure
#define NRF_BLE_CONN_PARAMS_ENABLED 1
//The largest deviation (+ or -) from the requested slave latency that will not be renegotiated.
#define NRF_BLE_CONN_PARAMS_MAX_SLAVE_LATENCY_DEVIATION 499
//The largest deviation (+ or -, in 10 ms units) from the requested supervision timeout that will not be renegotiated.
#define NRF_BLE_CONN_PARAMS_MAX_SUPERVISION_TIMEOUT_DEVIATION 65535
#define BLE_CONN_PARAMS_BLE_OBSERVER_PRIO 1
#define BLE_CONN_STATE_BLE_OBSERVER_PRIO 0
#define BLE_ADVERTISING_ENABLED 0 //Will be used in the future.
#define NRF_BLE_QWR_ENABLED 0  //Will be used in the future: Queued writes support module (prepare/execute write)
#define PM_MAX_REGISTRANTS 3  //Number of event handlers that can be registered.
#define PM_FLASH_BUFFERS 2  //Number of internal buffers for flash operations.
#define PM_CENTRAL_ENABLED 1  //Enable/disable central-specific Peer Manager functionality.
#define PM_BLE_OBSERVER_PRIO 1  //Priority with which BLE events are dispatched to the Peer Manager module.

// SAADC defines
#define SAADC_ENABLED 1
#define SAADC_CONFIG_RESOLUTION 1 // 0=> 8 bit 1=> 10 bit 2=> 12 bit 3=> 14 bit
#define SAADC_CONFIG_OVERSAMPLE 0  //Sample period 0: Disabled  1: 2x  2: 4x  3: 8x  4: 16x  5: 32x  6: 64x  7: 128x  8: 256x
#define SAADC_CONFIG_LP_MODE 0  //Enabling low power mode
#define SAADC_CONFIG_IRQ_PRIORITY 7  //Interrupt priority (0-7). On nrf52, 0,1,4,5 are reserved for SoftDevice

//CLOCK peripheral driver
#define CLOCK_ENABLED 1
#define CLOCK_CONFIG_XTAL_FREQ 0  //HF XTAL Frequency (0-> Default 64 MHz)
#define CLOCK_CONFIG_LF_SRC 0  //LF Clock Source (0 RC, 1 XTAL, 2 Synth)
#define CLOCK_CONFIG_IRQ_PRIORITY 7  //Interrupt priority (0(highest)-7). On nrf52, 0,1,4,5 are reserved for SoftDevice

//FDS flash backend.
#define FDS_BACKEND 2 //NRF_FSTORAGE_SD (use softdevice to use the flash storage)
#define NRF_FSTORAGE_SD_QUEUE_SIZE 4  //Size of the internal queue of operations.
#define NRF_FSTORAGE_SD_MAX_RETRIES 8 //Maximum number of attempts at executing an operation when the SoftDevice is busy.
//Maximum number of bytes to be written to flash in a single operation. Max is 4096 bytes.
#define NRF_FSTORAGE_SD_MAX_WRITE_SIZE 4096


// <e> TWI_ENABLED - nrf_drv_twi - TWI/TWIM peripheral driver
//==========================================================
#ifndef TWI_ENABLED
#define TWI_ENABLED 1
#endif
// <o> TWI_DEFAULT_CONFIG_FREQUENCY  - Frequency

// <26738688=> 100k
// <67108864=> 250k
// <104857600=> 400k

#ifndef TWI_DEFAULT_CONFIG_FREQUENCY
#define TWI_DEFAULT_CONFIG_FREQUENCY 26738688
#endif

// <q> TWI_DEFAULT_CONFIG_CLR_BUS_INIT  - Enables bus clearing procedure during init


#ifndef TWI_DEFAULT_CONFIG_CLR_BUS_INIT
#define TWI_DEFAULT_CONFIG_CLR_BUS_INIT 0
#endif

// <q> TWI_DEFAULT_CONFIG_HOLD_BUS_UNINIT  - Enables bus holding after uninit


#ifndef TWI_DEFAULT_CONFIG_HOLD_BUS_UNINIT
#define TWI_DEFAULT_CONFIG_HOLD_BUS_UNINIT 0
#endif

// <o> TWI_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority


// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest)
// <1=> 1
// <2=> 2
// <3=> 3
// <4=> 4
// <5=> 5
// <6=> 6
// <7=> 7

#ifndef TWI_DEFAULT_CONFIG_IRQ_PRIORITY
#define TWI_DEFAULT_CONFIG_IRQ_PRIORITY 7
#endif

// <e> TWI0_ENABLED - Enable TWI0 instance
//==========================================================
#ifndef TWI0_ENABLED
#define TWI0_ENABLED 0

#endif
// <q> TWI0_USE_EASY_DMA  - Use EasyDMA (if present)


#ifndef TWI0_USE_EASY_DMA
#define TWI0_USE_EASY_DMA 0
#endif

// </e>

// <e> TWI1_ENABLED - Enable TWI1 instance
//==========================================================
#ifndef TWI1_ENABLED
#define TWI1_ENABLED 1
#endif
// <q> TWI1_USE_EASY_DMA  - Use EasyDMA (if present)


#ifndef TWI1_USE_EASY_DMA
#define TWI1_USE_EASY_DMA 0
#endif

// </e>

// <q> TWIM_NRF52_ANOMALY_109_WORKAROUND_ENABLED  - Enables nRF52 anomaly 109 workaround for TWIM.


// <i> The workaround uses interrupts to wake up the CPU by catching
// <i> the start event of zero-frequency transmission, clear the
// <i> peripheral, set desired frequency, start the peripheral, and
// <i> the proper transmission. See more in the Errata document or
// <i> Anomaly 109 Addendum located at https://infocenter.nordicsemi.com/

#ifndef TWIM_NRF52_ANOMALY_109_WORKAROUND_ENABLED
#define TWIM_NRF52_ANOMALY_109_WORKAROUND_ENABLED 0
#endif
