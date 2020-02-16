#include <nrf_delay.h>
#include <nrf_gpio.h>
#include <nrf_sdh.h>
#include <sdk_errors.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include "boards.h"
#include "cli.h"
#include "drivers/ws2812fx.h"

//static void init_ble() {
//    create_ble_device(g_device_id);
//#ifdef NSEC_FLAVOR_BAR_BEACON
//    init_resistance_bar_beacon();
//    set_advertiser(get_resistance_bar_beacon());
//    ble_start_advertising();
//#else
//    init_adv_module();
//    enable_fast_advertising_mode(60);
//    enable_slow_advertising_mode(0); // No timeout
//    set_device_name(g_device_id);
//    /*nsec_led_ble_init();*/
//    init_identity_service();
//    init_button_service();
//    nsec_ble_init_device_information_service();
//    set_vendor_service_in_advertising_packet(nsec_identity_get_service(), false);
//    //set_vendor_service_in_scan_response(nsec_identity_get_service(), true);
//    set_advertiser(get_service_advertiser());
//
//    add_observer(get_resistance_propaganda_observer());
//    if (get_stored_ble_is_enabled()) {
//        ble_start_advertising();
//        ble_device_start_scan();
//    }
//    nsec_nearby_badges_init();
//#endif
//}

static void led_toggle_task(void *params)
{

    while (1) {
        nrf_gpio_pin_set(PIN_LED_STATUS_2);
        vTaskDelay(1000);
        nrf_gpio_pin_clear(PIN_LED_STATUS_2);
        vTaskDelay(1000);
    }
}

/* Task to call cli_process regularly. */

static void cli_task(void *params)
{
    while (1) {
        cli_process();
        vTaskDelay(100);
    }
}

static void neopixels_task(void *params)
{
    while (1) {
        service_WS2812FX();
        vTaskDelay(1);
    }
}

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    while (1) {
        nrf_gpio_pin_set(PIN_LED_STATUS_1);
        nrf_delay_ms(100);
        nrf_gpio_pin_clear(PIN_LED_STATUS_1);
        nrf_delay_ms(100);
    }

    NVIC_SystemReset();
}

static void led_toggle_timer_callback(void *params)
{
    nrf_gpio_pin_toggle(PIN_LED_STATUS_1);
}

static TaskHandle_t cli_task_handle;
static TaskHandle_t neopixels_task_handle;
static TaskHandle_t led_toggle_task_handle;
static TimerHandle_t led_toggle_timer_handle;

/* This is called by FreeRTOS if a pvPortMalloc call fails. */
extern void vApplicationMallocFailedHook(void);
void vApplicationMallocFailedHook(void)
{
    app_error_fault_handler(0, 0, 0);
}

/* This is called by FreeRTOS if it detects that a task overflows its stack. */
extern void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    app_error_fault_handler(0, 0, 0);
}

int main(void)
{
    ret_code_t ret_code;
    BaseType_t ret;
    nrfx_err_t nrfx_err;

    nrf_gpio_cfg_output(PIN_LED_STATUS_1);
    nrf_gpio_cfg_output(PIN_LED_STATUS_2);

    nrf_gpio_pin_set(PIN_LED_STATUS_1);
    nrf_gpio_pin_clear(PIN_LED_STATUS_2);

    // Initializing the SoftDevice also initializes the low-frequency clock,
    // which is used as a tick source for FreeRTOS.
    ret_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(ret_code);

    ret_code = sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
    APP_ERROR_CHECK(ret_code);

    // Initialize the CLI.
    cli_init();

    // Initialize the NeoPixel led controller.
    init_WS2812FX();

    setBrightness_WS2812FX(64);
    setSpeed_WS2812FX(40);
    setColor_WS2812FX(255, 0, 0);
    setMode_WS2812FX(FX_MODE_CHASE_COLOR);
    start_WS2812FX();

    ret = xTaskCreate(led_toggle_task, "LED", configMINIMAL_STACK_SIZE + 200,
                      NULL, 2, &led_toggle_task_handle);
    APP_ERROR_CHECK_BOOL(ret == pdPASS);

    ret = xTaskCreate(cli_task, "CLI", configMINIMAL_STACK_SIZE + 500, NULL, 2,
                      &cli_task_handle);
    APP_ERROR_CHECK_BOOL(ret == pdPASS);

    ret = xTaskCreate(neopixels_task, "Neo", configMINIMAL_STACK_SIZE + 200,
                      NULL, 2, &neopixels_task_handle);
    APP_ERROR_CHECK_BOOL(ret == pdPASS);

    led_toggle_timer_handle =
        xTimerCreate("LED1", 2000, pdTRUE, NULL, led_toggle_timer_callback);
    APP_ERROR_CHECK_BOOL(led_toggle_timer_handle != NULL);
    ret = xTimerStart(led_toggle_timer_handle, 0);
    APP_ERROR_CHECK_BOOL(ret == pdPASS);

    vTaskStartScheduler();

    APP_ERROR_CHECK_BOOL(0);

    return 0;
}
