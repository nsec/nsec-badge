# SPDX-FileCopyrightText: 2024 NorthSec
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

factory_reset:
	esptool.py --chip esp32s3 --port /dev/ttyACM0 erase_region 0x9000 0x4000

erase_ota:
	esptool.py --chip esp32s3 --port /dev/ttyACM0 erase_region 0xd000 0x2000

erase_conf:
	esptool.py --chip esp32s3 --port /dev/ttyACM0 erase_region 0x10000 0x100000

erase_ctf:
	esptool.py --chip esp32s3 --port /dev/ttyACM0 erase_region 0x110000 0x100000

erase_addon:
	esptool.py --chip esp32s3 --port /dev/ttyACM0 erase_region 0x210000 0x100000


.PHONY: build flash menuconfig compiledb check check-embedded reuse
