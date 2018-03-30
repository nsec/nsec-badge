FLAVOR ?= proto
PROJECT_NAME = nsec18_nrf52_$(FLAVOR)
TARGETS=$(PROJECT_NAME)
OUTPUT_DIRECTORY := builds
SDK_PATH = nordicsdk
SDK_VERSION = nRF5_SDK_14.2.0_17b948a

ifeq ($(FLAVOR), ctf_rao)
	CFLAGS += -DNSEC_HARDCODED_BADGE_IDENTITY_NAME=Rao!
	CFLAGS += -DNSEC_HARDCODED_BLE_DEVICE_ID=NSEC1337
	CFLAGS += -DNSEC_IDENTITY_BLE_SERVICE_DISABLE
	CFLAGS += -DNSEC_VULNERABLE_BLE_SERVICE_ENABLE
else ifeq ($(FLAVOR), ctf_namechange)
	CFLAGS += -DNSEC_HARDCODED_BADGE_IDENTITY_NAME="Comrade 31337"
	CFLAGS += -DNSEC_SHOW_FLAG_ON_SUCCESSFUL_NAME_CHANGE
	CFLAGS += -DNSEC_HIDE_SYNC_KEY_IN_SETTINGS
else ifeq ($(FLAVOR), ctf)
	CFLAGS += -DNSEC_HARDCODED_BADGE_CLASS=CTF
else ifeq ($(FLAVOR), speaker)
	CFLAGS += -DNSEC_HARDCODED_BADGE_CLASS=SPEAKER
else ifeq ($(FLAVOR), admin)
	CFLAGS += -DNSEC_HARDCODED_BADGE_CLASS=ADMIN
else ifeq ($(FLAVOR), devboard)
	#special flags for the devboard
	ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
	ASMFLAGS += -DBOARD_PCA10040
	ASMFLAGS += -DBSP_DEFINES_ONLY
	CFLAGS += -DCONFIG_GPIO_AS_PINRESET
	CFLAGS += -DBOARD_PCA10040
	CFLAGS += -DBSP_DEFINES_ONLY
else ifeq ($(FLAVOR), proto)
	# One of the oled screen pin is shared with the
	# NFC antenna, make sure it's configured as GPIO
	CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS
else ifneq ($(FLAVOR), conf)
all:
$(error "Unknown flavor `$(FLAVOR)`")
endif


