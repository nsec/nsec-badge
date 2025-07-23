# SPDX-FileCopyrightText: 2025 NorthSec
#
# SPDX-License-Identifier: MIT

ENV = conference

# Uncomment for verbose build
#VERBOSE = -v

all: build

build:
	pio run -e $(ENV) $(VERBOSE)

flash:
	pio run -e $(ENV) $(VERBOSE) -t upload

menuconfig:
	pio run -e $(ENV) $(VERBOSE) -t menuconfig

compiledb:
	pio run -e $(ENV) $(VERBOSE) -t compiledb

check:
	pio test -e native_tests $(VERBOSE)

check-embedded:
	pio test -e embedded_tests $(VERBOSE)

monitor:
	pio device monitor --filter=direct

reuse:
	reuse lint

# requires `pip install esptool` in venv
factory_reset:
	esptool.py --chip esp32c3 --port /dev/ttyACM0 erase_region 0x310000 0x100000

erase_ota:
	esptool.py --chip esp32c3 --port /dev/ttyACM0 erase_region 0xd000 0x2000

erase_conf:
	esptool.py --chip esp32c3 --port /dev/ttyACM0 erase_region 0x10000 0x180000

erase_ctf:
	esptool.py --chip esp32c3 --port /dev/ttyACM0 erase_region 0x190000 0x180000

erase_nvs:
	esptool.py --chip esp32c3 --port /dev/ttyACM0 erase_region 0x370000 0x32000

.PHONY: build flash menuconfig compiledb check check-embedded reuse
