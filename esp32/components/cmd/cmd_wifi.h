/* Console example — declarations of command registration functions.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Register WiFi functions
void wifi_disconnect();
void wifi_get_ssid(char *ssid);
void register_wifi();
void wifi_join_if_configured();
void wifi_join_if_configured_task(void *parm);
void register_wifi_disconnect();
bool is_wifi_connected();

#ifdef __cplusplus
}
#endif
