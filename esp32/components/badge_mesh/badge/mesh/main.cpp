#include <esp_system.h>
#include <esp_log.h>
#include "esp_ble_mesh_networking_api.h"

#include "badge/mesh/main.h"
#include "badge/mesh/host.h"
#include "badge/mesh/config.h"

static const char *TAG = "badge/mesh";

TaskHandle_t BadgeMesh::_taskHandle = NULL;

void BadgeMesh::init()
{
    xTaskCreate((TaskFunction_t)&(BadgeMesh::task), TAG, 4096, this, 5, &BadgeMesh::_taskHandle);
}

esp_err_t BadgeMesh::clientSend(uint16_t dst_addr, uint32_t op, uint8_t *msg, unsigned int length, bool needsResponse)
{
    esp_err_t err;
	esp_ble_mesh_msg_ctx_t ctx = {
		.net_idx = badge_network_info.net_idx,
		.app_idx = badge_network_info.app_idx,
		.addr = dst_addr,
        .send_rel = 1,
		.send_ttl = DEFAULT_TTL,
	};

	if (xSemaphoreTake(_bt_semaphore, (TickType_t)10) != pdTRUE) {
        ESP_LOGE(TAG, "%s: Could not aquire semaphore", __func__);
        return ESP_FAIL;
    }

    err = esp_ble_mesh_client_model_send_msg(cli_vnd_model, &ctx, op, length, msg, 10, needsResponse, ROLE_NODE);
    xSemaphoreGive(_bt_semaphore);
    return err;
}

esp_err_t BadgeMesh::serverSend(uint16_t dst_addr, uint32_t op, uint8_t *msg, unsigned int length)
{
    esp_err_t err;
	esp_ble_mesh_msg_ctx_t ctx = {
		.net_idx = badge_network_info.net_idx,
		.app_idx = badge_network_info.app_idx,
		.addr = dst_addr,
		.send_ttl = DEFAULT_TTL,
	};

	if (xSemaphoreTake(_bt_semaphore, (TickType_t)10) != pdTRUE) {
        ESP_LOGE(TAG, "%s: Could not aquire semaphore", __func__);
        return ESP_FAIL;
    }

    err = esp_ble_mesh_server_model_send_msg(srv_vnd_model, &ctx, op, length, msg);
    xSemaphoreGive(_bt_semaphore);
    return err;
}

esp_err_t mesh_client_send(uint16_t dst_addr, uint32_t op, uint8_t *msg, unsigned int length, bool needsResponse)
{
    return BadgeMesh::getInstance().clientSend(dst_addr, op, msg, length, needsResponse);
}

esp_err_t mesh_server_send(uint16_t dst_addr, uint32_t op, uint8_t *msg, unsigned int length)
{
    return BadgeMesh::getInstance().serverSend(dst_addr, op, msg, length);
}


void BadgeMesh::taskHandler()
{
    esp_err_t err;

    _bt_semaphore = xSemaphoreCreateMutex();

    vTaskDelay(1500 / portTICK_PERIOD_MS);

    ESP_LOGV(TAG, "Initializing...");

	if (xSemaphoreTake(_bt_semaphore, (TickType_t)9999) == pdTRUE) {
        err = mesh_host_initialize();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize NimBLE");
            return;
        }

        while(!mesh_host_initialized)
        {
            // Wait for the async mesh initialization process to finish
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        ESP_LOGV(TAG, "Mesh host has been initialized");

        err = mesh_configure_esp_ble_mesh();
        if (err) {
            ESP_LOGE(TAG, "Initializing mesh failed (err %d)", err);
            return;
        }

        if (!mesh_is_provisioned()) {
            ESP_LOGE(TAG, "This node is not properly initialized to be part of a mesh");
            return;
        } else {
            ESP_LOGV(TAG, "This node is provisioned correctly");
        }

        xSemaphoreGive(_bt_semaphore);
    }

    while(true) {

        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(BadgeMesh::_taskHandle);
}
