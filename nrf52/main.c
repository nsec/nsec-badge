//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "boards.h"

#include <app_util.h>
#include <app_scheduler.h>
#include <app_error.h>
#include <app_button.h>
#include <app_gpiote.h>
#include <app_uart.h>

#include "ble/nsec_ble.h"

#include <nrf.h>
#include <nrf_error.h>
#include <nrf_gpio.h>
#include <nrf_delay.h>
#include <nrf52.h>
#include <nrf52_bitfields.h>
#include <nordic_common.h>
#include <nrf_sdh.h>

#include <stdbool.h>
#include <stdint.h>

#include "ssd1306.h"

#include "images/nsec_logo_bitmap.c"
#include "status_bar.h"
#include "menu.h"
#include "nsec_conf_schedule.h"
#include "nsec_settings.h"
#include "battery.h"
#include "touch_button.h"
#include "gfx_effect.h"
#include "led_effects.h"
#include "identity.h"
#include "exploit_challenge.h"
#include "nsec_nearby_badges.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "led_effects.h"
#include "ble_lbs.h"
#include "nrf_ble_gatt.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "ble_gap.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#define NSEC_STRINGIFY_(...) #__VA_ARGS__
#define NSEC_STRINGIFY(...) NSEC_STRINGIFY_(__VA_ARGS__)

BLE_LBS_DEF(m_lbs); /**< LED Button Service instance. */
NRF_BLE_GATT_DEF(m_gatt); /**< GATT module instance. */
#define DEVICE_NAME "DevBoard" /**< Name of device. Will be included in the advertising data. */
#define APP_BLE_OBSERVER_PRIO 3 /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG 1
#define MIN_CONN_INTERVAL MSEC_TO_UNITS(100, UNIT_1_25_MS) /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(200, UNIT_1_25_MS) /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY 0 /**< Slave latency. */
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(4000, UNIT_10_MS) /**< Connection supervisory time-out (4 seconds). */
#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(20000) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT 3
#define APP_ADV_INTERVAL                64                                      /**< The advertising interval (in units of 0.625 ms; this value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED   /**< The advertising time-out (in units of seconds). When set to 0, we will never time out. */

static char g_device_id[10];

bool is_at_main_menu = false;
/*
void wdt_init(void)
{
    NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | ( WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);   //Configure Watchdog. a) Pause watchdog while the CPU is halted by the debugger.  b) Keep the watchdog running while the CPU is sleeping.
    NRF_WDT->CRV = 3*32768;             //ca 3 sek. timout
    NRF_WDT->RREN |= WDT_RREN_RR0_Msk;  //Enable reload register 0
    NRF_WDT->TASKS_START = 1;           //Start the Watchdog timer
}
*/
void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info){
	while(1){
		nrf_gpio_pin_clear(20);
		nrf_delay_ms(500);
		nrf_gpio_pin_set(20);
		nrf_delay_ms(500);
	}
}

void print_error_code(char const * function_name, uint32_t err_code){
	if(err_code != NRF_SUCCESS){
		NRF_LOG_INFO("%s: %s", function_name, nrf_strerror_get(err_code));
		NRF_LOG_FLUSH();
	}
}

/*
    static int error_displayed = 0;

    if(!error_displayed) {
        char error_msg[128];
        error_info_t * err_info = (error_info_t *) info;
        snprintf(error_msg, sizeof(error_msg), "%s:%u -> error 0x%08x\r\n", err_info->p_file_name, (unsigned int)err_info->line_num, (unsigned int)err_info->err_code);
        puts(error_msg);
        gfx_setCursor(0, 0);
        gfx_puts(error_msg);
        gfx_update();
        error_displayed = 1;
    }
    uint8_t count = 20;
    while (count > 0) {
        nsec_led_set_effect(count % 2 ? NSEC_LED_EFFECT_ALL_OFF : NSEC_LED_EFFECT_ALL_ON);
        nrf_delay_ms(250);
        count--;
    }

    NVIC_SystemReset();
}


/**
 * Callback function for asserts in the SoftDevice.
 * This function will be called in case of an assert in the SoftDevice.
 *
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name) {
    app_error_handler(0xdeadbeef, line_num, p_file_name);
}


/**
 * Task timers
 *
// Green hearbeat
APP_TIMER_DEF(m_heartbeat_timer_id);
static void heartbeat_timeout_handler(void * p_context) {
    //if (flashlight_cmd_data.state == ST_INIT) {
    //    nrf_gpio_pin_set(LED_RGB_GREEN);
    //}

    NRF_WDT->RR[0] = WDT_RR_RR_Reload;
}

/**
 * Init functions
 *
static void timers_init(void) {
    uint32_t err_code;

    // Initialize timer module.
    APP_ERROR_CHECK(app_timer_init());

    // Create timers.
    err_code = app_timer_create(&m_heartbeat_timer_id,
            APP_TIMER_MODE_REPEATED,
            heartbeat_timeout_handler);
    APP_ERROR_CHECK(err_code);
}
*/

