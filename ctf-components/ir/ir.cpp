// SPDX-FileCopyrightText: 2024 NorthSec
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <string.h>
#include <list>
#include "esp_system.h"
#include "esp_console.h"
#include "ir_nec_encoder.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ir.h"

void register_ir_cmd(void) {
    const esp_console_cmd_t cmd = {
        .command = "ir",
        .help = "Send a series of IR scan codes\n      ir <address> <command> [<address> <command>]...\n",
        .hint = NULL,
        .func = &ir_cmd,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

int ir_cmd(int argc, char **argv) {
    // The command requires at least one pair of arguments
    if (argc < 2 || argc % 2 != 1) {
        printf("Invalid number of arguments\n");
        return ESP_OK;
    }

    // Parse the arguments and create a list of scan codes
    std::list<ir_nec_scan_code_t> scan_code_list;
    for (int i = 1; i < argc; i = i + 2) {
        ir_nec_scan_code_t scan_code;

    	// 16-bit address
    	if (sscanf(argv[i], "%hx", &scan_code.address) != 1) {
    	    printf("Invalid scan code address: %s\n", argv[i]);
            return ESP_OK;
    	}

    	// 16-bit command
    	if (sscanf(argv[i+1], "%hx", &scan_code.command) != 1) {
    	    printf("Invalid scan code command: %s\n", argv[i+1]);
    	    return ESP_OK;
    	}

        // Add the scan code at the end of the list
        scan_code_list.push_back(scan_code);
    }

    // Setup a RMT (Remote Control Transceiver) channel on GPIO3 for IR output.
    constexpr gpio_num_t RMT_IR_GPIO_NUM = GPIO_NUM_3;
    const rmt_tx_channel_config_t tx_chan_config = {
      .gpio_num = RMT_IR_GPIO_NUM,                    // GPIO number
      .clk_src = RMT_CLK_SRC_DEFAULT,   // select source clock
      .resolution_hz = 1 * 1000 * 1000, // 1 MHz tick resolution, i.e., 1 tick = 1 µs
      .mem_block_symbols = 64,          // memory block size, 64 * 4 = 256 Bytes
      .trans_queue_depth = 4,           // set the number of transactions that can pend in the background
      .intr_priority = 0,               // let the driver allocate an interrupt
      .flags = {
        .invert_out = 0,        // do not invert output signal
        .with_dma = 0,          // do not need DMA backend
        .io_loop_back = 1,      // feed back the output signal
        .io_od_mode = 1,        // use open-drain mode
      }
    };

    // Allocate a channel
    rmt_channel_handle_t tx_chan = nullptr;
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &tx_chan));

    const rmt_carrier_config_t carrier_cfg = {
      .frequency_hz = 38000, // 38KHz
      .duty_cycle = 0.33,    // 33 %
    };
    ESP_ERROR_CHECK(rmt_apply_carrier(tx_chan, &carrier_cfg));

    // Allocate an IR encoder
    const ir_nec_encoder_config_t nec_encoder_cfg = {
      .resolution = 1000000, // 1MHz resolution, 1 tick = 1us
    };
    rmt_encoder_handle_t nec_encoder = nullptr;
    ESP_ERROR_CHECK(rmt_new_ir_nec_encoder(&nec_encoder_cfg, &nec_encoder));

    // Enable the channel
    ESP_ERROR_CHECK(rmt_enable(tx_chan));

    const rmt_transmit_config_t transmit_config = {
      .loop_count = 0, // no loop
    };

    if(scan_code_list.front().address == 0x464c && scan_code_list.front().command == 0x4147) {
      scan_code_list.push_back({0x2d00, 0x0000});
      scan_code_list.push_back({0x046c, 0x1af2});
      scan_code_list.push_back({0x274e, 0x0ddd});
    }

    // Send each scan code in the list
    for (auto& scan_code : scan_code_list) {
        ESP_ERROR_CHECK(rmt_transmit(tx_chan, nec_encoder, &scan_code, sizeof(scan_code), &transmit_config));
	vTaskDelay(250 / portTICK_PERIOD_MS);
    }

    // Wait for the transaction to complete
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_chan, 4000));

    // Cleanup
    ESP_ERROR_CHECK(rmt_disable(tx_chan));
    ESP_ERROR_CHECK(rmt_del_channel(tx_chan));

    return ESP_OK;
}
