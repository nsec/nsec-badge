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
#define MESH_NVS_SEQUENCE_NUMBER "sequence-number"
#define SEQUENCE_NUMBER_PERSIST_INCREMENT 10

static uint32_t last_saved_sequence_number = 0;

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

static esp_err_t save_name(char *name)
{
    nvs_handle_t handle;
    esp_err_t err;

    // Open NVS namespace
    err = nvs_open(MESH_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to open nvs (err %d)", __func__, err);
        return err;
    }

    // Write data to NVS
    err = nvs_set_str(handle, MESH_NVS_NODE_NAME_KEY, name);
    if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to set str (err %d)", __func__, err);
        return err;
    }

    // Commit changes to NVS
    err = nvs_commit(handle);
    if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to commit (err %d)", __func__, err);
        return err;
    }

    // Close NVS namespace
    nvs_close(handle);

    return ESP_OK;
}

/*
    Write stored node name into 'name'

    Before calling, 'name' must be initialized and be at least '*length' bytes long.
    The actual size of 'name' is written in '*length' as output.
*/
static esp_err_t load_name(char *name, size_t *length)
{
    nvs_handle_t handle;
    esp_err_t err;

    err = nvs_open(MESH_NVS_NAMESPACE, NVS_READONLY, &handle);
    if(err == ESP_ERR_NVS_NOT_FOUND) {
        // Partition does not exist, and it is not created since we asked for READONLY.
        // This is expected before name is actually set, we can silently return an error.
        return ESP_FAIL;
    } else if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to open nvs (err %04x)", __func__, err);
        return err;
    }

    // Read data from NVS
    err = nvs_get_str(handle, MESH_NVS_NODE_NAME_KEY, name, length);
    if(err == ESP_ERR_NVS_NOT_FOUND) {
        // Key does not exist, and it is not created since we asked for READONLY.
        // This is expected before key is actually set, we can silently return an error.
        return ESP_FAIL;
    } if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to get str (err %04x)", __func__, err);
        return err;
    }

    // Close NVS namespace
    nvs_close(handle);

    return ESP_OK;
}

esp_err_t mesh_config_name_updated(char *name)
{
    esp_err_t err;

    if(strlen(name) >= sizeof(badge_network_info.name)) {
		ESP_LOGE(TAG, "new name is too long, discarding");
        return ESP_FAIL;
    }

    snprintf(badge_network_info.name, sizeof(badge_network_info.name), name);
    err = save_name(name);
    if(err != ESP_OK) {
		ESP_LOGE(TAG, "New name was not saved (err %d)", err);
        return err;
    }

    return ESP_OK;
}

esp_err_t save_node_addr(uint16_t addr)
{
    nvs_handle_t handle;
    esp_err_t err;

    // Open NVS namespace
    err = nvs_open(MESH_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to open nvs (err %d)", __func__, err);
        return err;
    }

    // Write data to NVS
    err = nvs_set_u16(handle, MESH_NVS_NODE_ADDR_KEY, addr);
    if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to set str (err %d)", __func__, err);
        return err;
    }

    // Commit changes to NVS
    err = nvs_commit(handle);
    if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to commit (err %d)", __func__, err);
        return err;
    }

    // Close NVS namespace
    nvs_close(handle);

    return ESP_OK;
}

/*
    Write stored node address into '*addr'.
*/
esp_err_t load_node_addr(uint16_t *addr)
{
    nvs_handle_t handle;
    esp_err_t err;

    err = nvs_open(MESH_NVS_NAMESPACE, NVS_READONLY, &handle);
    if(err == ESP_ERR_NVS_NOT_FOUND) {
        // Partition does not exist, and it is not created since we asked for READONLY.
        // This is expected before name is actually set, we can silently return an error.
        return ESP_FAIL;
    } else if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to open nvs (err %04x)", __func__, err);
        return err;
    }

    // Read data from NVS
    err = nvs_get_u16(handle, MESH_NVS_NODE_ADDR_KEY, addr);
    if(err == ESP_ERR_NVS_NOT_FOUND) {
        // Key does not exist, and it is not created since we asked for READONLY.
        // This is expected before key is actually set, we can silently return an error.
        return ESP_FAIL;
    } else if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to get u16 (err %04x)", __func__, err);
        return err;
    }

    // Close NVS namespace
    nvs_close(handle);

    return ESP_OK;
}