static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context){}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void){
    ret_code_t err_code;
    //print("nrf_sdh_enable_request");
    err_code = nrf_sdh_enable_request();
    //print(nrf_strerror_get(err_code));

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    print_error_code("nrf_sdh_ble_default_cfg_set", err_code);
    NRF_LOG_INFO("Ram start is 0x%x", ram_start);
    NRF_LOG_FLUSH();

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    print_error_code("nrf_sdh_ble_enable", err_code);
    NRF_LOG_INFO("Ram start is 0x%x", ram_start);
    NRF_LOG_FLUSH();
    //APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void){
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    print_error_code("sd_ble_gap_device_name_set", err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    print_error_code("sd_ble_gap_ppcp_set", err_code);
}

/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void){
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    print_error_code("nrf_ble_gatt_init", err_code);
}

/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void){
    ret_code_t    err_code;
    ble_advdata_t advdata;
    ble_advdata_t srdata;

    ble_uuid_t adv_uuids[] = {{LBS_UUID_SERVICE, m_lbs.uuid_type}};

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = true;
    advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;


    memset(&srdata, 0, sizeof(srdata));
    srdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    srdata.uuids_complete.p_uuids  = adv_uuids;

    err_code = ble_advdata_set(&advdata, &srdata);
    print_error_code("ble_advdata_set", err_code);
}

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void){
    ret_code_t     err_code;
    ble_lbs_init_t init;

    //init.led_write_handler = led_write_handler;

    err_code = ble_lbs_init(&m_lbs, &init);
    print_error_code("ble_lbs_init", err_code);
}

static void on_conn_params_evt(ble_conn_params_evt_t * p_evt){}

static void conn_params_error_handler(uint32_t nrf_error){}

/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    sd_ble_gap_adv_start("ble_conn_params_init", err_code);
}

/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    ret_code_t           err_code;
    ble_gap_adv_params_t adv_params;

    // Start advertising
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_params.p_peer_addr = NULL;
    adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    adv_params.interval    = APP_ADV_INTERVAL;
    adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = sd_ble_gap_adv_start(&adv_params, APP_BLE_CONN_CFG_TAG);
    print_error_code("sd_ble_gap_adv_start", err_code);
}

void sys_evt_dispatch(uint32_t evt_id, void * p_context) {

}

