#include <nrf_delay.h>
#include <nrf_drv_clock.h>
#include <nrf_gpio.h>
#include <sdk_errors.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include "boards.h"

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

static TaskHandle_t led_toggle_task_handle;
static TimerHandle_t led_toggle_timer_handle;

int main(void)
{
    // ret_code_t ret_code;
    BaseType_t ret;

    // Use this if we want to use the rtc as a tick source for FreeRTOS
    // ret_code = nrf_drv_clock_init();
    // APP_ERROR_CHECK(ret_code);

    nrf_gpio_cfg_output(PIN_LED_STATUS_1);
    nrf_gpio_cfg_output(PIN_LED_STATUS_2);

    nrf_gpio_pin_set(PIN_LED_STATUS_1);
    nrf_gpio_pin_clear(PIN_LED_STATUS_2);

    ret = xTaskCreate(led_toggle_task, "LED", configMINIMAL_STACK_SIZE + 200,
                      NULL, 2, &led_toggle_task_handle);
    APP_ERROR_CHECK_BOOL(ret == pdPASS);

    led_toggle_timer_handle =
        xTimerCreate("LED1", 2000, pdTRUE, NULL, led_toggle_timer_callback);
    APP_ERROR_CHECK_BOOL(led_toggle_timer_handle != NULL);
    ret = xTimerStart(led_toggle_timer_handle, 0);
    APP_ERROR_CHECK_BOOL(pdPASS == pdPASS);

    vTaskStartScheduler();

    APP_ERROR_CHECK_BOOL(0);

    return 0;
}
