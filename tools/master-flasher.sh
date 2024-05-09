#!/bin/bash

# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

set -eu

#debug
#set -x

# Path to the firmware directory
FIRMWARE_DIR="binaries/conference"

BOOTLOADER_BIN="${FIRMWARE_DIR}/bootloader.bin"
BOOTLOADER_ADDR="0x0"
PARTITIONS_BIN="${FIRMWARE_DIR}/partitions.bin"
PARTITIONS_ADDR="0x8000"
OTA_DATA_BIN="${FIRMWARE_DIR}/ota_data_initial.bin"
OTA_DATA_ADDR="0xd000"
FIRMWARE_BIN="${FIRMWARE_DIR}/firmware.bin"
FIRMWARE_ADDR="0x10000"

ESP_TYPE="esp32s3"
BAUD_RATE="460800"

# Fixed path to the usb-serial device per usb port
# Can be found with 'udevadm info /dev/ttyACMX'
USB_PORT0="/dev/serial/by-path/pci-0000:00:14.0-usb-0:1.4:1.0"
USB_PORT1="/dev/serial/by-path/pci-0000:00:14.0-usb-0:1.3:1.0"
USB_PORT2="/dev/serial/by-path/pci-0000:00:14.0-usb-0:1.2:1.0"
USB_PORT3="/dev/serial/by-path/pci-0000:00:14.0-usb-0:1.1:1.0"

# Set the path to esptool.py
ESPTOOL="$HOME/Git/Nsec/pio/bin/esptool.py"


check_firmware() {
    if [ ! -f "$BOOTLOADER_BIN" ]; then
        echo "Missing firmware file: $BOOTLOADER_BIN"
        exit 1
    fi
    if [ ! -f "$PARTITIONS_BIN" ]; then
        echo "Missing firmware file: $PARTITIONS_BIN"
        exit 1
    fi
    if [ ! -f "$OTA_DATA_BIN" ]; then
        echo "Missing firmware file: $OTA_DATA_BIN"
        exit 1
    fi
    if [ ! -f "$FIRMWARE_BIN" ]; then
        echo "Missing firmware file: $FIRMWARE_BIN"
        exit 1
    fi
}

check_esptool() {
    if ! command -v "$ESPTOOL" >/dev/null; then
        echo "Set the PATH to the esptool.py binary in \$ESPTOOL"
        exit 1
    fi
}

flash_loop() {
    local usb_port=$1

    check_esptool
    check_firmware

    while true; do
        if ! test -e "$usb_port"; then
            echo "Waiting for badge on: $usb_port"
            sleep 15
            continue
        fi

        echo "Erase all flash on $usb_port"
        if ! "$ESPTOOL" --chip "$ESP_TYPE" --port "$usb_port" erase_flash; then
            echo "Failed to erase all flash. Press any key to retry."
            read -r
            continue
        fi

        echo "Flashing firmware on $usb_port"
        if ! "$ESPTOOL" \
	            --chip "$ESP_TYPE" \
	            --port "$usb_port" \
	            --baud "$BAUD_RATE" \
	            --before default_reset \
	            --after hard_reset \
	            write_flash \
	            -z \
	            --flash_mode dio \
	            --flash_freq 80m \
	            --flash_size 8MB \
                "$BOOTLOADER_ADDR" "$BOOTLOADER_BIN" \
                "$PARTITIONS_ADDR" "$PARTITIONS_BIN" \
                "$OTA_DATA_ADDR" "$OTA_DATA_BIN" \
                "$FIRMWARE_ADDR" "$FIRMWARE_BIN"; then

            echo "Failed to flash firmare. Press any key to retry."
            read -r
            continue
        fi

        echo "Flash successful. Press any key to exit."
        read -r
        exit 0
    done
}

master_flasher() {
    local session="master-flasher"
    local window='Master Flasher'

    if ! command -v tmux >/dev/null; then
        echo "This script requires tmux in the PATH."
        exit 1
    fi

    check_esptool
    check_firmware

    # Create tmux session
    tmux new-session -d -s $session -n "$window"

    # Create panes
    tmux split-window -h -l 50% -t "$session:$window.0"
    tmux split-window -h -l 50% -t "$session:$window.0"
    tmux split-window -h -l 50% -t "$session:$window.2"

    # Start flash script in each pane
    tmux send-keys -t "$session:$window.0" "${BASH_SOURCE[0]} flash0" Enter
    tmux send-keys -t "$session:$window.1" "${BASH_SOURCE[0]} flash1" Enter
    tmux send-keys -t "$session:$window.2" "${BASH_SOURCE[0]} flash2" Enter
    tmux send-keys -t "$session:$window.3" "${BASH_SOURCE[0]} flash3" Enter

    # Attach to tmux session
    tmux a -t "$session:$window"

    exit $?
}

case ${1:-} in
flash0)
    flash_loop "${USB_PORT0}"
    ;;
flash1)
    flash_loop "${USB_PORT1}"
    ;;
flash2)
    flash_loop "${USB_PORT2}"
    ;;
flash3)
    flash_loop "${USB_PORT3}"
    ;;
master)
    master_flasher
    ;;
*)
    echo "Usage: master-flasher [command]"
    echo "  master  Flash on all 4 ports"
    echo "  flash0  Flash on port 0"
    echo "  flash1  Flash on port 1"
    echo "  flash2  Flash on port 2"
    echo "  flash3  Flash on port 3"
    exit 0
    ;;
esac

# vim: expandtab tabstop=4 shiftwidth=4