static void softdevice_init(void) {
    uint32_t err_code;
/*
    err_code = nrf_sdh_enable_request();
    print_error_code(err_code);
    // Configure the BLE stack using the default settings.
	// Fetch the start address of the application RAM.

	uint32_t ram_start = 0;
	err_code = nrf_sdh_ble_default_cfg_set(1, &ram_start);
	print_error_code(err_code);

	// Enable BLE stack.
	err_code = nrf_sdh_ble_enable(&ram_start);
	print_error_code(err_code);/*
    // Register with the SoftDevice handler module for events.
	NRF_SDH_SOC_OBSERVER(sys_evt_observer, 3, sys_evt_dispatch, NULL);*/
}
/*
static void application_timers_start(void) {
    uint32_t err_code;

    // Start application timers.*/
    //err_code = app_timer_start(m_heartbeat_timer_id, APP_TIMER_TICKS(500 /* ms */, APP_TIMER_PRESCALER), NULL);
    /*APP_ERROR_CHECK(err_code);
}

static void nsec_intro(void) {
    for(uint8_t noise = 128; noise <= 128; noise -= 8) {
        gfx_fillScreen(BLACK);
        gfx_drawBitmap(17, 11, nsec_logo_bitmap, nsec_logo_bitmap_width, nsec_logo_bitmap_height, WHITE);
        nsec_gfx_effect_addNoise(noise);
        gfx_update();
    }
    for(uint8_t noise = 0; noise <= 128; noise += 8) {
        gfx_fillScreen(BLACK);
        gfx_drawBitmap(17, 11, nsec_logo_bitmap, nsec_logo_bitmap_width, nsec_logo_bitmap_height, WHITE);
        nsec_gfx_effect_addNoise(noise);
        gfx_update();
    }
}

void open_animal_care(uint8_t item);
void open_conference_schedule(uint8_t item);
void open_settings(uint8_t item);

static menu_item_s main_menu_items[] = {
    {
        .label = "Conference schedule",
        .handler = open_conference_schedule,
    }, {
        .label = "Settings",
        .handler = open_settings,
    }
};

void open_conference_schedule(uint8_t item) {
    menu_close();
    is_at_main_menu = false;
    nsec_schedule_show_dates();
}

void open_settings(uint8_t item) {
    menu_close();
    is_at_main_menu = false;
    nsec_setting_show();
}

void show_main_menu(void) {
    for(uint8_t noise = 128; noise <= 128; noise -= 16) {
        gfx_fillScreen(BLACK);
        nsec_identity_draw();
        nsec_gfx_effect_addNoise(noise);
        gfx_update();
    }
    nsec_status_bar_ui_redraw();
    menu_init(0, 64-8, 128, 8, sizeof(main_menu_items) / sizeof(main_menu_items[0]), main_menu_items);
    is_at_main_menu = true;
}

*/

void test_neopixels(){
	if(nsec_neopixel_init() == -1)
		return;
	for(int i = 0; i < NEOPIXEL_COUNT; i++){
		nsec_set_pixel_color(i, 255, 255, 255);
	}
	while(1)
		nsec_neopixel_show();
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main() {
#if defined(NSEC_HARDCODED_BLE_DEVICE_ID)
    sprintf(g_device_id, "%.8s", NSEC_STRINGIFY(NSEC_HARDCODED_BLE_DEVICE_ID));
#else
    sprintf(g_device_id, "NSEC%04X", (uint16_t)(NRF_FICR->DEVICEID[1] % 0xFFFF));
#endif
    g_device_id[9] = '\0';

    int leds[] = {17, 18, 19, 20};
	for(int i = 0; i < 4; i++)
		nrf_gpio_cfg_output(leds[i]);
	log_init();
    //softdevice_init();
	ble_stack_init();

	gap_params_init();
	gatt_init();
	services_init();
	advertising_init();
	conn_params_init();
	advertising_start();

    //APP_SCHED_INIT(APP_TIMER_SCHED_EVT_SIZE /* EVENT_SIZE */, 12 /* QUEUE SIZE */);
/*
    timers_init();
    APP_GPIOTE_INIT(2);

    nsec_led_init();
*/
    //test_neopixels();
    while(1){
    	for(int i = 0; i < 4; i++)
    		nrf_gpio_pin_clear(leds[i]);
    	nrf_delay_ms(500);
    	for(int i = 0; i < 4; i++)
			nrf_gpio_pin_set(leds[i]);
		nrf_delay_ms(500);
    }
    /*
    nrf_gpio_pin_clear(22);
    nrf_gpio_pin_clear(23);
    nrf_gpio_pin_clear(18);

    ssd1306_init();
    touch_init();
    gfx_setTextBackgroundColor(WHITE, BLACK);

    application_timers_start();

    nsec_ble_init(g_device_id);
    nsec_ble_add_device_information_service(g_device_id, "NSEC 2017 Badge", NULL, NULL, NULL, NULL);

#ifdef NSEC_VULNERABLE_BLE_SERVICE_ENABLE
    nsec_vuln_init();
#endif

    nsec_identitiy_init();

    nsec_battery_manager_init();

    nsec_status_bar_init();
    nsec_status_set_name(g_device_id);
    nsec_status_set_badge_class(NSEC_STRINGIFY(NSEC_HARDCODED_BADGE_CLASS));
    nsec_status_set_ble_status(STATUS_BLUETOOTH_ON);

    nsec_intro();
    show_main_menu();

    nsec_identity_draw();
    nsec_nearby_badges_init();

    nsec_led_set_delay(100);
    nsec_led_set_effect(NSEC_LED_EFFECT_SPIN);

    while (true) {
        app_sched_execute();

        APP_ERROR_CHECK(sd_app_evt_wait());
    }
*/
    return 0;
}
