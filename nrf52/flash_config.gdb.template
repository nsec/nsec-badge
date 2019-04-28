# Set the path of your the BlackMagic debugger device. Ex:
# "/dev/stty0" or "/dev/ttyUSB0" (Linux), "/dev/cu.usbmodem012345678" (macOS)
target extended-remote /dev/ttyACM0

# Erase all the data (App and SoftDevice) from the chip before flashing.
set $FORCE_ERASE_ALL = 0

# Change to 1 if you need to load the SoftDevice into the chip. This can be once
# and when SoftDevice version is updated.
set $LOAD_SOFTDEVICE = 0

# Quit gdb after flashing is done.
set $QUIT_AFTER_FLASH = 0

# Use an external blackmagic debugger
set $USE_EXTERNAL_DEBUGGER = 0
