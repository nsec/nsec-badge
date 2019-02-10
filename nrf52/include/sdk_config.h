#include <device/nrf52832_peripherals.h>

#define SPI_ENABLED 1
#define SPI0_ENABLED 1
#define SPI0_USE_EASY_DMA 0
#define SPI1_ENABLED 0
#define SPI2_ENABLED 1
#define SPI2_USE_EASY_DMA 0

// UART to STM32
#define UART_ENABLED 1
#define UART_EASY_DMA_SUPPORT 0
#define UART_LEGACY_SUPPORT 1

#define UART0_ENABLED 1
#define UART0_CONFIG_USE_EASY_DMA 0

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

// Defines added while trying to make the softdevice stuff compile. Maybe some are useless/misconfigured. Clean up later.
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
#define NRF_BLE_GATT_BLE_OBSERVER_PRIO 1
#define NRF_SD_BLE_API_VERSION 5
#define NRF_BLE_GATT_ENABLED 1
#define BLE_LBS_ENABLED 1
#define NRF_BLE_CONN_PARAMS_ENABLED 1
#define NRF_BLE_CONN_PARAMS_MAX_SLAVE_LATENCY_DEVIATION 499
#define NRF_BLE_CONN_PARAMS_MAX_SUPERVISION_TIMEOUT_DEVIATION 65535
#define BLE_CONN_PARAMS_BLE_OBSERVER_PRIO 1
#define BLE_CONN_STATE_BLE_OBSERVER_PRIO 0

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

#define SAADC_ENABLED 1

#define BLE_ADVERTISING_ENABLED 0 //could be useful in the future.

// <e> NRF_BLE_CONN_PARAMS_ENABLED - ble_conn_params - Initiating and executing a connection parameters negotiation procedure
#define NRF_BLE_CONN_PARAMS_ENABLED 1
// <o> NRF_BLE_CONN_PARAMS_MAX_SLAVE_LATENCY_DEVIATION - The largest acceptable deviation in slave latency.
// <i> The largest deviation (+ or -) from the requested slave latency that will not be renegotiated.
#define NRF_BLE_CONN_PARAMS_MAX_SLAVE_LATENCY_DEVIATION 499

// <o> NRF_BLE_CONN_PARAMS_MAX_SUPERVISION_TIMEOUT_DEVIATION - The largest acceptable deviation (in 10 ms units) in supervision timeout.
// <i> The largest deviation (+ or -, in 10 ms units) from the requested supervision timeout that will not be renegotiated.
#define NRF_BLE_CONN_PARAMS_MAX_SUPERVISION_TIMEOUT_DEVIATION 65535

#define NRF_BLE_GATT_ENABLED 1

// <q> NRF_BLE_QWR_ENABLED  - nrf_ble_qwr - Queued writes support module (prepare/execute write)
#define NRF_BLE_QWR_ENABLED 0

// <o> PM_MAX_REGISTRANTS - Number of event handlers that can be registered.
#ifndef PM_MAX_REGISTRANTS
#define PM_MAX_REGISTRANTS 3
#endif

// <o> PM_FLASH_BUFFERS - Number of internal buffers for flash operations.
// <i> Decrease this value to lower RAM usage.

#ifndef PM_FLASH_BUFFERS
#define PM_FLASH_BUFFERS 2
#endif

// <q> PM_CENTRAL_ENABLED  - Enable/disable central-specific Peer Manager functionality.


// <i> Enable/disable central-specific Peer Manager functionality.

#ifndef PM_CENTRAL_ENABLED
#define PM_CENTRAL_ENABLED 1
#endif


// <e> CLOCK_ENABLED - nrf_drv_clock - CLOCK peripheral driver
//==========================================================
#ifndef CLOCK_ENABLED
#define CLOCK_ENABLED 1
#endif
// <o> CLOCK_CONFIG_XTAL_FREQ  - HF XTAL Frequency

// <0=> Default (64 MHz)

#ifndef CLOCK_CONFIG_XTAL_FREQ
#define CLOCK_CONFIG_XTAL_FREQ 0
#endif

// <o> CLOCK_CONFIG_LF_SRC  - LF Clock Source

// <0=> RC
// <1=> XTAL
// <2=> Synth

#ifndef CLOCK_CONFIG_LF_SRC
#define CLOCK_CONFIG_LF_SRC 0
#endif

