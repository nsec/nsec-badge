FLAVOR ?= devboard
PROJECT_NAME = nsec18_nrf52_$(FLAVOR)
OUTPUT_DIRECTORY := builds
SDK_PATH = nordicsdk

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
else ifneq ($(FLAVOR), conf)
all:
$(error "Unknown flavor `$(FLAVOR)`")
endif

SRC_FILES  = $(notdir $(wildcard *.c))
#SRC_FILES += $(wildcard ble/*.c)
SRC_FILES += $(SDK_PATH)/toolchain/gcc/gcc_startup_nrf52.S
SRC_FILES += $(SDK_PATH)/toolchain/system_nrf52.c
SRC_FILES += $(SDK_PATH)/libraries/timer/app_timer.c
SRC_FILES += $(SDK_PATH)/libraries/scheduler/app_scheduler.c
SRC_FILES += $(SDK_PATH)/libraries/button/app_button.c
SRC_FILES += $(SDK_PATH)/libraries/fifo/app_fifo.c
SRC_FILES += $(SDK_PATH)/libraries/gpiote/app_gpiote.c
SRC_FILES += $(SDK_PATH)/libraries/uart/app_uart_fifo.c
SRC_FILES += $(SDK_PATH)/libraries/util/app_util_platform.c
SRC_FILES += $(SDK_PATH)/libraries/util/app_error.c
SRC_FILES += $(SDK_PATH)/libraries/util/sdk_mapped_flags.c
#SRC_FILES += $(SDK_PATH)/libraries/fds/fds.c
SRC_FILES += $(SDK_PATH)/libraries/fstorage/nrf_fstorage.c
SRC_FILES += $(SDK_PATH)/ble/common/ble_srv_common.c
SRC_FILES += $(SDK_PATH)/ble/common/ble_conn_params.c
SRC_FILES += $(SDK_PATH)/ble/common/ble_advdata.c
#SRC_FILES += $(SDK_PATH)/ble/common/ble_conn_state.c
SRC_FILES += $(SDK_PATH)/softdevice/common/nrf_sdh.c
SRC_FILES += $(SDK_PATH)/softdevice/common/nrf_sdh_soc.c
SRC_FILES += $(SDK_PATH)/softdevice/common/nrf_sdh_ble.c
SRC_FILES += $(SDK_PATH)/drivers_nrf/common/nrf_drv_common.c
SRC_FILES += $(SDK_PATH)/drivers_nrf/spi_slave/nrf_drv_spis.c
SRC_FILES += $(SDK_PATH)/drivers_nrf/spi_master/nrf_drv_spi.c
SRC_FILES += $(SDK_PATH)/drivers_nrf/gpiote/nrf_drv_gpiote.c
#SRC_FILES += $(wildcard $(SDK_PATH)/ble/peer_manager/*.c)
#SRC_FILES += $(SDK_PATH)/ble/ble_services/ble_dis/ble_dis.c
#SRC_FILES += $(SDK_PATH)/ble/ble_services/ble_hids/ble_hids.c
#SRC_FILES += $(SDK_PATH)/ble/ble_services/ble_bas/ble_bas.c
#SRC_FILES += $(SDK_PATH)/libraries/strerror/
SRC_FILES += $(SDK_PATH)/libraries/experimental_section_vars/nrf_section_iter.c
SRC_FILES += $(SDK_PATH)/ble/nrf_ble_gatt/nrf_ble_gatt.c
SRC_FILES += $(SDK_PATH)/ble/ble_services/ble_lbs/ble_lbs.c
SRC_FILES += $(SDK_PATH)/libraries/experimental_log/src/nrf_log_backend_rtt.c
SRC_FILES += $(SDK_PATH)/external/segger_rtt/SEGGER_RTT.c
SRC_FILES += $(SDK_PATH)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c
SRC_FILES += $(SDK_PATH)/external/segger_rtt/SEGGER_RTT_printf.c
SRC_FILES += $(SDK_PATH)/external/fprintf/nrf_fprintf.c
SRC_FILES += $(SDK_PATH)/external/fprintf/nrf_fprintf_format.c
SRC_FILES += $(SDK_PATH)/libraries/experimental_log/src/nrf_log_frontend.c
SRC_FILES += $(SDK_PATH)/libraries/experimental_memobj/nrf_memobj.c
SRC_FILES += $(SDK_PATH)/libraries/balloc/nrf_balloc.c
SRC_FILES += $(SDK_PATH)/libraries/experimental_log/src/nrf_log_default_backends.c
SRC_FILES += $(SDK_PATH)/libraries/experimental_log/src/nrf_log_backend_serial.c
SRC_FILES += $(SDK_PATH)/libraries/experimental_log/src/nrf_log_str_formatter.c
SRC_FILES += $(SDK_PATH)/libraries/strerror/nrf_strerror.c

