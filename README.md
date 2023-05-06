NorthSec 2023 badge
===================

Brought to you by the Team badge of NorthSec.

## Overview

<p align="center">
  <img src="hw/2023/badges.jpeg" height="320"> <br>
  <i>TODO: image & caption.</i>
</p>

## Hardware

The NorthSec 2023 badge is based on [ESP32 microcontroller](https://www.espressif.com/en/products/socs/esp32)
which is used to drive several periphery devices:

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