// <o> CLOCK_CONFIG_IRQ_PRIORITY  - Interrupt priority


// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest)
// <1=> 1
// <2=> 2
// <3=> 3
// <4=> 4
// <5=> 5
// <6=> 6
// <7=> 7

#ifndef CLOCK_CONFIG_IRQ_PRIORITY
#define CLOCK_CONFIG_IRQ_PRIORITY 7
#endif

// <o> SAADC_CONFIG_RESOLUTION  - Resolution

// <0=> 8 bit
// <1=> 10 bit
// <2=> 12 bit
// <3=> 14 bit

#ifndef SAADC_CONFIG_RESOLUTION
#define SAADC_CONFIG_RESOLUTION 1
#endif

// <o> SAADC_CONFIG_OVERSAMPLE  - Sample period

// <0=> Disabled
// <1=> 2x
// <2=> 4x
// <3=> 8x
// <4=> 16x
// <5=> 32x
// <6=> 64x
// <7=> 128x
// <8=> 256x

#ifndef SAADC_CONFIG_OVERSAMPLE
#define SAADC_CONFIG_OVERSAMPLE 0
#endif

// <q> SAADC_CONFIG_LP_MODE  - Enabling low power mode


#ifndef SAADC_CONFIG_LP_MODE
#define SAADC_CONFIG_LP_MODE 0
#endif

// <o> SAADC_CONFIG_IRQ_PRIORITY  - Interrupt priority


// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest)
// <1=> 1
// <2=> 2
// <3=> 3
// <4=> 4
// <5=> 5
// <6=> 6
// <7=> 7

#ifndef SAADC_CONFIG_IRQ_PRIORITY
#define SAADC_CONFIG_IRQ_PRIORITY 7
#endif

// <e> TIMER_ENABLED - nrf_drv_timer - TIMER periperal driver
//==========================================================
#ifndef TIMER_ENABLED
#define TIMER_ENABLED 0
#endif
// <o> TIMER_DEFAULT_CONFIG_FREQUENCY  - Timer frequency if in Timer mode

// <0=> 16 MHz
// <1=> 8 MHz
// <2=> 4 MHz
// <3=> 2 MHz
// <4=> 1 MHz
// <5=> 500 kHz
// <6=> 250 kHz
// <7=> 125 kHz
// <8=> 62.5 kHz
// <9=> 31.25 kHz

#ifndef TIMER_DEFAULT_CONFIG_FREQUENCY
#define TIMER_DEFAULT_CONFIG_FREQUENCY 0
#endif

// <o> TIMER_DEFAULT_CONFIG_MODE  - Timer mode or operation

// <0=> Timer
// <1=> Counter

#ifndef TIMER_DEFAULT_CONFIG_MODE
#define TIMER_DEFAULT_CONFIG_MODE 0
#endif

// <o> TIMER_DEFAULT_CONFIG_BIT_WIDTH  - Timer counter bit width

// <0=> 16 bit
// <1=> 8 bit
// <2=> 24 bit
// <3=> 32 bit

#ifndef TIMER_DEFAULT_CONFIG_BIT_WIDTH
#define TIMER_DEFAULT_CONFIG_BIT_WIDTH 0
#endif

// <o> TIMER_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority


// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest)
// <1=> 1
// <2=> 2
// <3=> 3
// <4=> 4
// <5=> 5
// <6=> 6
// <7=> 7

#ifndef TIMER_DEFAULT_CONFIG_IRQ_PRIORITY
#define TIMER_DEFAULT_CONFIG_IRQ_PRIORITY 7
#endif

// <o> FDS_BACKEND  - FDS flash backend.

// <i> NRF_FSTORAGE_SD uses the nrf_fstorage_sd backend implementation using the SoftDevice API. Use this if you have a SoftDevice present.
// <i> NRF_FSTORAGE_NVMC uses the nrf_fstorage_nvmc implementation. Use this setting if you don't use the SoftDevice.
// <1=> NRF_FSTORAGE_NVMC
// <2=> NRF_FSTORAGE_SD

#ifndef FDS_BACKEND
#define FDS_BACKEND 2
#endif


// <h> CRC - CRC functionality

//==========================================================
// <e> FDS_CRC_CHECK_ON_READ - Enable CRC checks.

