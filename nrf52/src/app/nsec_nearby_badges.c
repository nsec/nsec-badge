//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)
#if 0

#include "nsec_nearby_badges.h"
#include "drivers/led_effects.h"
#include "drivers/ws2812fx.h"

#include <string.h>
#include <app_timer.h>

#include "ble/abstract_ble_observer.h"
#include "ble/ble_device.h"

static const uint32_t colors[15] = {0x00FF00, 0x24FF00, 0x48FF00, 0x6DFF00,
                                    0x91FF00, 0xB6FF00, 0xDAFF00, 0xFEFF00,
                                    0xFFDA00, 0xFFB600, 0xFF9100, 0xFF6D00,
                                    0xFF4800, 0xFF2400, 0xFF0000};

static struct {
    uint8_t addr[BLE_GAP_ADDR_LEN];
    uint8_t timeout_in_sec;
} _nearby_badges[NSEC_MAX_NEARBY_BADGES_COUNT];

typedef struct
{
    uint8_t * p_data;
    uint16_t  data_len;
} data_t;

APP_TIMER_DEF(m_nearby_timer);

static void nsec_nearby_badges_process(uint8_t badge_addr[]) {
    for(uint8_t i = 0; i < NSEC_MAX_NEARBY_BADGES_COUNT; i++) {
        if(!memcmp(_nearby_badges[i].addr, badge_addr, BLE_GAP_ADDR_LEN)) {
            _nearby_badges[i].timeout_in_sec = 60;
            return;
        }
    }
    // New badge \o/
    for(uint8_t i = 0; i < NSEC_MAX_NEARBY_BADGES_COUNT; i++) {
        if(_nearby_badges[i].timeout_in_sec == 0) {
            memcpy(_nearby_badges[i].addr, badge_addr, BLE_GAP_ADDR_LEN);
            _nearby_badges[i].timeout_in_sec = 60;
            break;
        }
    }
}

static uint32_t adv_report_parse(uint8_t type, data_t * p_advdata, data_t * p_typedata)
{
    uint32_t  index = 0;
    uint8_t * p_data;

    p_data = p_advdata->p_data;

    while (index < p_advdata->data_len)
    {
        uint8_t field_length = p_data[index];
        uint8_t field_type   = p_data[index + 1];

        if (field_type == type)
        {
            p_typedata->p_data   = &p_data[index + 2];
            p_typedata->data_len = field_length - 1;
            return NRF_SUCCESS;
        }
        index += field_length + 1;
    }
    return NRF_ERROR_NOT_FOUND;
}

static void on_advertising_report(const ble_gap_evt_adv_report_t* report) {
    if (getMode_WS2812FX() != FX_MODE_CUSTOM) {
        return;
    }

    if(report->scan_rsp == 0) {
        data_t adv_data;
        data_t dev_name;
        uint8_t data[32];
        uint8_t addr[BLE_GAP_ADDR_LEN];
        char dev_str[32];

        memcpy(addr, report->peer_addr.addr, BLE_GAP_ADDR_LEN);
        memcpy(data, report->data, BLE_GAP_ADV_SET_DATA_SIZE_MAX);
        adv_data.p_data = data;
        adv_data.data_len = report->dlen;

        if (adv_report_parse(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, &adv_data, &dev_name) != NRF_SUCCESS) {
            if (adv_report_parse(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, &adv_data, &dev_name) != NRF_SUCCESS) {
                return;
            }
        }

        if (strncmp((char*)dev_name.p_data, "NSEC", 4)) {
                return;
        }

        nsec_nearby_badges_process(addr);
    }
}

static void on_scan_timeout(const ble_gap_evt_timeout_t* timeout_event) {

}


static struct BleObserver nearby_badge_observer = {
    &on_advertising_report,
    &on_scan_timeout,
};

static void nsec_nearby_each_second(void * context) {
    for(uint8_t i = 0; i < NSEC_MAX_NEARBY_BADGES_COUNT; i++) {
        if(_nearby_badges[i].timeout_in_sec > 0) {
            _nearby_badges[i].timeout_in_sec--;
        }
    }
}

uint8_t nsec_nearby_badges_current_count(void) {
    uint8_t count = 0;
    for(uint8_t i = 0; i < NSEC_MAX_NEARBY_BADGES_COUNT; i++) {
        if(_nearby_badges[i].timeout_in_sec > 0) {
            count++;
        }
    }
    return count;
}

uint16_t nsec_nearby_badges_pattern(void) {
    uint8_t count = nsec_nearby_badges_current_count();
    uint8_t black_pixel = 0;

    if (count <= 5) {
        black_pixel = 15 - count;
    } else if (count <= 10) {
        black_pixel = 9;
    } else if (count <= 15) {
        black_pixel = 8;
    } else if (count <= 20) {
        black_pixel = 7;
    } else if (count <= 25) {
        black_pixel = 6;
    } else if (count <= 30) {
        black_pixel = 5;
    } else if (count <= 35) {
        black_pixel = 4;
    } else if (count <= 40) {
        black_pixel = 3;
    } else if (count <= 45) {
        black_pixel = 2;
    } else if (count <= 50) {
        black_pixel = 1;
    } else {
        black_pixel = 0;
    }

    for (int i = 0; i < NEOPIXEL_COUNT - black_pixel; i++) {
        nsec_neoPixel_set_pixel_color_packed(i, colors[i]);
    }

    for (int i = NEOPIXEL_COUNT - black_pixel; i < NEOPIXEL_COUNT; i++) {
        nsec_neoPixel_set_pixel_color_packed(i, BLACK);
    }

    return getSegmentSpeed_WS2812FX(0);
}

void select_nearby_badges_pattern(void)
{
     // Reset led params
    setCustomMode(&nsec_nearby_badges_pattern);

    setNumSegments_WS2812FX(1);
    for (int i = 0 ; i < NEOPIXEL_COUNT; i++) {
        nsec_neoPixel_set_pixel_color_packed(i, BLACK);
    }

    setSegment_WS2812FX(0, 0, NEOPIXEL_COUNT - 1, FX_MODE_CUSTOM, GREEN, 500,
                        false);
}

void nsec_nearby_badges_init(void)
{
    static bool is_init = false;
    if (!is_init) {
        is_init = true;
        add_observer(&nearby_badge_observer);
        ret_code_t err_code = app_timer_create(&m_nearby_timer, APP_TIMER_MODE_REPEATED,
                                    nsec_nearby_each_second);
        APP_ERROR_CHECK(err_code);

        memset(_nearby_badges, 0, sizeof(_nearby_badges));
        err_code = app_timer_start(m_nearby_timer, APP_TIMER_TICKS(1000), NULL);
        APP_ERROR_CHECK(err_code);
    }
}
#endif
