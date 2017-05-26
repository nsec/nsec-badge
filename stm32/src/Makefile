##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

ifeq ($(BINARY),stm32_nsec17_ctf)
OBJS += usb_cdc_acm.o
OBJS += usb_hid_kbd.o
OBJS += challenge1.o
OBJS += challenge2.o
OBJS += challenge3.o
OBJS += challenge4.o
OBJS += challenge4_vm.o
OBJS += shell.o
OBJS += scheduler.o
OBJS += usb_common.o
OBJS += circbuf.o
else ifeq ($(BINARY), stm32_nsec17_conf)
LDFLAGS += -L../blackmagic/src
LDLIBS += -lblackmagic
CPPFLAGS += -I../blackmagic/src/include
CPPFLAGS += -I../blackmagic/src/platforms/common
CPPFLAGS += -I../blackmagic/src/platforms/stm32
CPPFLAGS += -I../blackmagic/src/platforms/nsec17
else
$(error Run `make BINARY=stm32_nsec17_ctf` or `make BINARY=stm32_nsec17_conf`)
endif

# Common objects used by both projects
OBJS += tsc.o
OBJS += touch_handler.o

OPENCM3_DIR=../libopencm3
LDSCRIPT = ../nsec17_badge.ld

include ../libopencm3.target.mk
