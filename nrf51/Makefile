# List all source files the application uses.
APPLICATION_SRCS  = $(notdir $(wildcard *.c))

APPLICATION_SRCS += app_timer.c
APPLICATION_SRCS += app_button.c
APPLICATION_SRCS += app_scheduler.c
APPLICATION_SRCS += app_fifo.c
APPLICATION_SRCS += app_gpiote.c
APPLICATION_SRCS += app_uart_fifo.c
APPLICATION_SRCS += ble_srv_common.c
APPLICATION_SRCS += ble_conn_params.c
APPLICATION_SRCS += ble_advdata.c
APPLICATION_SRCS += nrf_delay.c
APPLICATION_SRCS += softdevice_handler.c
APPLICATION_SRCS += spi_master.c
APPLICATION_SRCS += ble_dis.c

APPLICATION_SRCS += $(notdir $(wildcard ble/*.c))
SOURCE_PATHS += ble

APPLICATION_SRCS += nordicsdk/Source/ble/ble_services/ble_hids.c
APPLICATION_SRCS += nordicsdk/Source/ble/ble_services/ble_bas.c
APPLICATION_SRCS += nordicsdk/Source/spi_slave/spi_slave.c
SOURCE_PATHS += nordicsdk/Source/ble/ble_services/
SOURCE_PATHS += nordicsdk/Source/spi_slave/

PROJECT_NAME = nsec16_badge

LIBRARY_PATHS += "."

DEVICE = NRF51
BOARD = BOARD_PCA10000
#SEGGER_SERIAL =

SDK_PATH = nordicsdk/

# Using s110 softdevice with pach for version 7
USE_SOFTDEVICE = s110
SOFTDEVICE = $(SDK_PATH)s110_nrf51822_7.0.0/s110_nrf51822_7.0.0_softdevice.hex

TEMPLATE_PATH = nrf51-pure-gcc-setup/template/
#TOOLCHAIN_PATH = /opt/gcc-arm-none-eabi/bin/
#TOOLCHAIN_PATH = /usr/bin/
#TOOLCHAIN_PATH =

CFLAGS += -DDEBUG -g3 -Os
LDFLAGS += -g3
ASMFLAGS += -DDEBUG -g3 -Os

CFLAGS += -DNRF51822_QFAA_CA -DSPI_MASTER_0_ENABLE -D__HEAP_SIZE=0 -D__STACK_SIZE=2048 -D PROD
CFLAGS += -flto -ffunction-sections -fdata-sections -fno-builtin -fno-omit-frame-pointer -Os
LDFLAGS += --specs=nano.specs -lc -lnosys -Wl,--gc-sections -fno-omit-frame-pointer -Os

GDB_PORT_NUMBER = 2331
TERMINAL = dup

include $(TEMPLATE_PATH)Makefile

%_bitmap.c: %.png
	python gen_image.py -i $< -o $@

bitmaps: $(patsubst %.png,%_bitmap.c,$(wildcard images/*.png))

$(SDK_PATH):
	wget http://developer.nordicsemi.com/nRF5_SDK/nRF51_SDK_v6.x.x/nrf51_sdk_v6_1_0_b2ec2e6.zip
	unzip nrf51_sdk_v6_1_0_b2ec2e6.zip nrf51822/*
	mv nrf51822 $(SDK_PATH)
	rm nrf51_sdk_v6_1_0_b2ec2e6.zip

_build/s110_nrf51822_7.0.0_softdevice.hex:
	mkdir -p _build
	wget http://www.nordicsemi.com/eng/nordic/download_resource/48420/8/56824997 -O s110_nrf51822_7.0.0.zip
	unzip -n s110_nrf51822_7.0.0.zip s110_nrf51822_7.0.0_licence_agreement.pdf
	unzip -n s110_nrf51822_7.0.0.zip -d _build/ s110_nrf51822_7.0.0_softdevice.hex
	rm s110_nrf51822_7.0.0.zip
	echo "Please read s110_nrf51822_7.0.0_licence_agreement.pdf"

_build/s110_nrf51822_7.0.0.elf: _build/s110_nrf51822_7.0.0_softdevice.hex
	arm-none-eabi-objcopy -I ihex -O elf32-littlearm \
		_build/s110_nrf51822_7.0.0_softdevice.hex \
		_build/s110_nrf51822_7.0.0_softdevice.elf

gdb: debug-gdbinit
	$(TERMINAL) "$(JLINKGDBSERVER) -port $(GDB_PORT_NUMBER)"

