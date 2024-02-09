## Behavior and usage

### Project Names
Use these projects names on the firmware to be loaded:
| project name | purpose | target partition | flashed when |
| ------------ | ------- | ---------------- | ---------------- |
| nsec-badge | for the conference | factory | before event by staff |
| nsec-ctf | for the CTF | ota_0 | during the CTF at the admin table |
| nsec-ctf-addon | for the CTF Addon | ota_1 | when the addon is connected |

### Commands
`firmware_select` will show the available firmwares to boot into.
The command won't show if the only firmware available is the conference.

### CTF Addon
- When CTF Addon connected, GPIO 12 will be pulled down, and then "CTF Addon detected" will show.
- When this happen, upon boot (any time you flash/monitor/reset), it will check if the OTA_1 partition has already the firmware
- If it doesn't find the firmware with project name "nsec-ctf-addon" in OTA_1, it will flash one from the ctf addon external flash
- Flashing is really quick, like less than 2 seconds!
- After flashing, it automagically reboot into the ctf addon firmware

Blue light (GPIO 7) behavior:
- The blue light on the addon indicate that the addon has been detected.
- When it's blinking very quickly it means that the firmware is being copied
- When it's blinking at 500ms it means that it can't flash the firmware due to errors (see ESP_LOGE outputs); these shouldn't happen outside of provisionning testing
- When it's blinking at 1000ms it means that the flash is not detected but the firmware was; this is prolly a hardware failure on the chip side
- When it's blinking 3 times and then stays on, it means that it won't flash the firmware from its flash because there's already one present.

To update an already existing firmware:
- Disconnect the addon
- Use `parttool.py erase_partition --partition-name=ota_1`
- Do `idf.py flash monitor` if you want to reset the device to boot from factory firmware
- Replug the addon while the esp32s3 is on, then press the restart button
- Check the logs, upon successful behavior you should see `ota_init: CTF Addon detected` and `ota_actions: Writing to OTA partition 1 from external flash...` and also the blue light will blink very quickly during the copy
- The esp32s3 will reboot itself into OTA 1 after the CTF addon firmware is loaded from the flash
- Use the command `firmware_select addon` (available in all firmwares) to boot into the CTF addon firmware

## Build
You have to build 3 times to get 3 files as `project_name.bin` in the build/ directory.
Use `idf.py menuconfig` to select the firmware to build, it will be right there under "NORTHSEC 2024".
You can switch and run them all without deleting build/ because they will have different bin filenames.
Menu config is useful to change the options and you need to run it between builds.
Also sometimes (fresh git clone or clean build) you need to build twice to get the right one (but not the conference this one acts like default).
Use `idf.py build` to build.

## Provisionning
If order to flash your firmware:
- Use `idf.py flash` for the conference firmware. This works like the default.
- Use `parttool.py write_partition --partition-name=ota_0 --input build\nsec-ctf.bin` for the ctf.
- No need to flash the ctf addon, it will take care of it itself!
- To load into the ctf addon you need to use a test device, load the addon firmware into ota_0, uncomment the line `storage_read_from_ota(0, flash);`, build, boot to load into external flash, uncomment the line, build, boot to load into ota 1 and validate it works.
- Most likely https://github.com/nsec/ctf-addon-prov will take care of that alongside the crypto device and generation of flags

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


```
error: 'UART_SCLK_RTC' undeclared (first use in this function); did you mean 'UART_SCLK_APB'?
```
This most likely means you are building esp32 and not esp32s3. Use `idf.py set-target esp32s3` and build again.