// <i> Save a record's CRC when it is written to flash and check it when the record is opened.
// <i> Records with an incorrect CRC can still be 'seen' by the user using FDS functions, but they cannot be opened.
// <i> Additionally, they will not be garbage collected until they are deleted.
//==========================================================
#ifndef FDS_CRC_CHECK_ON_READ
#define FDS_CRC_CHECK_ON_READ 0
#endif
// <o> FDS_CRC_CHECK_ON_WRITE  - Perform a CRC check on newly written records.


// <i> Perform a CRC check on newly written records.
// <i> This setting can be used to make sure that the record data was not altered while being written to flash.
// <1=> Enabled
// <0=> Disabled

#ifndef FDS_CRC_CHECK_ON_WRITE
#define FDS_CRC_CHECK_ON_WRITE 0
#endif

// <e> HARDFAULT_HANDLER_ENABLED - hardfault_default - HardFault default handler for debugging and release
//==========================================================
#ifndef HARDFAULT_HANDLER_ENABLED
#define HARDFAULT_HANDLER_ENABLED 0
#endif
// <q> HARDFAULT_HANDLER_GDB_PSP_BACKTRACE  - Bypass the GDB problem with multiple stack pointers backtrace


// <i> There is a known bug in GDB which causes it to incorrectly backtrace the code
// <i> when multiple stack pointers are used (main and process stack pointers).
// <i> This option enables the fix for that problem and allows to see the proper backtrace info.
// <i> It makes it possible to trace the code to the exact point where a HardFault appeared.
// <i> This option requires additional commands and may temporarily switch MSP stack to store data on PSP space.
// <i> This is an optional parameter - enable it while debugging.
// <i> Before a HardFault handler exits, the stack will be reverted to its previous value.

#ifndef HARDFAULT_HANDLER_GDB_PSP_BACKTRACE
#define HARDFAULT_HANDLER_GDB_PSP_BACKTRACE 1
#endif


// <e> NRF_BALLOC_ENABLED - nrf_balloc - Block allocator module
//==========================================================
#ifndef NRF_BALLOC_ENABLED
#define NRF_BALLOC_ENABLED 1
#endif
// <e> NRF_BALLOC_CONFIG_DEBUG_ENABLED - Enables debug mode in the module.
//==========================================================
#ifndef NRF_BALLOC_CONFIG_DEBUG_ENABLED
#define NRF_BALLOC_CONFIG_DEBUG_ENABLED 0
#endif
// <o> NRF_BALLOC_CONFIG_HEAD_GUARD_WORDS - Number of words used as head guard.  <0-255>


#ifndef NRF_BALLOC_CONFIG_HEAD_GUARD_WORDS
#define NRF_BALLOC_CONFIG_HEAD_GUARD_WORDS 1
#endif

// <o> NRF_BALLOC_CONFIG_TAIL_GUARD_WORDS - Number of words used as tail guard.  <0-255>


#ifndef NRF_BALLOC_CONFIG_TAIL_GUARD_WORDS
#define NRF_BALLOC_CONFIG_TAIL_GUARD_WORDS 1
#endif

// <q> NRF_BALLOC_CONFIG_BASIC_CHECKS_ENABLED  - Enables basic checks in this module.


#ifndef NRF_BALLOC_CONFIG_BASIC_CHECKS_ENABLED
#define NRF_BALLOC_CONFIG_BASIC_CHECKS_ENABLED 0
#endif

// <q> NRF_BALLOC_CONFIG_DOUBLE_FREE_CHECK_ENABLED  - Enables double memory free check in this module.


#ifndef NRF_BALLOC_CONFIG_DOUBLE_FREE_CHECK_ENABLED
#define NRF_BALLOC_CONFIG_DOUBLE_FREE_CHECK_ENABLED 0
#endif

// <q> NRF_BALLOC_CONFIG_DATA_TRASHING_CHECK_ENABLED  - Enables free memory corruption check in this module.


#ifndef NRF_BALLOC_CONFIG_DATA_TRASHING_CHECK_ENABLED
#define NRF_BALLOC_CONFIG_DATA_TRASHING_CHECK_ENABLED 0
#endif


// <q> NRF_FPRINTF_ENABLED  - nrf_fprintf - fprintf function.


#ifndef NRF_FPRINTF_ENABLED
#define NRF_FPRINTF_ENABLED 1
#endif

// <h> nrf_fstorage_sd - Implementation using the SoftDevice.

