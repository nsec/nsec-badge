/*
 * Copyright 2019 Eric Tremblay <habscup@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "cli.h"

#include <app_timer.h>

#include <drivers/cli_uart.h>

#include "ble/abstract_ble_observer.h"
#include "ble/ble_device.h"

#include "status_bar.h"

APP_TIMER_DEF(m_scan_timer);

struct char_doc {
    const char* name;
    const char* UUID;
    const char* description;
    const char* read_security;
    const char* write_security;
};

struct service_doc
{
    const char* name;
    const char* UUID;
    const char* description;
};

typedef struct
{
    uint8_t * p_data;
    uint16_t  data_len;
} data_t;

static struct service_doc identity =
{
    .name = "Identity",
    .UUID = "010d",
    .description = "Customize your identity",
};

static struct char_doc identity_name = {
    .name = "Citizen name",
    .UUID = "011d",
    .description =
        "Set or get the badge's name\r\nPrintable ASCII only, 17 char maximum",
    .read_security = "None",
    .write_security = "Need bonding",
};

static struct service_doc buttons = {
    .name = "Buttons",
    .UUID = "0020",
    .description = "Control the 4 directional buttons",
};

static struct char_doc buttons_up = {
    .name = "Up",
    .UUID = "0120",
    .description =
        "Press or release the button\r\n0x01 = Press, 0x00 = Release",
    .read_security = "No read",
    .write_security = "Need bonding",
};

static struct char_doc buttons_down = {
    .name = "Down",
    .UUID = "0220",
    .description =
        "Press or release the button\r\n0x01 = Press, 0x00 = Release",
    .read_security = "No read",
    .write_security = "Need bonding",
};

static struct char_doc buttons_enter = {
    .name = "Enter",
    .UUID = "0320",
    .description =
        "Press or release the button\r\n0x01 = Press, 0x00 = Release",
    .read_security = "No read",
    .write_security = "Need bonding",
};

static struct char_doc buttons_back = {
    .name = "Back",
    .UUID = "0420",
    .description =
        "Press or release the button\r\n0x01 = Press, 0x00 = Release",
    .read_security = "No read",
    .write_security = "Need bonding",
};

static bool start_scan = false;
static bool scan_in_progress = false;

/* List of seen devices in the current scan. */
static struct {
    uint8_t addr[BLE_GAP_ADDR_LEN];
} device_list[250];
static uint8_t device_count = 0;

static bool is_in_device_list(const ble_gap_evt_adv_report_t* report)
{
    for (int i = 0; i < device_count; i++) {
        if (!memcmp(report->peer_addr.addr, device_list[i].addr,
                    BLE_GAP_ADDR_LEN)) {
            return true;
        }
    }
    return false;
}

static void add_to_device_list(const ble_gap_evt_adv_report_t* report)
{
    memcpy(device_list[device_count].addr, report->peer_addr.addr,
           BLE_GAP_ADDR_LEN);
    device_count++;
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
    if (scan_in_progress) {
        if (!is_in_device_list(report)) {
            data_t adv_data;
            data_t dev_name;
            uint8_t data[32];
            char dev_str[32];

            memcpy(data, report->data, BLE_GAP_ADV_MAX_SIZE);
            adv_data.p_data = data;
            adv_data.data_len = report->dlen;

            if (adv_report_parse(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, &adv_data, &dev_name) != NRF_SUCCESS) {
                if (adv_report_parse(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, &adv_data, &dev_name) != NRF_SUCCESS) {
                    return;
                }
            }

            cli_printf("%02X:%02X:%02X:%02X:%02X:%02X ", report->peer_addr.addr[5],
                       report->peer_addr.addr[4], report->peer_addr.addr[3],
                       report->peer_addr.addr[2], report->peer_addr.addr[1],
                       report->peer_addr.addr[0]);

            strncpy(dev_str, (char*)dev_name.p_data, dev_name.data_len);
            dev_str[dev_name.data_len] = '\0';

            cli_printf("%s ", dev_str);

            cli_printf("%d\r\n", report->rssi);
            add_to_device_list(report);
        }
    }
}

static void on_scan_timeout(const ble_gap_evt_timeout_t* timeout_event){
}

static struct BleObserver ble_scanner = {
    &on_advertising_report,
    &on_scan_timeout,
};

static void scan_timeout_handler(void *p_context) {
    cli_printf("Scan completed\r\n");
    scan_in_progress = false;
}

static void print_service_doc(const nrf_cli_t *p_cli, struct service_doc *doc)
{
    nrf_cli_fprintf( p_cli, NRF_CLI_NORMAL,
        "****************************************************************\r\n");

    nrf_cli_fprintf(
        p_cli, NRF_CLI_NORMAL, "Service name: ");
    nrf_cli_fprintf(
        p_cli, NRF_CLI_DEFAULT, "%s\r\n", doc->name);

    nrf_cli_fprintf(
        p_cli, NRF_CLI_NORMAL, "Service UUID: ");
    nrf_cli_fprintf(
        p_cli, NRF_CLI_DEFAULT, "%s\r\n", doc->UUID);

    nrf_cli_fprintf(
        p_cli, NRF_CLI_NORMAL, "Service description: \r\n");
    nrf_cli_fprintf(
        p_cli, NRF_CLI_DEFAULT, "%s\r\n\n", doc->description);

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Characteristics :\r\n");

}

