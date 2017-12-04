PROJECT_NAME = nsec17_badge
OUTPUT_DIRECTORY := _build
SDK_PATH = nordicsdk

SRC_FILES  = $(notdir $(wildcard *.c))
SRC_FILES += $(wildcard ble/*.c)
SRC_FILES += $(SDK_PATH)/toolchain/gcc/gcc_startup_nrf51.S
SRC_FILES += $(SDK_PATH)/toolchain/system_nrf51.c
SRC_FILES += $(SDK_PATH)/libraries/timer/app_timer.c
SRC_FILES += $(SDK_PATH)/libraries/scheduler/app_scheduler.c
SRC_FILES += $(SDK_PATH)/libraries/timer/app_timer_appsh.c
SRC_FILES += $(SDK_PATH)/libraries/button/app_button.c
SRC_FILES += $(SDK_PATH)/libraries/fifo/app_fifo.c
SRC_FILES += $(SDK_PATH)/libraries/gpiote/app_gpiote.c
SRC_FILES += $(SDK_PATH)/libraries/uart/app_uart_fifo.c
SRC_FILES += $(SDK_PATH)/libraries/util/app_util_platform.c
SRC_FILES += $(SDK_PATH)/libraries/util/app_error.c
SRC_FILES += $(SDK_PATH)/libraries/util/sdk_mapped_flags.c
SRC_FILES += $(SDK_PATH)/libraries/fds/fds.c
SRC_FILES += $(SDK_PATH)/libraries/fstorage/fstorage.c
SRC_FILES += $(SDK_PATH)/ble/common/ble_srv_common.c
SRC_FILES += $(SDK_PATH)/ble/common/ble_conn_params.c
SRC_FILES += $(SDK_PATH)/ble/common/ble_advdata.c
SRC_FILES += $(SDK_PATH)/ble/common/ble_conn_state.c
SRC_FILES += $(SDK_PATH)/softdevice/common/softdevice_handler/softdevice_handler.c
SRC_FILES += $(SDK_PATH)/drivers_nrf/common/nrf_drv_common.c
SRC_FILES += $(SDK_PATH)/drivers_nrf/spi_slave/nrf_drv_spis.c
SRC_FILES += $(SDK_PATH)/drivers_nrf/spi_master/nrf_drv_spi.c
SRC_FILES += $(SDK_PATH)/drivers_nrf/gpiote/nrf_drv_gpiote.c
SRC_FILES += $(wildcard $(SDK_PATH)/ble/peer_manager/*.c)
SRC_FILES += $(SDK_PATH)/ble/ble_services/ble_dis/ble_dis.c
SRC_FILES += $(SDK_PATH)/ble/ble_services/ble_hids/ble_hids.c
SRC_FILES += $(SDK_PATH)/ble/ble_services/ble_bas/ble_bas.c

INC_FOLDERS  = .
INC_FOLDERS += $(SDK_PATH)
INC_FOLDERS += $(SDK_PATH)/toolchain
INC_FOLDERS += $(SDK_PATH)/toolchain/gcc
INC_FOLDERS += $(SDK_PATH)/toolchain/cmsis/include
INC_FOLDERS += $(SDK_PATH)/device
INC_FOLDERS += $(SDK_PATH)/softdevice/common/softdevice_handler
INC_FOLDERS += $(SDK_PATH)/softdevice/s130/headers
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/common
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/gpiote
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/hal
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/delay
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/spi_master
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/spi_slave
INC_FOLDERS += $(SDK_PATH)/libraries/util
INC_FOLDERS += $(SDK_PATH)/libraries/timer
INC_FOLDERS += $(SDK_PATH)/libraries/button
INC_FOLDERS += $(SDK_PATH)/libraries/scheduler
INC_FOLDERS += $(SDK_PATH)/libraries/fifo
INC_FOLDERS += $(SDK_PATH)/libraries/gpiote
INC_FOLDERS += $(SDK_PATH)/libraries/uart
INC_FOLDERS += $(SDK_PATH)/libraries/log
INC_FOLDERS += $(SDK_PATH)/libraries/log/src
INC_FOLDERS += $(SDK_PATH)/libraries/fds
INC_FOLDERS += $(SDK_PATH)/libraries/fstorage
INC_FOLDERS += $(SDK_PATH)/libraries/experimental_section_vars
INC_FOLDERS += $(SDK_PATH)/ble/common
INC_FOLDERS += $(SDK_PATH)/ble/peer_manager
INC_FOLDERS += $(SDK_PATH)/ble/ble_services/ble_bas
INC_FOLDERS += $(SDK_PATH)/ble/ble_services/ble_dis
INC_FOLDERS += $(SDK_PATH)/ble/ble_services/ble_hids
INC_FOLDERS += $(SDK_PATH)/serialization/common/transport/ser_phy/config

DEVICE = NRF51

USE_SOFTDEVICE = s130
SOFTDEVICE = $(OUTPUT_DIRECTORY)/s130_nrf51_2.0.1_softdevice.hex

TEMPLATE_PATH = $(SDK_PATH)/toolchain/gcc/

CFLAGS += -DDEBUG -g3 -Os
LDFLAGS += -g3
ASMFLAGS += -DDEBUG -g3 -Os

CFLAGS += -DNRF51
CFLAGS += -DNRF51822_QFAA_CA
CFLAGS += -DNRF51822
CFLAGS += -DSPI_MASTER_0_ENABLE
CFLAGS += -DPROD
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DNRF_SD_BLE_API_VERSION=2
CFLAGS += -DS130
CFLAGS += -flto -ffunction-sections -fdata-sections -fno-builtin -fno-omit-frame-pointer -Os
CFLAGS += -mthumb -mabi=aapcs -mcpu=cortex-m0
LDFLAGS += --specs=nano.specs -lc -lnosys -Wl,--gc-sections -fno-omit-frame-pointer -Os
LDFLAGS += -mthumb -mabi=aapcs
LDFLAGS += -mcpu=cortex-m0
LDFLAGS += -L $(TEMPLATE_PATH)
LDFLAGS += -T nsec_badge.ld

ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DNRF51
ASMFLAGS += -DNRF51822
ASMFLAGS += -DS130
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=2
ASMFLAGS += -D__HEAP_SIZE=0

TERMINAL = dup

TARGETS=$(OUTPUT_DIRECTORY)/s130_nrf51_2.0.1_softdevice.elf $(PROJECT_NAME)

.PHONY: bitmaps clean-bitmaps

$(PROJECT_NAME): $(SDK_PATH) bitmaps

clean: clean-bitmaps

%_bitmap.c: %.png
	python2 gen_image.py -i $< -o $@

images/font_bitmap.c: images/font.png
	python2 gen_image.py -i $< -o $@ -r

bitmaps: $(patsubst %.png,%_bitmap.c,$(wildcard images/*.png))

clean-bitmaps:
	rm -f images/*.c

$(SDK_PATH):
	wget http://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v12.x.x/nRF5_SDK_12.1.0_0d23e2a.zip
	unzip nRF5_SDK_12.1.0_0d23e2a.zip components/*
	mv components $@
	patch -p0 -d $@ < nordic-sdk-use-arm-none-eabi-gcc-in-path.patch
	patch -p0 -d $@ < nordic-sdk-makefile-fix-object-depends-on-src.patch
	rm nRF5_SDK_12.1.0_0d23e2a.zip
	# Restart make so that the include of Makefile.common bellow works and
	# update targets.
	$(MAKE) $(MAKECMDGOALS)

-include $(TEMPLATE_PATH)/Makefile.common
$(call define_target, $(PROJECT_NAME))

$(OUTPUT_DIRECTORY)/s130_nrf51_2.0.1_softdevice.hex: $(OUTPUT_DIRECTORY) $(SDK_PATH)
	cp $(SDK_PATH)/softdevice/s130/hex/s130_nrf51_2.0.1_softdevice.hex $@

$(OUTPUT_DIRECTORY)/%.elf: $(OUTPUT_DIRECTORY)/%.hex
	arm-none-eabi-objcopy -I ihex -O elf32-littlearm -B armv5 $< $@
