NorthSec 2023 badge
===================

Brought to you by the Team badge of NorthSec.

## Overview

<p align="center">
  <img src="hw/2023/tie.gif" height="320"> <br>
</p>

## Hardware

The NorthSec 2023 badge is based on
[ESP32 microcontroller](https://www.espressif.com/en/products/socs/esp32)
(ESP32-WROOM-32-N4) which is used to drive several periphery devices:

 - WiFi interface
 - BLE interface
 - [NeoPixel](https://en.wikipedia.org/wiki/Adafruit_Industries#NeoPixel) RGB LEDs

The badge is powered through a USB-C port or through an external battery that
can supply between 3.7V and 6V.

If you wish to hack your badge or create a new one based on the hardware. All
the information [is available here](hw/2023/).

## Building the firmware

The badge firmware is based on
[ESP-IDF](https://www.espressif.com/en/products/sdks/esp-idf) v5.0.1
framework. The exact commit is tracked as a submodule in `esp-idf`, and you
can install it with these commands:

```bash
git clone https://github.com/nsec/nsec-badge.git
cd nsec-badge/
git submodule update --init
./esp-idf/install.sh
```

The installation procedure for your OS may differ a little, please consult the
[documentation website](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#installation-step-by-step)
if you have any difficulties.

Once the installation is complete you can build the firmware and flash it to
the badge:

```bash
source esp-idf/export.sh
cd esp32/
idf.py build flash monitor
```

If for some reason `idf.py` is not able to complete the operation, refer to the
[ESP-IDF documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#step-9-flash-onto-the-device).

## Debugging

To read the logging statements output on the USB serial interface, it is
possible to use `idf.py`'s `monitor` command.

However, you can use your preferred terminal emulator by pointing it to the
`/dev/ttyACM0` device. The badge is configured to output at 115'200 bauds,
without parity, and with one stop bit per character.

Note that electrically resetting the board using the hardware switch will
cause the pseudo-terminal device to disappear, making it hard to read the
logs emitted during boot.

Pulsing the DTR signal (lowering it for 1 second) will cause the board to reset
without affecting the pseudo-terminal device, allowing you to read early-boot
messages. This is supported by most terminal emulators.

## Credits
NorthSec CTF badge 2023 is brought to you by the team work of:

 - [EiNSTeiN-](https://github.com/EiNSTeiN-)
 - [Svieg](https://github.com/Svieg)
 - [jgalar](https://github.com/jgalar)
 - [jmarcil](https://github.com/jmarcil)
 - [lle](https://github.com/lle)
 - [mjeanson](https://github.com/mjeanson)
 - [nyx0](https://github.com/nyx0)

Special thanks to:
 - [20th](https://github.com/20th)
 - [mtartare](https://github.com/mtartare)
