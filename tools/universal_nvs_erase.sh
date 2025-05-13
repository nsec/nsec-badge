## Erase NVS Memory (both conf/ctf)
esptool.py --chip esp32c3 erase_region 0x370000 0x32000

## If esptool.py doesn't work, you can try running it through python directly - depends on your setup:
# python -m esptool --chip esp32c3 erase_region 0x370000 0x32000