INC_FOLDERS  = .
INC_FOLDERS += $(SDK_PATH)
INC_FOLDERS += $(SDK_PATH)/toolchain
INC_FOLDERS += $(SDK_PATH)/toolchain/gcc
INC_FOLDERS += $(SDK_PATH)/toolchain/cmsis/include
INC_FOLDERS += $(SDK_PATH)/device
INC_FOLDERS += $(SDK_PATH)/softdevice/common
INC_FOLDERS += $(SDK_PATH)/softdevice/s132/headers
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
INC_FOLDERS += $(SDK_PATH)/libraries/experimental_log
INC_FOLDERS += $(SDK_PATH)/libraries/experimental_log/src
#INC_FOLDERS += $(SDK_PATH)/libraries/fds
INC_FOLDERS += $(SDK_PATH)/libraries/fstorage
INC_FOLDERS += $(SDK_PATH)/libraries/experimental_section_vars
INC_FOLDERS += $(SDK_PATH)/libraries/atomic
INC_FOLDERS += $(SDK_PATH)/libraries/atomic_fifo
INC_FOLDERS += $(SDK_PATH)/ble/common
#INC_FOLDERS += $(SDK_PATH)/ble/peer_manager
#INC_FOLDERS += $(SDK_PATH)/ble/ble_services/ble_bas
#INC_FOLDERS += $(SDK_PATH)/ble/ble_services/ble_dis
#INC_FOLDERS += $(SDK_PATH)/ble/ble_services/ble_hids
INC_FOLDERS += $(SDK_PATH)/serialization/common/transport/ser_phy/config
INC_FOLDERS += $(SDK_PATH)/libraries/strerror
INC_FOLDERS += $(SDK_PATH)/libraries/experimental_section_vars
INC_FOLDERS += $(SDK_PATH)/ble/ble_services/ble_lbs
INC_FOLDERS += $(SDK_PATH)/ble/nrf_ble_gatt/
INC_FOLDERS += $(SDK_PATH)/libraries/timer/
INC_FOLDERS += $(SDK_PATH)/libraries/experimental_memobj
INC_FOLDERS += $(SDK_PATH)/libraries/balloc/
INC_FOLDERS += $(SDK_PATH)/external/fprintf
INC_FOLDERS += $(SDK_PATH)/external/segger_rtt
INC_FOLDERS += $(SDK_PATH)/libraries/strerror

DEVICE = NRF52

USE_SOFTDEVICE = s132
SOFTDEVICE = $(OUTPUT_DIRECTORY)/s132_nrf52_5.0.0_softdevice.hex

TEMPLATE_PATH = $(SDK_PATH)/toolchain/gcc/

CFLAGS += -DDEBUG -g3 -Os
LDFLAGS += -g3
ASMFLAGS += -DDEBUG -g3 -Os