static void print_char_doc(const nrf_cli_t *p_cli, struct char_doc *doc)
{
    nrf_cli_fprintf(
        p_cli, NRF_CLI_NORMAL, "Characteristic name: ");
    nrf_cli_fprintf(
        p_cli, NRF_CLI_DEFAULT, "%s\r\n", doc->name);

    nrf_cli_fprintf(
        p_cli, NRF_CLI_NORMAL, "Characteristic UUID: ");
    nrf_cli_fprintf(
        p_cli, NRF_CLI_DEFAULT, "%s\r\n", doc->UUID);

    nrf_cli_fprintf(
        p_cli, NRF_CLI_NORMAL, "Characteristic read security: ");
    nrf_cli_fprintf(
        p_cli, NRF_CLI_DEFAULT, "%s\r\n", doc->read_security);

    nrf_cli_fprintf(
        p_cli, NRF_CLI_NORMAL, "Characteristic write security: ");
    nrf_cli_fprintf(
        p_cli, NRF_CLI_DEFAULT, "%s\r\n", doc->write_security);

    nrf_cli_fprintf(
        p_cli, NRF_CLI_NORMAL, "Characteristic description: \r\n");
    nrf_cli_fprintf(
        p_cli, NRF_CLI_DEFAULT, "%s\r\n\n", doc->description);
}

static void do_ble_docs(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (!standard_check(p_cli, argc, 1, argv, NULL, 0)) {
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
        "NorthSec 2019 badge BLE service documentation\r\n\n");

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
        "All services and characteristics are using this base "
        "UUID:\r\na10d{SERVICE/CHAR_UUID}-9d8e-728e-3a49-2a72267b584d\r\nThe "
        "{SERVICE/CHAR_UUID} will be replaced by a 16bit UUID depending on the "
        "service or characteristics.\r\n\n");

    // Insert documentation here
    print_service_doc(p_cli, &identity);
    print_char_doc(p_cli, &identity_name);

    print_service_doc(p_cli, &buttons);
    print_char_doc(p_cli, &buttons_up);
    print_char_doc(p_cli, &buttons_down);
    print_char_doc(p_cli, &buttons_enter);
    print_char_doc(p_cli, &buttons_back);

}

static void do_scan(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    static bool is_observer_added = false;
    if (!standard_check(p_cli, argc, 1, argv, NULL, 0)) {
        return;
    }

    if (!is_ble_enabled()) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                        "BLE is disable, please enable it before scanning\r\n");
        return;
    }

    if (!is_observer_added) {
        add_observer(&ble_scanner);
        is_observer_added = true;

        ret_code_t err_code = app_timer_create(
            &m_scan_timer, APP_TIMER_MODE_SINGLE_SHOT, scan_timeout_handler);
        APP_ERROR_CHECK(err_code);
    }

    if (!scan_in_progress) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                        "Start scanning for nearby devices for 30 seconds\r\n");
        memset(device_list, 0, sizeof(device_list));
        device_count = 0;
        ret_code_t err_code =
            app_timer_start(m_scan_timer, APP_TIMER_TICKS(30000), NULL);
        APP_ERROR_CHECK(err_code);
        scan_in_progress = true;
    } else {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Scan already in progress\r\n");
    }
}

static void do_ble_enable(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (!standard_check(p_cli, argc, 1, argv, NULL, 0)) {
        return;
    }

    bool ble_enable = is_ble_enabled();

    if (argc == 1) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                        "BLE is %s\r\n", (ble_enable) ? "enable" : "disable");
        return;
    } else if (argc == 2) {
        long int val = strtol(argv[1], NULL, 10);
        if (val == 0 && strcmp(argv[1], "0")) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
            return;
        } else if (val != 0 && val != 1) {
            nrf_cli_fprintf(
                p_cli, NRF_CLI_ERROR,
                "%s: Invalid parameter, value should be 1 or 0\r\n", argv[1]);
            return;
        }

        if (val == ble_enable) {
            nrf_cli_fprintf(
                p_cli, NRF_CLI_DEFAULT,
                "BLE is already %s\r\n", (ble_enable) ? "enable" : "disable");
            return;
        } else {
            ble_enable = ble_device_toggle_ble();
            nsec_status_set_ble_status(ble_enable);
             nrf_cli_fprintf(
                p_cli, NRF_CLI_DEFAULT,
                "BLE is now %s\r\n", (ble_enable) ? "enable" : "disable");
            return;
        }
    } else {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n",
                        argv[0]);
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
}

static void do_ble(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (!standard_check(p_cli, argc, 2, argv, NULL, 0)) {
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: unknown parameter: %s\r\n",
                    argv[0], argv[1]);
}

NRF_CLI_CREATE_STATIC_SUBCMD_SET(sub_ble){
    NRF_CLI_CMD(scan, NULL, "Scan BLE devices (Maximum 250 devices)", do_scan),
    NRF_CLI_CMD(ble_enable, NULL,
                "Turn on/off BLE\r\n Usage: blectl ble_enable {0/1}",
                do_ble_enable),
    NRF_CLI_CMD(service_guide, NULL,
                "NorthSec 2019 badge BLE service documentation", do_ble_docs),
    NRF_CLI_SUBCMD_SET_END};

NRF_CLI_CMD_REGISTER(blectl, &sub_ble, "BLE Tools collection", do_ble);
