#include <esp_system.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_wifi.h>
#include <esp_netif.h>

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/lwip_napt.h"
#include "dhcpserver/dhcpserver.h"
#include "esp_wifi_netif.h"

#include "save.h"
#include "wifi.h"
#include "modbus.h"

static const char *TAG = "wifi";

#define RAND_CHR (char)((esp_random() % 26) + 0x61)

static esp_event_handler_instance_t event_handler_instance;

const esp_netif_ip_info_t subnet_ip = {
        .ip = { .addr = ESP_IP4TOADDR( 192, 168, 4, 1) },
        .netmask = { .addr = ESP_IP4TOADDR( 255, 255, 255, 0) },
        .gw = { .addr = ESP_IP4TOADDR( 192, 168, 4, 1) },
};

static bool wifi_config_saved()
{
    return strlen(Save::save_data.wifi_ssid) && strlen(Save::save_data.wifi_password);
}

void Wifi::init()
{
    if(!wifi_config_saved()) {
        snprintf((char *)&Save::save_data.wifi_ssid, sizeof(Save::save_data.wifi_ssid), "PAD-%08lx", esp_random());
        snprintf((char *)&Save::save_data.wifi_password, sizeof(Save::save_data.wifi_password), "%c%c%c%c%c%c%c%c%c%c%c%c",
        RAND_CHR, RAND_CHR, RAND_CHR, RAND_CHR, RAND_CHR, RAND_CHR,
        RAND_CHR, RAND_CHR, RAND_CHR, RAND_CHR, RAND_CHR, RAND_CHR
        );
    }

    char ssid_log[1024];
    sprintf(ssid_log, "SSID: %s, Password: %s\n", Save::save_data.wifi_ssid, Save::save_data.wifi_password);
    printf(ssid_log);
    _enabled = false;
    _state = State::Disabled;

    enable();
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{

    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
    else {
        ESP_LOGI(TAG, "%s: other event %ld", __func__, event_id);
    }
}

static esp_err_t set_dhcps_dns(esp_netif_t *netif, uint32_t addr)
{
    esp_netif_dns_info_t dns;
    dns.ip.u_addr.ip4.addr = addr;
    dns.ip.type = ESP_IPADDR_TYPE_V4;
    dhcps_offer_t dhcps_dns_value = OFFER_DNS;
    ESP_ERROR_CHECK(esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_DOMAIN_NAME_SERVER, &dhcps_dns_value, sizeof(dhcps_dns_value)));
    ESP_ERROR_CHECK(esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns));
    return ESP_OK;
}

esp_err_t Wifi::enable()
{
    esp_err_t ret;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    if(_enabled) {
        ESP_LOGV(TAG, "Already enabled...");
        return ESP_OK;
    }

    strcpy((char *)&_config.ap.ssid, Save::save_data.wifi_ssid);
    strcpy((char *)&_config.ap.password, Save::save_data.wifi_password);

    _config.ap.ssid_len = strlen((char *)&_config.ap.ssid);
    _config.ap.channel = 0;
    _config.ap.max_connection = 10;
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
    _config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    // _config.ap.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;
#else
    _config.ap.authmode = WIFI_AUTH_WPA2_PSK;
#endif
    _config.ap.pmf_cfg.required = true;

    _enabled = true;

    ret = esp_netif_init();
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Could not initialize netif (%s)", __func__, esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ret = esp_event_loop_create_default();
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Could not create event loop (%s)", __func__, esp_err_to_name(ret));
        goto fail;
    }

    ret = esp_wifi_init(&cfg);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Could not initialize wifi (%s)", __func__, esp_err_to_name(ret));
        goto fail;
    }
    ret = esp_wifi_set_country_code("CA", false);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Could not set country code (%s)", __func__, esp_err_to_name(ret));
        goto fail;
    }

    ret = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
        &wifi_event_handler, NULL, &event_handler_instance);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Could not register event handler (%s)", __func__, esp_err_to_name(ret));
        goto fail;
    }

    ret = esp_wifi_set_mode(WIFI_MODE_AP);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Could not set wifi AP mode (%s)", __func__, esp_err_to_name(ret));
        goto fail;
    }

    ret = esp_wifi_set_config(WIFI_IF_AP, &_config);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Could not set wifi config (%s)", __func__, esp_err_to_name(ret));
        goto fail;
    }

    _netif_ap = esp_netif_create_default_wifi_ap();
    assert(_netif_ap);

    set_dhcps_dns(_netif_ap, ESP_IP4TOADDR( 8, 8, 8, 8 ));
    ip_napt_enable(subnet_ip.ip.addr, 1);

    ret = esp_wifi_start();
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Could not start wifi (%s)", __func__, esp_err_to_name(ret));
        goto fail;
    }

    modbus_start(_netif_ap);

    _state = State::Enabled;

    return ESP_OK;
fail:
    esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler_instance);
    esp_wifi_deinit();
    esp_event_loop_delete_default();
    _state = State::Failed;
    return ret;
}

esp_err_t Wifi::disable()
{
    esp_err_t ret;

    if(!_enabled) {
        ESP_LOGV(TAG, "Already disabled...");
        return ESP_OK;
    }

    _enabled = false;

    modbus_stop();

    ret = esp_wifi_stop();
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Could not stop wifi (%s)", __func__, esp_err_to_name(ret));
    }

    ret = esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler_instance);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Could not unregister event handler instance (%s)", __func__, esp_err_to_name(ret));
    }

    ret = esp_wifi_deinit();
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Could not deinit wifi (%s)", __func__, esp_err_to_name(ret));
    }

    ret = esp_event_loop_delete_default();
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Could not delete default event loop (%s)", __func__, esp_err_to_name(ret));
    }

    _state = State::Disabled;

    return ESP_OK;
}
