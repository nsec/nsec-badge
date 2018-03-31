//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "nrf.h"
#include <nrf_delay.h>
#include "nordic_common.h"
#include "boards.h"
#include "nrf_fstorage.h"

#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_fstorage_sd.h"

#include "led_code_storage.h"

#define PAGE_START_MAGIC    0xDEADC0DE

static uint32_t stored_code;

uint32_t pw[] = {SPONSOR_0_PW, SPONSOR_1_PW, SPONSOR_2_PW, SPONSOR_3_PW, SPONSOR_4_PW,
                SPONSOR_5_PW, SPONSOR_6_PW, SPONSOR_7_PW, SPONSOR_8_PW, SPONSOR_9_PW,
                SPONSOR_10_PW, SPONSOR_11_PW, SPONSOR_12_PW, SPONSOR_13_PW, SPONSOR_14_PW,
                SPONSOR_15_PW};

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
{
    /* Set a handler for fstorage events. */
    .evt_handler = NULL,

    /* These below are the boundaries of the flash space assigned to this instance of fstorage.
     * You must set these manually, even at runtime, before nrf_fstorage_init() is called.
     * The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
     * last page of flash available to write data. */
    .start_addr = 0x7D000,
    .end_addr   = 0x7E000,
};

void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage)
{
    /* While fstorage is busy*/
    while (nrf_fstorage_is_busy(p_fstorage)) {
        nrf_delay_us(10);
    }
}

void update_stored_code() {
    ret_code_t rc;

    rc = nrf_fstorage_erase(&fstorage, fstorage.start_addr, 1, NULL);
    APP_ERROR_CHECK(rc);
    wait_for_flash_ready(&fstorage);

    rc = nrf_fstorage_write(&fstorage, fstorage.start_addr, &stored_code, 4, NULL);
    APP_ERROR_CHECK(rc);
    wait_for_flash_ready(&fstorage);
}

void unlock_all_code() {
    if (stored_code != 0xFFFFFFFF) {
        stored_code = 0xFFFFFFFF;
        update_stored_code();
    }
}

void unlock_code(uint32_t sponsor_index) {
    if (sponsor_index < 31) {
        SET_BIT(stored_code, sponsor_index);
        update_stored_code();
    }
}

uint32_t nsec_get_code_pw(uint32_t sponsor_index) {
    if (sponsor_index < SPONSOR_PW_SIZE) {
        return pw[sponsor_index];
    }
    return 0;
}

bool nsec_code_is_unlock(uint32_t sponsor_index) {
    return IS_SET(stored_code, sponsor_index);
}

// true valid, false invalid
bool nsec_unlock_code(uint32_t password) {
    if (password == MASTER_PW) {
        unlock_all_code();
        return true;
    }
    for (int i = 0; i < SPONSOR_PW_SIZE; i++) {
        if (password == pw[i]) {
            if (!nsec_code_is_unlock(i)) {
                unlock_code(i);
            }
            return true;
        }
    }
    return false;
}

void nsec_code_storage_init() {
    ret_code_t rc;

    nrf_fstorage_api_t *p_fs_api;
    p_fs_api = &nrf_fstorage_sd;

    rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);
    
    /* Get actual stored_code */
    rc = nrf_fstorage_read(&fstorage, fstorage.start_addr, &stored_code, 4);
    APP_ERROR_CHECK(rc);
    wait_for_flash_ready(&fstorage);

    // Check if this a fresh device
    if (stored_code == PAGE_START_MAGIC) {
        stored_code = 0;
        rc = nrf_fstorage_write(&fstorage, fstorage.start_addr, &stored_code, 4, NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&fstorage);
    }
}