// <i> Configuration options for the fstorage implementation using the SoftDevice.
//==========================================================
// <o> NRF_FSTORAGE_SD_QUEUE_SIZE - Size of the internal queue of operations.
// <i> Increase this value if API calls frequently return the error @ref NRF_ERROR_NO_MEM.

#ifndef NRF_FSTORAGE_SD_QUEUE_SIZE
#define NRF_FSTORAGE_SD_QUEUE_SIZE 4
#endif

// <o> NRF_FSTORAGE_SD_MAX_RETRIES - Maximum number of attempts at executing an operation when the SoftDevice is busy.
// <i> Increase this value if events frequently return the @ref NRF_ERROR_TIMEOUT error.
// <i> The SoftDevice might fail to schedule flash access due to high BLE activity.

#ifndef NRF_FSTORAGE_SD_MAX_RETRIES
#define NRF_FSTORAGE_SD_MAX_RETRIES 8
#endif

// <o> NRF_FSTORAGE_SD_MAX_WRITE_SIZE - Maximum number of bytes to be written to flash in a single operation.
// <i> This value must be a multiple of four.
// <i> Lowering this value can increase the chances of the SoftDevice being able to execute flash operations in between radio activity.
// <i> This value is bound by the maximum number of bytes that can be written to flash in a single call to @ref sd_flash_write.
// <i> That is 1024 bytes for nRF51 ICs and 4096 bytes for nRF52 ICs.

#ifndef NRF_FSTORAGE_SD_MAX_WRITE_SIZE
#define NRF_FSTORAGE_SD_MAX_WRITE_SIZE 4096
#endif

// </h>
//==========================================================

// </e>

// <q> NRF_MEMOBJ_ENABLED  - nrf_memobj - Linked memory allocator module


#ifndef NRF_MEMOBJ_ENABLED
#define NRF_MEMOBJ_ENABLED 1
#endif

// <q> NRF_QUEUE_ENABLED  - nrf_queue - Queue module


#ifndef NRF_QUEUE_ENABLED
#define NRF_QUEUE_ENABLED 0
#endif

// <q> NRF_STRERROR_ENABLED  - nrf_strerror - Library for converting error code to string.


#ifndef NRF_STRERROR_ENABLED
#define NRF_STRERROR_ENABLED 1
#endif


// <e> COMMON_CONFIG_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef COMMON_CONFIG_LOG_ENABLED
#define COMMON_CONFIG_LOG_ENABLED 0
#endif
// <o> COMMON_CONFIG_LOG_LEVEL  - Default Severity level

// <0=> Off
// <1=> Error
// <2=> Warning
// <3=> Info
// <4=> Debug

#ifndef COMMON_CONFIG_LOG_LEVEL
#define COMMON_CONFIG_LOG_LEVEL 3
#endif

// <o> COMMON_CONFIG_INFO_COLOR  - ANSI escape code prefix.

// <0=> Default
// <1=> Black
// <2=> Red
// <3=> Green
// <4=> Yellow
// <5=> Blue
// <6=> Magenta
// <7=> Cyan
// <8=> White

#ifndef COMMON_CONFIG_INFO_COLOR
#define COMMON_CONFIG_INFO_COLOR 0
#endif

// <o> COMMON_CONFIG_DEBUG_COLOR  - ANSI escape code prefix.

// <0=> Default
// <1=> Black
// <2=> Red
// <3=> Green
// <4=> Yellow
// <5=> Blue
// <6=> Magenta
// <7=> Cyan
// <8=> White

#ifndef COMMON_CONFIG_DEBUG_COLOR
#define COMMON_CONFIG_DEBUG_COLOR 0
#endif

// <e> SAADC_CONFIG_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef SAADC_CONFIG_LOG_ENABLED
#define SAADC_CONFIG_LOG_ENABLED 0
#endif
// <o> SAADC_CONFIG_LOG_LEVEL  - Default Severity level

// <0=> Off
// <1=> Error
// <2=> Warning
// <3=> Info
// <4=> Debug

#ifndef SAADC_CONFIG_LOG_LEVEL
#define SAADC_CONFIG_LOG_LEVEL 3
#endif

// <o> SAADC_CONFIG_INFO_COLOR  - ANSI escape code prefix.

// <0=> Default
// <1=> Black
// <2=> Red
// <3=> Green
// <4=> Yellow
// <5=> Blue
// <6=> Magenta
// <7=> Cyan
// <8=> White

