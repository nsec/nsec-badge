//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)


#include "ble/nsec_ble.h"

#include <nrf.h>
#include <nrf_error.h>
#include <nrf_gpio.h>
#include <nrf_delay.h>
#include <nrf52.h>
#include <nrf52_bitfields.h>
#include <nordic_common.h>
#include <stdint.h>
#include <nrf_soc.h>

#include "ble/ble_device.h"
#include "drivers/battery.h"
#include "drivers/battery_manager.h"
#include "drivers/buttons.h"
#include "drivers/display.h"
#include "drivers/flash.h"
#include "drivers/power.h"
#include "drivers/softdevice.h"
#include "drivers/ws2812fx.h"
#include "drivers/nsec_storage.h"

#include "application.h"
#include "cli.h"
#include "logs.h"
#include "gfx_effect.h"
#include "identity.h"
#include "nsec_conf_schedule.h"
#include "nsec_settings.h"
#include "timer.h"
#include "status_bar.h"
#include "utils.h"
#include "nsec_led_pattern.h"
#include "nsec_warning.h"
#include "flash_mode.h"
#include "home_menu.h"
#include "mode_zombie.h"
#include "persistency.h"
#include "app_soldering.h"
#include "app_intro.h"

#include "ble/service_characteristic.h"
#include "ble/vendor_service.h"
#include "images/nsec_logo_bitmap.h"

#include "ble/resistance_bar_beacon.h"
#include "resistance_propaganda_observer.h"

static char g_device_id[10];

/*
 * Callback function when APP_ERROR_CHECK() fails
 *
 *  - Display the filename, line number and error code.
 *  - Flash all neopixels red for 5 seconds.
 *  - Reset the system.
 */
void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info) {
    static int error_displayed = 0;
    uint8_t count = 50;

    if(!error_displayed) {
        char error_msg[128];
        error_info_t *err_info = (error_info_t *) info;
        snprintf(error_msg, sizeof(error_msg), "%s:%u -> error 0x%08x\r\n",
                 err_info->p_file_name, (unsigned int)err_info->line_num,
                 (unsigned int)err_info->err_code);
        puts(error_msg);
        gfx_set_cursor(0, 0);
        gfx_puts(error_msg);
        gfx_update();
        error_displayed = 1;
    }

    init_WS2812FX();
    setBrightness_WS2812FX(64);
    setSpeed_WS2812FX(200);
    setColor_WS2812FX(255, 0, 0);
    setMode_WS2812FX(FX_MODE_BLINK);
    start_WS2812FX();

    while (count > 0) {
        service_WS2812FX();
        nrf_delay_ms(100);
        count--;
    }

    NVIC_SystemReset();
}

static void init_ble() {
    create_ble_device(g_device_id);
#ifdef NSEC_FLAVOR_BAR_BEACON
    init_resistance_bar_beacon();
    set_advertiser(get_resistance_bar_beacon());
    start_advertising();
#else
    add_observer(get_resistance_propaganda_observer());
    ble_device_start_scan();
#endif
}

static void main_service_device() {
    cli_process();
    nsec_controls_process();
    battery_status_process();
    mode_zombie_process();
    service_WS2812FX();

    /* Wait until next event */
    power_manage();
}

int main(void) {
#if defined(NSEC_HARDCODED_BLE_DEVICE_ID)
    sprintf(g_device_id, "%.8s", NSEC_STRINGIFY(NSEC_HARDCODED_BLE_DEVICE_ID));
#else
    sprintf(g_device_id, "NSEC%04X", (uint16_t)(NRF_FICR->DEVICEID[1] % 0xFFFF));
#endif
    g_device_id[9] = '\0';

    /*
     * Initialize base hardware
     */
    power_init();
    softdevice_init();
    timer_init();
    flash_init();
    init_WS2812FX();
    display_init();
    load_persistency();
    nsec_buttons_init();

#ifdef NSEC_FLAVOR_CTF
    mode_zombie_init();
#endif

    // Enter flash mode if the "up" button is pressed.
    if (nsec_button_is_pushed(BUTTON_UP)) {
      flash_mode();
    }

    // cli_init depends on timer_init, and also needs to be after flash_mode
    // (the CLI takes over the UART, which the flash mode uses).
    cli_init();

    init_ble();
    //nsec_status_set_name(g_device_id);
    //nsec_status_set_badge_class(NSEC_STRINGIFY(NSEC_HARDCODED_BADGE_CLASS));

#ifdef SOLDERING_TRACK
    application_set(app_soldering);
#else
    /* Set the intro as the first app to run */
    application_set(app_intro);
#endif

    /*
     * Main loop
     */
    while (true) {
        application_t application = application_get();
        if (application) {
            application(main_service_device);
        }

        main_service_device();
    }

    return 0;
}