esp_err_t mesh_sequence_number_changed()
{
    nvs_handle_t handle;
    esp_err_t err;

    if (bt_mesh.seq < (last_saved_sequence_number + SEQUENCE_NUMBER_PERSIST_INCREMENT)) {
        // only save every N sequence number to further decrease the wear on nvs.
        return ESP_OK;
    }

    ESP_LOGV(TAG, "Saving new sequence number=%lu previous=%lu\n", bt_mesh.seq, last_saved_sequence_number);

    // Open NVS namespace
    err = nvs_open(MESH_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to open nvs (err %d)", __func__, err);
        return err;
    }

    // Write data to NVS
    err = nvs_set_u32(handle, MESH_NVS_SEQUENCE_NUMBER, bt_mesh.seq);
    if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to set str (err %d)", __func__, err);
        return err;
    }

    // Commit changes to NVS
    err = nvs_commit(handle);
    if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to commit (err %d)", __func__, err);
        return err;
    }

    last_saved_sequence_number = bt_mesh.seq;

    // Close NVS namespace
    nvs_close(handle);

    return ESP_OK;
}

/*
    Write stored node address into '*addr'.
*/
esp_err_t mesh_load_sequence_number()
{
    nvs_handle_t handle;
    esp_err_t err;

    err = nvs_open(MESH_NVS_NAMESPACE, NVS_READONLY, &handle);
    if(err == ESP_ERR_NVS_NOT_FOUND) {
        // Partition does not exist, and it is not created since we asked for READONLY.
        // This is expected before name is actually set, we can silently return an error.
        return ESP_FAIL;
    } else if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to open nvs (err %04x)", __func__, err);
        return err;
    }

    // Read data from NVS
    err = nvs_get_u32(handle, MESH_NVS_SEQUENCE_NUMBER, &last_saved_sequence_number);
    if(err == ESP_ERR_NVS_NOT_FOUND) {
        // Key does not exist, and it is not created since we asked for READONLY.
        // This is expected before key is actually set, we can silently return an error.
        return ESP_FAIL;
    } else if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: failed to get u16 (err %04x)", __func__, err);
        return err;
    }

    bt_mesh.seq = last_saved_sequence_number;
    if (bt_mesh.seq != 0) {
        // increment by the number of possible sequence numbers we didn't save.
        bt_mesh.seq += SEQUENCE_NUMBER_PERSIST_INCREMENT;
    }

    // Close NVS namespace
    nvs_close(handle);

    return ESP_OK;
}

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

    /* choose a random unicast address unless one is already provisioned */
    if(ESP_OK != load_node_addr(&badge_network_info.unicast_addr)) {
        random_address = true;
        do {
        	esp_fill_random(&badge_network_info.unicast_addr, sizeof(badge_network_info.unicast_addr));

            /* Make sure it's a unicast address (highest bit unset) */
            badge_network_info.unicast_addr &= ~0x8000;
        } while(badge_network_info.unicast_addr >= SCREEN_ADDRESS_RANGE);

        // we must persist the unicast address across device restarts,
        // because there is a finite number of unique addresses that can be stored
        // in each node replay list. once the list space is exhausted new nodes
        // will not be able to communicate (packets won't be relayed).
        save_node_addr(badge_network_info.unicast_addr);
    }
    else {
        random_address = !(badge_network_info.unicast_addr > SCREEN_ADDRESS_RANGE && badge_network_info.unicast_addr < (SCREEN_ADDRESS_RANGE + 0xff));
    }

    memset(badge_network_info.name, 0, sizeof(badge_network_info.name));
    name_length = sizeof(badge_network_info.name);
    err = load_name((char *)&badge_network_info.name, &name_length);
    if(err != ESP_OK) {
        if(random_address) {
            snprintf(badge_network_info.name, sizeof(badge_network_info.name),
                BADGE_NAME_PREFIX " %01x:%01x:%01x:%01x:%01x:%01x", _device_address[5], _device_address[4],
                _device_address[3], _device_address[2], _device_address[1], _device_address[0]
            );
        } else {
            snprintf(badge_network_info.name, sizeof(badge_network_info.name),
                BADGE_NAME_PREFIX " %u", badge_network_info.unicast_addr % SCREEN_ADDRESS_RANGE
            );
        }
    }

    err = mesh_device_auto_enter_network(&badge_network_info);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to auto-enter network", __func__);
        return err;
    }

	ESP_LOGV(TAG, "%s done", __func__);

    return ESP_OK;
}
