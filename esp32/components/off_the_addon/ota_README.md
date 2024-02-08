## Common errors

```
I (371) ota_init: CTF Addon detected
I (381) ota_init: Initializing external SPI Flash at 20MHz
E (381) memspi: no response

E (381) ota_init: Failed to initialize external Flash: ESP_ERR_INVALID_RESPONSE (0x108)
```
This means you have the ctf addon connected, but the flash is not detected
which means someone has pulled 12 to ground and didn't plug the ctf addon, or something hardware is broken
Blue LED will flash at 1000ms when this happens.

```
I (391) ota_write: Found OTA partition 1! size: 1048576
I (401) ota_write: No ota_1 partition firmware detected: ESP_ERR_NOT_FOUND (0x105)
I (411) ota_write: Writing to OTA partition 1 from external flash...
E (751) esp_ota_ops: OTA image has invalid magic byte (expected 0xE9, saw 0x46)
E (751) ota_write: esp_ota_write failed, error=5379
```
This means the external flash (or ctf addon) has not been provisionned with the firmware
this can be because the FLASH_DEST_ADDR is wrong (it needs to points where the firmware is in the flash, with a size of ota_partition->size)
or something hardware is is broken
Blue LED will flash at 500ms when this happens.