#ifndef SAADC_CONFIG_INFO_COLOR
#define SAADC_CONFIG_INFO_COLOR 0
#endif

// <o> SAADC_CONFIG_DEBUG_COLOR  - ANSI escape code prefix.

// <0=> Default
// <1=> Black
// <2=> Red
// <3=> Green
// <4=> Yellow
// <5=> Blue
// <6=> Magenta
// <7=> Cyan
// <8=> White

#ifndef SAADC_CONFIG_DEBUG_COLOR
#define SAADC_CONFIG_DEBUG_COLOR 0
#endif

// <e> NRF_SDH_BLE_LOG_ENABLED - Enable logging in SoftDevice handler (BLE) module.
//==========================================================
#ifndef NRF_SDH_BLE_LOG_ENABLED
#define NRF_SDH_BLE_LOG_ENABLED 1
#endif
// <o> NRF_SDH_BLE_LOG_LEVEL  - Default Severity level

// <0=> Off
// <1=> Error
// <2=> Warning
// <3=> Info
// <4=> Debug

#ifndef NRF_SDH_BLE_LOG_LEVEL
#define NRF_SDH_BLE_LOG_LEVEL 3
#endif

// <o> NRF_SDH_BLE_INFO_COLOR  - ANSI escape code prefix.

// <0=> Default
// <1=> Black
// <2=> Red
// <3=> Green
// <4=> Yellow
// <5=> Blue
// <6=> Magenta
// <7=> Cyan
// <8=> White

#ifndef NRF_SDH_BLE_INFO_COLOR
#define NRF_SDH_BLE_INFO_COLOR 0
#endif

// <o> NRF_SDH_BLE_DEBUG_COLOR  - ANSI escape code prefix.

// <0=> Default
// <1=> Black
// <2=> Red
// <3=> Green
// <4=> Yellow
// <5=> Blue
// <6=> Magenta
// <7=> Cyan
// <8=> White

#ifndef NRF_SDH_BLE_DEBUG_COLOR
#define NRF_SDH_BLE_DEBUG_COLOR 0
#endif

// </e>

// <e> NRF_SDH_LOG_ENABLED - Enable logging in SoftDevice handler module.
//==========================================================
#ifndef NRF_SDH_LOG_ENABLED
#define NRF_SDH_LOG_ENABLED 1
#endif
// <o> NRF_SDH_LOG_LEVEL  - Default Severity level

// <0=> Off
// <1=> Error
// <2=> Warning
// <3=> Info
// <4=> Debug

#ifndef NRF_SDH_LOG_LEVEL
#define NRF_SDH_LOG_LEVEL 3
#endif

// <o> NRF_SDH_INFO_COLOR  - ANSI escape code prefix.

// <0=> Default
// <1=> Black
// <2=> Red
// <3=> Green
// <4=> Yellow
// <5=> Blue
// <6=> Magenta
// <7=> Cyan
// <8=> White

#ifndef NRF_SDH_INFO_COLOR
#define NRF_SDH_INFO_COLOR 0
#endif

// <o> NRF_SDH_DEBUG_COLOR  - ANSI escape code prefix.

// <0=> Default
// <1=> Black
// <2=> Red
// <3=> Green
// <4=> Yellow
// <5=> Blue
// <6=> Magenta
// <7=> Cyan
// <8=> White

#ifndef NRF_SDH_DEBUG_COLOR
#define NRF_SDH_DEBUG_COLOR 0
#endif

// </e>

// <e> NRF_SDH_SOC_LOG_ENABLED - Enable logging in SoftDevice handler (SoC) module.
//==========================================================
#ifndef NRF_SDH_SOC_LOG_ENABLED
#define NRF_SDH_SOC_LOG_ENABLED 1
#endif
// <o> NRF_SDH_SOC_LOG_LEVEL  - Default Severity level

// <0=> Off
// <1=> Error
// <2=> Warning
// <3=> Info
// <4=> Debug

#ifndef NRF_SDH_SOC_LOG_LEVEL
#define NRF_SDH_SOC_LOG_LEVEL 3
#endif

// <o> NRF_SDH_SOC_INFO_COLOR  - ANSI escape code prefix.

// <0=> Default
// <1=> Black
// <2=> Red
// <3=> Green
// <4=> Yellow
// <5=> Blue
// <6=> Magenta
// <7=> Cyan
// <8=> White

