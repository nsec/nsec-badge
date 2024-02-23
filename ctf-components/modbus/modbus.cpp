/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// FreeModbus Slave Example ESP32

#include "mbcontroller.h"       // for mbcontroller defines and api

#include "modbus.h"

static const char *TAG = "SLAVE_TEST";

// Set register values into known state
static void setup_reg_data(void)
{
    // Define initial state of parameters
    discrete_reg_params.discrete_input0 = 1;
    discrete_reg_params.discrete_input1 = 0;
    discrete_reg_params.discrete_input2 = 1;
    discrete_reg_params.discrete_input3 = 0;
    discrete_reg_params.discrete_input4 = 1;
    discrete_reg_params.discrete_input5 = 0;
    discrete_reg_params.discrete_input6 = 1;
    discrete_reg_params.discrete_input7 = 0;

    holding_reg_params.holding_data0 = 1.34;
    holding_reg_params.holding_data1 = 2.56;
    holding_reg_params.holding_data2 = 3.78;
    holding_reg_params.holding_data3 = 4.90;

    holding_reg_params.holding_data4 = 5.67;
    holding_reg_params.holding_data5 = 6.78;
    holding_reg_params.holding_data6 = 7.79;
    holding_reg_params.holding_data7 = 8.80;
    coil_reg_params.coils_port0 = 0x55;
    coil_reg_params.coils_port1 = 0xAA;

    holding_reg_params.test_regs[45] = 0x464c;
    holding_reg_params.test_regs[46] = 0x4147;
    holding_reg_params.test_regs[47] = 0x2d4d;
    holding_reg_params.test_regs[48] = 0x3064;
    holding_reg_params.test_regs[49] = 0x6275;
    holding_reg_params.test_regs[50] = 0x2452;
    holding_reg_params.test_regs[51] = 0x5455;

    input_reg_params.input_data0 = 1.12;
    input_reg_params.input_data1 = 2.34;
    input_reg_params.input_data2 = 3.56;
    input_reg_params.input_data3 = 4.78;
    input_reg_params.input_data4 = 1.12;
    input_reg_params.input_data5 = 2.34;
    input_reg_params.input_data6 = 3.56;
    input_reg_params.input_data7 = 4.78;
}

// Modbus slave initialization
static esp_err_t slave_init(esp_netif_t *netif, mb_communication_info_t* comm_info)
{
    mb_register_area_descriptor_t reg_area; // Modbus register area descriptor structure

    void* slave_handler = NULL;

    // Initialization of Modbus controller
    esp_err_t err = mbc_slave_init_tcp(&slave_handler);
    MB_RETURN_ON_FALSE((err == ESP_OK && slave_handler != NULL), ESP_ERR_INVALID_STATE,
                                TAG,
                                "mb controller initialization fail.");

    comm_info->ip_addr = NULL; // Bind to any address
    comm_info->ip_netif_ptr = netif;

    // Setup communication parameters and start stack
    err = mbc_slave_setup((void*)comm_info);
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                        TAG,
                                        "mbc_slave_setup fail, returns(0x%x).",
                                        (uint32_t)err);

    // The code below initializes Modbus register area descriptors
    // for Modbus Holding Registers, Input Registers, Coils and Discrete Inputs
    // Initialization should be done for each supported Modbus register area according to register map.
    // When external master trying to access the register in the area that is not initialized
    // by mbc_slave_set_descriptor() API call then Modbus stack
    // will send exception response for this register area.
    reg_area.type = MB_PARAM_HOLDING; // Set type of register area
    reg_area.start_offset = MB_REG_HOLDING_START_AREA0; // Offset of register area in Modbus protocol
    reg_area.address = (void*)&holding_reg_params.holding_data0; // Set pointer to storage instance
    reg_area.size = (MB_REG_HOLDING_START_AREA1 - MB_REG_HOLDING_START_AREA0) << 1; // Set the size of register storage instance
    err = mbc_slave_set_descriptor(reg_area);
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                    TAG,
                                    "mbc_slave_set_descriptor fail, returns(0x%x).",
                                    (uint32_t)err);

    reg_area.type = MB_PARAM_HOLDING; // Set type of register area
    reg_area.start_offset = MB_REG_HOLDING_START_AREA1; // Offset of register area in Modbus protocol
    reg_area.address = (void*)&holding_reg_params.holding_data4; // Set pointer to storage instance
    reg_area.size = sizeof(float) << 2; // Set the size of register storage instance
    err = mbc_slave_set_descriptor(reg_area);
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                    TAG,
                                    "mbc_slave_set_descriptor fail, returns(0x%x).",
                                    (uint32_t)err);

    // Initialization of Input Registers area
    reg_area.type = MB_PARAM_INPUT;
    reg_area.start_offset = MB_REG_INPUT_START_AREA0;
    reg_area.address = (void*)&input_reg_params.input_data0;
    reg_area.size = sizeof(float) << 2;
    err = mbc_slave_set_descriptor(reg_area);
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                        TAG,
                                        "mbc_slave_set_descriptor fail, returns(0x%x).",
                                        (uint32_t)err);
    reg_area.type = MB_PARAM_INPUT;
    reg_area.start_offset = MB_REG_INPUT_START_AREA1;
    reg_area.address = (void*)&input_reg_params.input_data4;
    reg_area.size = sizeof(float) << 2;
    err = mbc_slave_set_descriptor(reg_area);
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                        TAG,
                                        "mbc_slave_set_descriptor fail, returns(0x%x).",
                                        (uint32_t)err);

    // Initialization of Coils register area
    reg_area.type = MB_PARAM_COIL;
    reg_area.start_offset = MB_REG_COILS_START;
    reg_area.address = (void*)&coil_reg_params;
    reg_area.size = sizeof(coil_reg_params);
    err = mbc_slave_set_descriptor(reg_area);
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                    TAG,
                                    "mbc_slave_set_descriptor fail, returns(0x%x).",
                                    (uint32_t)err);

    // Initialization of Discrete Inputs register area
    reg_area.type = MB_PARAM_DISCRETE;
    reg_area.start_offset = MB_REG_DISCRETE_INPUT_START;
    reg_area.address = (void*)&discrete_reg_params;
    reg_area.size = sizeof(discrete_reg_params);
    err = mbc_slave_set_descriptor(reg_area);
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                    TAG,
                                    "mbc_slave_set_descriptor fail, returns(0x%x).",
                                    (uint32_t)err);

    // Set values into known state
    setup_reg_data();

    // Starts of modbus controller and stack
    err = mbc_slave_start();
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                        TAG,
                                        "mbc_slave_start fail, returns(0x%x).",
                                        (uint32_t)err);
    vTaskDelay(5);
    return err;
}

