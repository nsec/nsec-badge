# Usage and behavior

## Project names
Use these projects names on the firmware to be loaded:
| project name | purpose | target partition | flashed when |
| ------------ | ------- | ---------------- | ---------------- |
| nsec-badge | for the conference | `factory` | before the event by staff |
| nsec-ctf | for the CTF | `ota_0` | during the CTF at the admin table |
| nsec-ctf-addon | for the CTF Addon | `ota_1` | when the add-on is connected |

## Loading firmware
`firmware_select <project_name>` loads the firmware associated to the project
name. Without argument, the command shows available firmwares to boot.
If there is only one firmware, the command is not available.

## Build
You have to build 3 times to get 3 files as `project_name.bin` in the `build/`
directory. Use `idf.py menuconfig` to select the firmware to build, it will be
right there under "NORTHSEC 2024". You can switch and run them all without
deleting `build/` because they will have different bin filenames. `menuconfig`
is useful to change the options and you need to run it between builds. Also
sometimes (fresh git clone or clean build) you need to build twice to get the
right one (but not the conference this one acts like default). Use `idf.py
build` to build.

## Provisioning
If order to flash your firmware:
- Use `idf.py flash` for the conference firmware. This works like the default.
- Use `parttool.py write_partition --partition-name=ota_0 --input
  build\nsec-ctf.bin` for the CTF.
- No need to flash the CTF Addon, it will take care of it itself!
- To load into the CTF Addon you need to use a test device, load the add-on
  firmware into `ota_0`, uncomment the line `storage_read_from_ota(0, flash);`,
  build, boot to load into external flash, uncomment the line, build, boot to
  load into ota 1 and validate it works.
- Most likely https://github.com/nsec/ctf-addon-prov will take care of that
  alongside the crypto device and generation of flags.

## CTF Addon details
- When the CTF Addon is connected, the `GPIO 12` is grounded and the message
  "CTF Addon detected" shows in the log.
- During the boot sequence, `ota_1` partition is checked if the
  "nsec-ctf-addon" is present.
- If the firmware "nsec-ctf-addon" is not present in `ota_1`, it flashes the
  firmware from the CTF Addon external flash.
- Flashing is really quick, like less than 2 seconds!
- After flashing, it automagically reboots into the CTF Addon firmware.

### Blue light (`GPIO 7`) addon behavior
- When the blue light on the CTF Addon is on, it means that the add-on has been detected.
- When it's blinking very quickly, it means that the firmware is being copied.
- When it's blinking at 500ms it means that it can't flash the firmware due to
  errors (see `ESP_LOGE` outputs); these shouldn't happen outside of
  provisioning testing.
- When it's blinking at 1000ms it means that the flash is not detected but the
  firmware was; this is prolly a hardware failure on the chip side.
- When it's blinking 3 times and then stays on, it means that it won't flash
  the firmware from its flash because there's already one present.

### Update existing addon firmware
- Disconnect the CTF Addon.
- Execute `parttool.py erase_partition --partition-name=ota_1`.
- Execute `idf.py flash monitor` if you want to reset the device to boot from
  factory firmware.
- Re-plug the add-on while the esp32s3 is on, then press the `RST` button.
- Check the logs, upon successful behavior you should see `ota_init: CTF Addon
  detected` and `ota_actions: Writing to OTA partition 1 from external
  flash...` and also the blue light will blink very quickly during the copy.
- The esp32s3 will reboot itself into `ota_1` after the CTF Addon firmware is
  loaded from the flash.
- Use the command `firmware_select addon` (available in all firmwares) to boot
  into the CTF Addon firmware.
  

## Common errors

```
I (371) ota_init: CTF Addon detected
I (381) ota_init: Initializing external SPI Flash at 20MHz
E (381) memspi: no response

E (381) ota_init: Failed to initialize external Flash: ESP_ERR_INVALID_RESPONSE (0x108)
```
This means you have the CTF Addon connected, but the flash is not detected
which means that `GPIO 12` is grounded and the CTF Addon is not plugged, or
something hardware is broken. The blue LED will blink at 1000ms when this
happens.

```
I (391) ota_write: Found OTA partition 1! size: 1048576
I (401) ota_write: No ota_1 partition firmware detected: ESP_ERR_NOT_FOUND (0x105)
I (411) ota_write: Writing to OTA partition 1 from external flash...
E (751) esp_ota_ops: OTA image has invalid magic byte (expected 0xE9, saw 0x46)
E (751) ota_write: esp_ota_write failed, error=5379
```
This means the external flash (or CTF Addon) has not been provisioned with the
firmware, this can be because the `FLASH_DEST_ADDR` is wrong (it needs to
points where the firmware is in the flash, with a size of
`ota_partition->size`) or something hardware is broken. The blue LED will flash
at 500ms when this happens.

```
error: 'UART_SCLK_RTC' undeclared (first use in this function); did you mean 'UART_SCLK_APB'?
```
This most likely means your target is esp32 and not esp32s3. Use `idf.py
set-target esp32s3` and build again.

```
I (342) ota_init: CTF Addon not detected
E (352) ota_actions: Failed to find subtype 16 partition
E (352) ota_actions: Failed to find subtype 17 partition
```
This most likely mneans that you haven't flkashed a firmware with 2 OTA partition. CHeck in `idf.py menuconfig` under `Partition Table` if you see `Factory app, two OTA definitions`. Also make sure that the `Serial flasher config ` `Flash size` is at 8MB as the two often goes together.

