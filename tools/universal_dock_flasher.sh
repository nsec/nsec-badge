# Helper script to flash the non-python dock firmwares for the CTF.
esptool.py --chip esp32s3 --baud 921600 write_flash 0x0 $1

#Example: esptool.py --chip esp32s3 --baud 921600 write_flash 0x0 ./binaries/dock/nsec-badge-dock-qkd.bin

# Similarly, if esptool.py doesn't work, you can try running it through python directly - depends on your setup:
# python -m esptool --chip esp32s3 --baud 921600 write_flash 0x0 $1
# Example: python -m esptool --chip esp32s3 --baud 921600 write_flash 0x0 ./binaries/dock/nsec-badge-dock-qkd.bin

