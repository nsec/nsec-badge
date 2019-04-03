#!/usr/bin/env python3

# This is a script used to stress test the CLI over UART.  It sends a lot of
# times the "nsec" command, a lot more than enough to fill the ring buffer
# managed by nrf_cli_uart, which is written to by the UART irq handler and read
# by nrf_cli_process in the main loop.  Even though we expect to drop many
# input bytes (we send them much faster than the chip is able to process them),
# the system should be able to recover, and the CLI should stay responsive.
#
# So, to stress test, run this script:
#
#   python3 spam-uart.py
#
# Once it has stopped receiving data from the board, quit it (ctrl-C) and open
# your favorite terminal emulator:
#
#   picocom --baud 115200 --omap delbs  /dev/ttyACM1
#
# You should still be able to type and use the CLI.  It should also be possible
# to run this script multiple times in a row and have the badge keep responding
# to the "nsec" commands.

import serial

def main():
    with serial.Serial('/dev/ttyACM1', 115200) as ser:
        for i in range(500):
            ser.write(b'nsec\r')

        while True:
            l = ser.readline()
            print(l)


if __name__ == '__main__':
    main()