SRC_FILES  = $(notdir $(wildcard *.c))
SRC_FILES += ble/ble_device.c
SRC_FILES += ble/gap_configuration.c
SRC_FILES += ble/ble_vendor_service.c
SRC_FILES += ble/vendor_service.c
#SRC_FILES += $(wildcard ble/*.c)
SRC_FILES += $(SDK_PATH)/ble/ble_services/ble_dis/ble_dis.c
SRC_FILES += $(SDK_PATH)/ble/ble_services/ble_lbs/ble_lbs.c
SRC_FILES += $(SDK_PATH)/ble/common/ble_advdata.c
SRC_FILES += $(SDK_PATH)/ble/common/ble_conn_params.c
SRC_FILES += $(SDK_PATH)/ble/common/ble_conn_state.c
SRC_FILES += $(SDK_PATH)/ble/common/ble_srv_common.c
SRC_FILES += $(SDK_PATH)/ble/nrf_ble_gatt/nrf_ble_gatt.c
SRC_FILES += $(wildcard $(SDK_PATH)/ble/peer_manager/*.c)
SRC_FILES += $(SDK_PATH)/drivers_nrf/clock/nrf_drv_clock.c
SRC_FILES += $(SDK_PATH)/drivers_nrf/common/nrf_drv_common.c
SRC_FILES += $(SDK_PATH)/drivers_nrf/gpiote/nrf_drv_gpiote.c
SRC_FILES += $(SDK_PATH)/drivers_nrf/power/nrf_drv_power.c
SRC_FILES += $(SDK_PATH)/drivers_nrf/spi_master/nrf_drv_spi.c
SRC_FILES += $(SDK_PATH)/drivers_nrf/spi_slave/nrf_drv_spis.c
SRC_FILES += $(SDK_PATH)/external/fprintf/nrf_fprintf.c
SRC_FILES += $(SDK_PATH)/external/fprintf/nrf_fprintf_format.c
SRC_FILES += $(SDK_PATH)/external/segger_rtt/SEGGER_RTT.c
SRC_FILES += $(SDK_PATH)/external/segger_rtt/SEGGER_RTT_printf.c
SRC_FILES += $(SDK_PATH)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c
SRC_FILES += $(SDK_PATH)/libraries/atomic_fifo/nrf_atfifo.c
SRC_FILES += $(SDK_PATH)/libraries/balloc/nrf_balloc.c
SRC_FILES += $(SDK_PATH)/libraries/button/app_button.c
SRC_FILES += $(SDK_PATH)/libraries/experimental_log/src/nrf_log_backend_rtt.c
SRC_FILES += $(SDK_PATH)/libraries/experimental_log/src/nrf_log_backend_serial.c
SRC_FILES += $(SDK_PATH)/libraries/experimental_log/src/nrf_log_default_backends.c
SRC_FILES += $(SDK_PATH)/libraries/experimental_log/src/nrf_log_frontend.c
SRC_FILES += $(SDK_PATH)/libraries/experimental_log/src/nrf_log_str_formatter.c
SRC_FILES += $(SDK_PATH)/libraries/experimental_memobj/nrf_memobj.c
SRC_FILES += $(SDK_PATH)/libraries/experimental_section_vars/nrf_section_iter.c
SRC_FILES += $(SDK_PATH)/libraries/fds/fds.c
SRC_FILES += $(SDK_PATH)/libraries/fifo/app_fifo.c
SRC_FILES += $(SDK_PATH)/libraries/fstorage/nrf_fstorage.c
SRC_FILES += $(SDK_PATH)/libraries/fstorage/nrf_fstorage_sd.c
SRC_FILES += $(SDK_PATH)/libraries/gpiote/app_gpiote.c
SRC_FILES += $(SDK_PATH)/libraries/scheduler/app_scheduler.c
SRC_FILES += $(SDK_PATH)/libraries/strerror/nrf_strerror.c
SRC_FILES += $(SDK_PATH)/libraries/timer/app_timer.c
SRC_FILES += $(SDK_PATH)/libraries/uart/app_uart_fifo.c
SRC_FILES += $(SDK_PATH)/libraries/util/app_error.c
SRC_FILES += $(SDK_PATH)/libraries/util/app_error_weak.c
SRC_FILES += $(SDK_PATH)/libraries/util/app_util_platform.c
SRC_FILES += $(SDK_PATH)/libraries/util/sdk_mapped_flags.c
# include theses files before the softdevice source code, or strange bugs will occur.
SRC_FILES += $(SDK_PATH)/toolchain/gcc/gcc_startup_nrf52.S
SRC_FILES += $(SDK_PATH)/toolchain/system_nrf52.c
SRC_FILES += $(SDK_PATH)/softdevice/common/nrf_sdh_ble.c
SRC_FILES += $(SDK_PATH)/softdevice/common/nrf_sdh.c
SRC_FILES += $(SDK_PATH)/softdevice/common/nrf_sdh_soc.c


INC_FOLDERS  = .
INC_FOLDERS += $(SDK_PATH)
INC_FOLDERS += $(SDK_PATH)/toolchain
INC_FOLDERS += $(SDK_PATH)/toolchain/gcc
INC_FOLDERS += $(SDK_PATH)/toolchain/cmsis/include
INC_FOLDERS += $(SDK_PATH)/device
INC_FOLDERS += $(SDK_PATH)/softdevice/common
INC_FOLDERS += $(SDK_PATH)/softdevice/s132/headers
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/clock
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/common
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/delay
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/gpiote
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/hal
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/power
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/spi_master
INC_FOLDERS += $(SDK_PATH)/drivers_nrf/spi_slave
INC_FOLDERS += $(SDK_PATH)/libraries/atomic
INC_FOLDERS += $(SDK_PATH)/libraries/atomic_fifo
INC_FOLDERS += $(SDK_PATH)/libraries/balloc/
INC_FOLDERS += $(SDK_PATH)/libraries/button
INC_FOLDERS += $(SDK_PATH)/libraries/experimental_log
INC_FOLDERS += $(SDK_PATH)/libraries/experimental_log/src
INC_FOLDERS += $(SDK_PATH)/libraries/experimental_memobj
INC_FOLDERS += $(SDK_PATH)/libraries/experimental_section_vars
INC_FOLDERS += $(SDK_PATH)/libraries/experimental_section_vars
INC_FOLDERS += $(SDK_PATH)/libraries/fds
INC_FOLDERS += $(SDK_PATH)/libraries/fifo
INC_FOLDERS += $(SDK_PATH)/libraries/fstorage
INC_FOLDERS += $(SDK_PATH)/libraries/gpiote
INC_FOLDERS += $(SDK_PATH)/libraries/scheduler
INC_FOLDERS += $(SDK_PATH)/libraries/strerror
INC_FOLDERS += $(SDK_PATH)/libraries/timer
INC_FOLDERS += $(SDK_PATH)/libraries/uart
INC_FOLDERS += $(SDK_PATH)/libraries/util
INC_FOLDERS += $(SDK_PATH)/ble/common
INC_FOLDERS += $(SDK_PATH)/ble/ble_services/ble_bas
INC_FOLDERS += $(SDK_PATH)/ble/ble_services/ble_dis
INC_FOLDERS += $(SDK_PATH)/ble/nrf_ble_gatt
INC_FOLDERS += $(SDK_PATH)/ble/nrf_ble_gatt/
INC_FOLDERS += $(SDK_PATH)/ble/peer_manager
INC_FOLDERS += $(SDK_PATH)/serialization/common/transport/ser_phy/config
INC_FOLDERS += $(SDK_PATH)/external/fprintf
INC_FOLDERS += $(SDK_PATH)/external/segger_rtt

# Libraries common to all targets
LIB_FILES += \

DEVICE = NRF52
CFLAGS   += -DNRF52 -DNRF52832_XXAA
ASMFLAGS += -DNRF52 -DNRF52832_XXAA

USE_SOFTDEVICE = s132
SOFTDEVICE = $(SDK_PATH)/softdevice/s132/hex/s132_nrf52_5.0.0_softdevice.hex
CFLAGS   += -DS132 -DSOFTDEVICE_PRESENT
ASMFLAGS += -DS132 -DSOFTDEVICE_PRESENT

TEMPLATE_PATH = $(SDK_PATH)/toolchain/gcc/


# Optimization flags
OPTIMIZATIONS = -DDEBUG -g3 -Os

# Enable link time optimization
OPTIMIZATIONS += -flto


# Architecture and ABI flags
ARCH = -mthumb -mabi=aapcs -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16


# C flags common to all targets
CFLAGS += $(OPTIMIZATIONS)
CFLAGS += $(ARCH)
CFLAGS += -Wall #-Werror
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums
# Store the stack frame pointer in a register (why?)
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -D__HEAP_SIZE=0


# Assembler flags common to all targets
ASMFLAGS += $(OPTIMIZATIONS)
ASMFLAGS += $(ARCH)
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -D__HEAP_SIZE=0


# Linker flags
LDFLAGS += $(OPTIMIZATIONS)
LDFLAGS += $(ARCH)
LDFLAGS += -L $(TEMPLATE_PATH) -T nsec_badge.ld
# Store the stack frame pointer in a register (why?)
LDFLAGS += -fno-omit-frame-pointer
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys

TERMINAL = dup

default: bitmaps $(PROJECT_NAME)

$(PROJECT_NAME): $(SDK_PATH)

clean: clean-bitmaps

%_bitmap.c: %.png
	python2 gen_image.py -i $< -o $@

images/font_bitmap.c: images/font.png
	python2 gen_image.py -i $< -o $@ -r

bitmaps: $(patsubst %.png,%_bitmap.c,$(wildcard images/*.png))

clean-bitmaps:
	rm -f images/*.c

$(SDK_PATH):
	./install-sdk.sh $@ $(SDK_VERSION)
	# Restart make so that the include of Makefile.common below works and
	# update targets.
	$(MAKE) $(MAKECMDGOALS)

clean-$(SDK_PATH):
	rm -rf ./$(SDK_PATH)
	rm -rf ./$(SDK_VERSION)
	rm -f ./$(SDK_VERSION).zip

-include $(TEMPLATE_PATH)/Makefile.common
$(foreach target, $(TARGETS), $(call define_target, $(target)))


$(OUTPUT_DIRECTORY)/program.hex: $(PROJECT_NAME)
	mergehex/mergehex -m $(SOFTDEVICE) $(OUTPUT_DIRECTORY)/$<.hex -o $@

# TODO add target to install nrfjprog and mergehex
flash-devboard: $(OUTPUT_DIRECTORY)/program.hex
	@echo Flashing: $<
	nrfjprog/nrfjprog -f nrf52 --eraseall
	nrfjprog/nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/program.hex --sectorerase --verify
	nrfjprog/nrfjprog -f nrf52 --reset

merge: $(OUTPUT_DIRECTORY)/program.hex

sdk_config: $(SDK_PATH)
	java -jar $(SDK_PATH)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar sdk_config.h

.PHONY: bitmaps clean clean-bitmaps clean-$(SDK_PATH) flash-devboard merge sdk_config
