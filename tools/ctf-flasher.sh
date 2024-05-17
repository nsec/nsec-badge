#!/bin/bash

# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

set -eu

#debug
#set -x

export IDF_PATH=/home/mjeanson/.platformio/packages/framework-espidf

# Path to the firmware directory
FIRMWARE_DIR="binaries/ctf"

#OTA_DATA_BIN="${FIRMWARE_DIR}/ota_data_initial.bin"
#OTA_DATA_ADDR="0xd000"
FIRMWARE_BIN="${FIRMWARE_DIR}/firmware.bin"
FIRMWARE_ADDR="0x110000"

ESP_TYPE="esp32s3"
BAUD_RATE="460800"

# Fixed path to the usb-serial device per usb port
# Can be found with 'udevadm info /dev/ttyACMX'
USB_PORT0="/dev/serial/by-path/pci-0000:00:14.0-usb-0:4:1.0"
USB_PORT1="/dev/serial/by-path/pci-0000:00:14.0-usb-0:1.3:1.0"
USB_PORT2="/dev/serial/by-path/pci-0000:00:14.0-usb-0:1.2:1.0"
USB_PORT3="/dev/serial/by-path/pci-0000:00:14.0-usb-0:1.1:1.0"

USB_PORT4="/dev/serial/by-path/pci-0000:00:14.0-usb-0:2.4:1.0"
USB_PORT5="/dev/serial/by-path/pci-0000:00:14.0-usb-0:2.3:1.0"
USB_PORT6="/dev/serial/by-path/pci-0000:00:14.0-usb-0:2.2:1.0"
USB_PORT7="/dev/serial/by-path/pci-0000:00:14.0-usb-0:2.1:1.0"

# Set the path to esptool.py
ESPTOOL="$HOME/Git/Nsec/pio/bin/esptool.py"
OTATOOL="$HOME/.platformio/packages/framework-espidf/components/app_update/otatool.py"


check_firmware() {
    #if [ ! -f "$OTA_DATA_BIN" ]; then
    #    echo "Missing firmware file: $OTA_DATA_BIN"
    #    exit 1
    #fi
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
            sleep 5
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
                "$FIRMWARE_ADDR" "$FIRMWARE_BIN"; then

            echo "Failed to flash firmare. Press any key to retry."
            read -r
            continue
        fi

        if ! python "$OTATOOL" \
                --port "$usb_port" \
                switch_ota_partition \
                --name ota_0; then
            echo "Failed to switch ota partition. Press any key to retry."
            read -r
            continue
        fi

        echo "Flash successful. Press any key to exit."
        read -r
        exit 0
    done
}

master_flasher() {
    local session="ctf-flasher"
    local window='CTF Flasher'

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

    tmux split-window -v -l 50% -t "$session:$window.0"
    tmux split-window -v -l 50% -t "$session:$window.2"
    tmux split-window -v -l 50% -t "$session:$window.4"
    tmux split-window -v -l 50% -t "$session:$window.6"

    # Start flash script in each pane
    tmux send-keys -t "$session:$window.0" "${BASH_SOURCE[0]} flash0" Enter
    tmux send-keys -t "$session:$window.1" "${BASH_SOURCE[0]} flash1" Enter
    tmux send-keys -t "$session:$window.2" "${BASH_SOURCE[0]} flash2" Enter
    tmux send-keys -t "$session:$window.3" "${BASH_SOURCE[0]} flash3" Enter
    tmux send-keys -t "$session:$window.4" "${BASH_SOURCE[0]} flash4" Enter
    tmux send-keys -t "$session:$window.5" "${BASH_SOURCE[0]} flash5" Enter
    tmux send-keys -t "$session:$window.6" "${BASH_SOURCE[0]} flash6" Enter
    tmux send-keys -t "$session:$window.7" "${BASH_SOURCE[0]} flash7" Enter

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
flash4)
    flash_loop "${USB_PORT4}"
    ;;
flash5)
    flash_loop "${USB_PORT5}"
    ;;
flash6)
    flash_loop "${USB_PORT6}"
    ;;
flash7)
    flash_loop "${USB_PORT7}"
    ;;
master)
    master_flasher
    ;;
*)
    echo "Usage: ctf-flasher [command]"
    echo "  ctf  Flash on all 4 ports"
    echo "  flash0  Flash on port 0"
    echo "  flash1  Flash on port 1"
    echo "  flash2  Flash on port 2"
    echo "  flash3  Flash on port 3"
    exit 0
    ;;
esac

# vim: expandtab tabstop=4 shiftwidth=4
