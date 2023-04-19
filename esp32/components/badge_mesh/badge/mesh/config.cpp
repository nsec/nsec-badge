#include <esp_system.h>
#include <esp_log.h>
#include <esp_random.h>
#include "nvs_flash.h"
#include "nvs.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

#include "badge/mesh/config.h"
#include "badge/mesh/main.h"
#include "badge/mesh/models.h"
#include "badge/mesh/network.h"
#include "badge/mesh/ops.h"
#include "badge/mesh/host.h"

static const char *TAG = "badge/mesh";

#define MESH_NVS_NAMESPACE "badge-mesh"
#define MESH_NVS_NODE_NAME_KEY "node-name"
#define MESH_NVS_NODE_ADDR_KEY "node-addr"

badge_network_info_t badge_network_info = {
    .net_key = {
        0x5a, 0xfd, 0x31, 0x9a, 0x12, 0x4c, 0x4a, 0xa0,
        0x4e, 0xb3, 0x91, 0x7e, 0xa7, 0x6a, 0x81, 0xc4,
    },
    .net_idx = NETWORK_NET_KEY_IDX,
    .flags = NETWORK_FLAGS,
    .iv_index = 0,
    .app_key = {
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8,
        0x1b, 0x2b, 0x3b, 0x4b, 0x5b, 0x6b, 0x7b, 0x8b,
    },
    .app_idx = NETWORK_APP_KEY_IDX,
    .group_addr = NETWORK_GROUP_ADDR,
};

// esp_ble_mesh_cfg_srv_t config_server = {
//     /* 3 transmissions with a 20ms interval */
//     .net_transmit = ESP_BLE_MESH_TRANSMIT(2, 20),
//     .relay = ESP_BLE_MESH_RELAY_ENABLED,
//     .relay_retransmit = ESP_BLE_MESH_TRANSMIT(2, 20),
//     .beacon = ESP_BLE_MESH_BEACON_DISABLED,
//     .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_NOT_SUPPORTED,
//     .friend_state = ESP_BLE_MESH_FRIEND_ENABLED,
//     .default_ttl = 7,
// };
// esp_ble_mesh_client_t config_client;

/*
    Root models are models that are defined by the BLE mesh spec.
*/
esp_ble_mesh_model_t root_models[] = {
    // __ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    // __ESP_BLE_MESH_MODEL_CFG_CLI(&config_client),
};

/*
    Define all client and server vendor models.
*/
esp_ble_mesh_model_t vnd_models[] = {
    __ESP_BLE_MESH_VENDOR_MODEL(NSEC_COMPANY_ID, MODEL_CLI_ID, vnd_cli_ops, NULL, &mesh_client),
    __ESP_BLE_MESH_VENDOR_MODEL(NSEC_COMPANY_ID, MODEL_SRV_ID, vnd_srv_ops, NULL, NULL),
};

esp_ble_mesh_model_t *cli_vnd_model = &vnd_models[0];
esp_ble_mesh_model_t *srv_vnd_model = &vnd_models[1];

/*
    All mesh elements, i.e. all sig models that are prescribed by the BLE mesh spec,
    and all vendor models that are custom defined for this project.
*/
static esp_ble_mesh_elem_t elements[] = {
    {
        .location         = 0,
        .sig_model_count  = ARRAY_SIZE(root_models),
        .vnd_model_count  = ARRAY_SIZE(vnd_models),
        .sig_models       = (root_models),
        .vnd_models       = (vnd_models),
    },
};

/*
    We don't use the mesh provider but we still need to define a device uuid.
*/
static const uint8_t dev_uuid[16] = {};
static esp_ble_mesh_prov_t prov = {
    .uuid = (uint8_t *)&dev_uuid,
};

/*
    Mesh composition data defines all the mesh elements which contain all the mesh models.
*/
static esp_ble_mesh_comp_t comp = {
	.cid = NSEC_COMPANY_ID,
	.element_count = ARRAY_SIZE(elements),
	.elements = elements,
};

esp_err_t mesh_configure_esp_ble_mesh()
{
	int err;
    size_t name_length;
    int random_address = false;

	ESP_LOGV(TAG, "%s starting", __func__);

    err = esp_ble_mesh_register_custom_model_callback(&mesh_custom_model_cb);
	if (err) {
		ESP_LOGE(TAG, "esp_ble_mesh_register_custom_model_callback() failed (err %d)", err);
		return err;
	}

	err = esp_ble_mesh_init(&prov, &comp);
	if (err) {
		ESP_LOGE(TAG, "esp_ble_mesh_init() failed (err %d)", err);
		return err;
	}

    err = esp_ble_mesh_client_model_init(cli_vnd_model);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to initialize fast prov client model", __func__);
        return err;
    }

	esp_fill_random(&badge_network_info.dev_key, sizeof(badge_network_info.dev_key));

    do {
        esp_fill_random(&badge_network_info.unicast_addr, sizeof(badge_network_info.unicast_addr));

        /* Make sure it's a unicast address (highest bit unset) */
        badge_network_info.unicast_addr &= ~0x8000;
    } while(badge_network_info.unicast_addr >= SCREEN_ADDRESS_RANGE);

    snprintf(badge_network_info.name, sizeof(badge_network_info.name),
        BADGE_NAME_PREFIX " %01x:%01x:%01x:%01x:%01x:%01x", _device_address[5], _device_address[4],
        _device_address[3], _device_address[2], _device_address[1], _device_address[0]
    );

    err = mesh_device_auto_enter_network(&badge_network_info);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to auto-enter network", __func__);
        return err;
    }

	ESP_LOGV(TAG, "%s done", __func__);

    return ESP_OK;
}
