## Dual Flasher that adds Conf FW to factory partition and CTF FW to OTA partition 
 
esptool.py --chip esp32c3 --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x0 .\binaries\ctf\bootloader.bin 0x8000 .\binaries\ctf\partitions.bin 0xd000 .\binaries\ctf\ota_data_initial.bin 0x10000 .\binaries\ctf\conf-firmware.bin 0x190000 .\binaries\ctf\ctf-firmware.bin

## if esptool.py doesn't work, you can try running it through python directly - depends on your setup:
# python -m esptool --chip esp32c3 --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x0 .\binaries\ctf\bootloader.bin 0x8000 .\binaries\ctf\partitions.bin 0xd000 .\binaries\ctf\ota_data_initial.bin 0x10000 .\binaries\ctf\conf-firmware.bin 0x190000 .\binaries\ctf\ctf-firmware.bin