static esp_err_t slave_destroy(void)
{
    esp_err_t err = mbc_slave_destroy();
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                TAG,
                                "mbc_slave_destroy fail, returns(0x%x).",
                                (uint32_t)err);
    return err;
}

void modbus_start(esp_netif_t *netif)
{
    mb_communication_info_t comm_info = { };

#if !CONFIG_EXAMPLE_CONNECT_IPV6
    comm_info.ip_addr_type = MB_IPV4;
#else
    comm_info.ip_addr_type = MB_IPV6;
#endif
    comm_info.ip_mode = MB_MODE_TCP;

    comm_info.ip_port = MB_TCP_PORT_NUMBER;
    ESP_ERROR_CHECK(slave_init(netif, &comm_info));
}

void modbus_work()
{
    mb_param_info_t reg_info; // keeps the Modbus registers access information

    // Check for read/write events of Modbus master for certain events
    mb_event_group_t event = mbc_slave_check_event((mb_event_group_t)(MB_READ_MASK | MB_WRITE_MASK));
    const char* rw_str = (event & MB_READ_MASK) ? "READ" : "WRITE";
    // Filter events and process them accordingly
    if(event & (MB_EVENT_HOLDING_REG_WR | MB_EVENT_HOLDING_REG_RD)) {
        // Get parameter information from parameter queue
        ESP_ERROR_CHECK(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
        ESP_LOGI(TAG, "HOLDING %s (%u us), ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                rw_str,
                (uint32_t)reg_info.time_stamp,
                (uint32_t)reg_info.mb_offset,
                (uint32_t)reg_info.type,
                (uint32_t)reg_info.address,
                (uint32_t)reg_info.size);
        if (reg_info.address == (uint8_t*)&holding_reg_params.holding_data0)
        {
            portENTER_CRITICAL(&param_lock);
            holding_reg_params.holding_data0 += MB_CHAN_DATA_OFFSET;
            if (holding_reg_params.holding_data0 >= (MB_CHAN_DATA_MAX_VAL - MB_CHAN_DATA_OFFSET)) {
                coil_reg_params.coils_port1 = 0xFF;
            }
            portEXIT_CRITICAL(&param_lock);
        }
    } else if (event & MB_EVENT_INPUT_REG_RD) {
        ESP_ERROR_CHECK(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
        ESP_LOGI(TAG, "INPUT READ (%u us), ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                (uint32_t)reg_info.time_stamp,
                (uint32_t)reg_info.mb_offset,
                (uint32_t)reg_info.type,
                (uint32_t)reg_info.address,
                (uint32_t)reg_info.size);
    } else if (event & MB_EVENT_DISCRETE_RD) {
        ESP_ERROR_CHECK(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
        ESP_LOGI(TAG, "DISCRETE READ (%u us): ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                            (uint32_t)reg_info.time_stamp,
                            (uint32_t)reg_info.mb_offset,
                            (uint32_t)reg_info.type,
                            (uint32_t)reg_info.address,
                            (uint32_t)reg_info.size);
    } else if (event & (MB_EVENT_COILS_RD | MB_EVENT_COILS_WR)) {
        ESP_ERROR_CHECK(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
        ESP_LOGI(TAG, "COILS %s (%u us), ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                            rw_str,
                            (uint32_t)reg_info.time_stamp,
                            (uint32_t)reg_info.mb_offset,
                            (uint32_t)reg_info.type,
                            (uint32_t)reg_info.address,
                            (uint32_t)reg_info.size);
        if (coil_reg_params.coils_port1 == 0xFF) return;
    }

    return;
}

void modbus_stop() {
    ESP_ERROR_CHECK(slave_destroy());
}
