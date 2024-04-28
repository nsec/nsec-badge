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

.PHONY: build flash menuconfig compiledb check check-embedded reuse