#ifndef NRF_SDH_SOC_INFO_COLOR
#define NRF_SDH_SOC_INFO_COLOR 0
#endif

// <o> NRF_SDH_SOC_DEBUG_COLOR  - ANSI escape code prefix.

// <0=> Default
// <1=> Black
// <2=> Red
// <3=> Green
// <4=> Yellow
// <5=> Blue
// <6=> Magenta
// <7=> Cyan
// <8=> White

#ifndef NRF_SDH_SOC_DEBUG_COLOR
#define NRF_SDH_SOC_DEBUG_COLOR 0
#endif

// <o> NRF_SDH_BLE_TOTAL_LINK_COUNT - Maximum number of total concurrent connections using the default configuration.
#ifndef NRF_SDH_BLE_TOTAL_LINK_COUNT
#define NRF_SDH_BLE_TOTAL_LINK_COUNT 1
#endif

// <o> NRF_SDH_BLE_GAP_EVENT_LENGTH - The time set aside for this connection on every connection interval in 1.25 ms units.
#ifndef NRF_SDH_BLE_GAP_EVENT_LENGTH
#define NRF_SDH_BLE_GAP_EVENT_LENGTH 3
#endif

// <o> BLE_BAS_BLE_OBSERVER_PRIO
// <i> Priority with which BLE events are dispatched to the Battery Service.

#ifndef BLE_BAS_BLE_OBSERVER_PRIO
#define BLE_BAS_BLE_OBSERVER_PRIO 2
#endif

// <o> BLE_BAS_C_BLE_OBSERVER_PRIO
// <i> Priority with which BLE events are dispatched to the Battery Service Client.

#ifndef BLE_BAS_C_BLE_OBSERVER_PRIO
#define BLE_BAS_C_BLE_OBSERVER_PRIO 2
#endif

// <o> BLE_TPS_BLE_OBSERVER_PRIO
// <i> Priority with which BLE events are dispatched to the TX Power Service.

#ifndef BLE_TPS_BLE_OBSERVER_PRIO
#define BLE_TPS_BLE_OBSERVER_PRIO 2
#endif



// <o> PM_BLE_OBSERVER_PRIO - Priority with which BLE events are dispatched to the Peer Manager module.
#ifndef PM_BLE_OBSERVER_PRIO
#define PM_BLE_OBSERVER_PRIO 1
#endif

// <h> Dispatch model
// <i> This setting configures how Stack events are dispatched to the application.
// <o> NRF_SDH_DISPATCH_MODEL

// <i> NRF_SDH_DISPATCH_MODEL_INTERRUPT: SoftDevice events are passed to the application from the interrupt context.
// <i> NRF_SDH_DISPATCH_MODEL_APPSH: SoftDevice events are scheduled using @ref app_scheduler.
// <i> NRF_SDH_DISPATCH_MODEL_POLLING: SoftDevice events are to be fetched manually.
// <0=> NRF_SDH_DISPATCH_MODEL_INTERRUPT
// <1=> NRF_SDH_DISPATCH_MODEL_APPSH
// <2=> NRF_SDH_DISPATCH_MODEL_POLLING

#ifndef NRF_SDH_DISPATCH_MODEL
#define NRF_SDH_DISPATCH_MODEL 0
#endif

// </h>
//==========================================================

// <h> SDH Observers - Observers and priority levels

//==========================================================
// <o> NRF_SDH_REQ_OBSERVER_PRIO_LEVELS - Total number of priority levels for request observers.
// <i> This setting configures the number of priority levels available for the SoftDevice request event handlers.
// <i> The priority level of a handler determines the order in which it receives events, with respect to other handlers.

#ifndef NRF_SDH_REQ_OBSERVER_PRIO_LEVELS
#define NRF_SDH_REQ_OBSERVER_PRIO_LEVELS 2
#endif

// <o> NRF_SDH_STATE_OBSERVER_PRIO_LEVELS - Total number of priority levels for state observers.
// <i> This setting configures the number of priority levels available for the SoftDevice state event handlers.
// <i> The priority level of a handler determines the order in which it receives events, with respect to other handlers.

#ifndef NRF_SDH_STATE_OBSERVER_PRIO_LEVELS
#define NRF_SDH_STATE_OBSERVER_PRIO_LEVELS 2
#endif

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