#CFLAGS += -DNRF51822_QFAA_CA
#CFLAGS += -DNRF51822
#CFLAGS += -DSPI_MASTER_0_ENABLE
#CFLAGS += -DPROD
CFLAGS += -DSOFTDEVICE_PRESENT
#CFLAGS += -DBLE_STACK_SUPPORT_REQD
#CFLAGS += -DNRF_SD_BLE_API_VERSION=2
CFLAGS += -DS132
CFLAGS += -flto -ffunction-sections -fdata-sections -fno-builtin -fno-omit-frame-pointer -Os
CFLAGS += -mthumb -mabi=aapcs -mcpu=cortex-m4
LDFLAGS += --specs=nano.specs -lc -lnosys -Wl,--gc-sections -fno-omit-frame-pointer -Os
LDFLAGS += -mthumb -mabi=aapcs
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -L $(TEMPLATE_PATH)
LDFLAGS += -T nsec_badge.ld

CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -DNRF52
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16

ASMFLAGS += -x assembler-with-cpp
#ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DSOFTDEVICE_PRESENT
#ASMFLAGS += -DNRF51822
ASMFLAGS += -DS132
#ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
#ASMFLAGS += -DNRF_SD_BLE_API_VERSION=2
ASMFLAGS += -D__HEAP_SIZE=0
ASMFLAGS += -DNRF52

TERMINAL = dup

TARGETS=$(OUTPUT_DIRECTORY)/s132_nrf52_5.0.0_softdevice.elf $(PROJECT_NAME)

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
	wget http://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v14.x.x/nRF5_SDK_14.2.0_17b948a.zip
	unzip nRF5_SDK_14.2.0_17b948a.zip nRF5_SDK_14.2.0_17b948a/components/*
	unzip nRF5_SDK_14.2.0_17b948a.zip nRF5_SDK_14.2.0_17b948a/external/* 
	mv nRF5_SDK_14.2.0_17b948a/components .
	mv nRF5_SDK_14.2.0_17b948a/external .
	mv components $@
	mv external $@
	./nordic-sdk-use-arm-none-eabi-gcc-in-path.patch
	unzip -n -p nRF5_SDK_14.2.0_17b948a.zip nRF5_SDK_14.2.0_17b948a/documentation/nRF5_Nordic_license.txt > nordicsdk_licence_agreement.txt
	unzip -n -p nRF5_SDK_14.2.0_17b948a.zip nRF5_SDK_14.2.0_17b948a/documentation/nRF5_Dynastream_license.txt > dynastream_licence_agreement.txt
	cp $@/softdevice/s132/doc/s132_nrf52_5.0.0_licence-agreement.txt .
	rm nRF5_SDK_14.2.0_17b948a.zip
	rm -d nRF5_SDK_14.2.0_17b948a
	# Restart make so that the include of Makefile.common below works and
	# update targets.
	$(MAKE) $(MAKECMDGOALS)

-include $(TEMPLATE_PATH)/Makefile.common
$(call define_target, $(PROJECT_NAME))

$(OUTPUT_DIRECTORY)/s132_nrf52_5.0.0_softdevice.hex: $(OUTPUT_DIRECTORY) $(SDK_PATH)
	cp $(SDK_PATH)/softdevice/s132/hex/s132_nrf52_5.0.0_softdevice.hex $@

$(OUTPUT_DIRECTORY)/%.elf: $(OUTPUT_DIRECTORY)/%.hex
	arm-none-eabi-objcopy -I ihex -O elf32-littlearm -B armv5 $< $@
	
flash: $(PROJECT_NAME) $(OUTPUT_DIRECTORY)/s132_nrf52_5.0.0_softdevice.hex
	@echo Flashing: $<
	nrfjprog/nrfjprog -f nrf52 --eraseall
	mergehex/mergehex -m $(OUTPUT_DIRECTORY)/s132_nrf52_5.0.0_softdevice.hex $(OUTPUT_DIRECTORY)/$<.hex -o $(OUTPUT_DIRECTORY)/program.hex
	nrfjprog/nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/program.hex --sectorerase --verify
	nrfjprog/nrfjprog -f nrf52 --reset
