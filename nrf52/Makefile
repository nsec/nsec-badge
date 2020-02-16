# Override these on the command line, if needed.  For example:
#
#   $ make FLAVOR=conf

FLAVOR ?= ctf
BOARD ?= brain
ifeq ($(origin CC),default)
CC = arm-none-eabi-gcc
endif
SIZE ?= arm-none-eabi-size

# SDK version we use.
SDK_MAJOR_VERSION = 16
SDK_MINOR_VERSION = 0
SDK_PATCH_VERSION = 0
SDK_COMMIT_VERSION = 98a08e2
SDK_VERSION = nRF5_SDK_$(SDK_MAJOR_VERSION).$(SDK_MINOR_VERSION).$(SDK_PATCH_VERSION)_$(SDK_COMMIT_VERSION)
SDK_PATH = nordicsdk-$(SDK_MAJOR_VERSION).$(SDK_MINOR_VERSION).$(SDK_PATCH_VERSION)

USE_SOFTDEVICE = s132

# Base name for a few files.
PROJECT = nsec_nrf52_$(BOARD)_$(FLAVOR)

# Build output location.
BUILD = builds/$(BOARD)-$(FLAVOR)

# Final ELF executable.
ELF = $(BUILD)/$(PROJECT).elf

# Flavor-dependent definitions.
ifeq ($(FLAVOR), ctf)
	FLAVOR_CFLAGS += -DNSEC_HARDCODED_BADGE_CLASS=CTF
	FLAVOR_CFLAGS += -DNSEC_FLAVOR_CTF
else ifeq ($(FLAVOR), conf)
	FLAVOR_CFLAGS += -DNSEC_HARDCODED_BADGE_CLASS=CONF
	FLAVOR_CFLAGS += -DNSEC_FLAVOR_CONF
else ifeq ($(FLAVOR), bar_beacon)
	FLAVOR_CFLAGS += -DNSEC_HARDCODED_BADGE_CLASS=BAR
	FLAVOR_CFLAGS += -DNSEC_FLAVOR_BAR_BEACON
else ifeq ($(FLAVOR), soldering)
	FLAVOR_CFLAGS += -DSOLDERING_TRACK
	FLAVOR_CFLAGS += -DNSEC_HARDCODED_BADGE_CLASS=CTF
	FLAVOR_CFLAGS += -DNSEC_FLAVOR_CTF
else
$(error "Unknown flavor `$(FLAVOR)`")
endif

# Board-dependent definitions
ifeq ($(BOARD), sputnik)
	# One of the oled screen pin is shared with the
	# NFC antenna, make sure it's configured as GPIO
	BOARD_CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS
	BOARD_CFLAGS += -DBOARD_SPUTNIK
else ifeq ($(BOARD), brain)
	BOARD_CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS
	BOARD_CFLAGS += -DBOARD_BRAIN
else ifeq ($(BOARD), pca10040)
	#special flags for the devboard
	BOARD_CFLAGS += -DCONFIG_GPIO_AS_PINRESET
	BOARD_CFLAGS += -DBOARD_PCA10040
	BOARD_CFLAGS += -DBSP_DEFINES_ONLY
else
$(error "Unknown board `$(BOARD)`")
endif

# Main target.
all: $(ELF)

# Our source files to include in the build, adjust if needed.
USER_SRCS += $(wildcard src/app/*.c)
#USER_SRCS += $(wildcard src/ble/*.c)
USER_SRCS += $(wildcard src/drivers/*.c)

# The source files from the SDK to include in the build, adjust if needed.
SDK_C_SRCS += $(SDK_PATH)/components/libraries/atomic/nrf_atomic.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/atomic_fifo/nrf_atfifo.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/balloc/nrf_balloc.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/button/app_button.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/cli/nrf_cli.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/cli/uart/nrf_cli_uart.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/crc32/crc32.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/ringbuf/nrf_ringbuf.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/experimental_section_vars/nrf_section_iter.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/log/src/nrf_log_frontend.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/log/src/nrf_log_str_formatter.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/memobj/nrf_memobj.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/queue/nrf_queue.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/sortlist/nrf_sortlist.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/timer/app_timer2.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/timer/drv_rtc.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/util/app_error_handler_gcc.c
SDK_C_SRCS += $(SDK_PATH)/components/libraries/util/app_util_platform.c
SDK_C_SRCS += $(SDK_PATH)/components/softdevice/common/nrf_sdh.c
SDK_C_SRCS += $(SDK_PATH)/external/fprintf/nrf_fprintf.c
SDK_C_SRCS += $(SDK_PATH)/external/fprintf/nrf_fprintf_format.c
SDK_C_SRCS += $(SDK_PATH)/integration/nrfx/legacy/nrf_drv_uart.c
SDK_C_SRCS += $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_gpiote.c
SDK_C_SRCS += $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_pwm.c
SDK_C_SRCS += $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_saadc.c
SDK_C_SRCS += $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_spim.c
SDK_C_SRCS += $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_uart.c
SDK_C_SRCS += $(SDK_PATH)/modules/nrfx/drivers/src/nrfx_uarte.c
SDK_C_SRCS += $(SDK_PATH)/modules/nrfx/drivers/src/prs/nrfx_prs.c
SDK_C_SRCS += $(SDK_PATH)/modules/nrfx/mdk/system_nrf52.c

SDK_ASM_SRCS += $(SDK_PATH)/modules/nrfx/mdk/gcc_startup_nrf52.S

# Those files come from extracting the sdk.
$(SDK_C_SRCS) $(SDK_ASM_SRCS): $(SDK_PATH)

# The objects derived from our source files.
USER_OBJS = $(USER_SRCS:%.c=$(BUILD)/%.o)

# The objects derived from the SDK source files.
SDK_C_OBJS = $(SDK_C_SRCS:%.c=$(BUILD)/%.o)
SDK_ASM_OBJS = $(SDK_ASM_SRCS:%.S=$(BUILD)/%.o)

ifeq ($(FLAVOR),conf)
# For the gosecure animation, we generate some images that go into the
# external flash, but we don't know the list of the generated images
# files in advance, so we can't hard-code that list in this Makefile.

GEN_GOSECURE_ANIMATION_SEQUENCES_SCRIPT = utils/gosecure/generate.py

# We therefore use this stamp file as a placeholder dependency.  The
# generation of the external flash file depends on it.
$(BUILD)/gosecure-animation-sequences-stamp: $(GEN_GOSECURE_ANIMATION_SEQUENCES_SCRIPT)
	mkdir -p $(BUILD)
	mkdir -p $(BUILD)/gosecure-animation-sequences
	python3 $(GEN_GOSECURE_ANIMATION_SEQUENCES_SCRIPT) utils/gosecure $(BUILD) $(BUILD)/gosecure-animation-sequences
	for f in $(BUILD)/gosecure-animation-sequences/*.png; do \
		echo $$f; \
		python3 $(GEN_IMAGE_SCRIPT) --external-flash $$f `dirname $$f`/`basename -s .png $$f`_bitmap; \
	done
	touch $(BUILD)/gosecure-animation-sequences-stamp

# This source file and header file are generated by the script.
$(BUILD)/gosecure_animation_sequences.h $(BUILD)/gosecure_animation_sequences.c: $(BUILD)/gosecure-animation-sequences-stamp

# This source file includes that generated header file.
$(BUILD)/src/app/gosecure_animation.o: $(BUILD)/gosecure_animation_sequences.h

GEN_OBJS += $(BUILD)/gosecure_animation_sequences.o
endif

ALL_OBJS = $(USER_OBJS) $(SDK_C_OBJS) $(SDK_ASM_OBJS) $(GEN_OBJS)

# Tracking of include file dependencies.  This is so that if you build
# some file `foo.c` which includes `bar.h`, `foo.c` is re-built whenever
# `bar.h` changes.
#
# This is based on this method:
#
#   http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/#tldr
#
# In short, the $(DEPFLAGS) ask gcc to output a file that lists all the files
# that were included in the compilation, in the form of Makefile dependencies.
# These files are included later in this Makefile.
#
# The compilation of source file
#
#   src/app/yo.c
#
# into
#
#   builds/$(BOARD)-$(FLAVOR)/src/app/yo.o
#
# will produce the dependency file
#
#   builds/$(BOARD)-$(FLAVOR)/.deps/src/app/yo.d

DEPDIR = $(BUILD)/.deps
DEPFILES = $(ALL_OBJS:$(BUILD)/%.o=$(DEPDIR)/%.d)
DEPFLAGS = -MF $(DEPDIR)/$*.d -MP -MD

# Libraries to include in the link.
LIBS = $(SDK_PATH)/components/toolchain/cmsis/dsp/GCC/libarm_cortexM4lf_math.a

# Silent rules stuff.
# If V is undefined or V=0 is specified, use the silent/verbose/compact mode.
V ?= 0
ifeq ($(V),0)
ECHO_AS     = @echo "  AS      $@";
ECHO_CC     = @echo "  CC      $@";
ECHO_LD     = @echo "  LD      $@";
ECHO_BMPINT = @echo "  BMPINT  $<";
ECHO_BMPEXT = @echo "  BMPEXT  $<";
ECHO_PACK   = @echo "  PACK    $@";
SILENCE = @
endif

# The images to convert to .h/.c.
#
# Data of internal (INT) bitmaps is going to end up in the application, so is
# compiled as an array in a .c file, with an accompanying .h file that can be
# included to use said data.
#
# Data of external (EXT) bitmaps is going to end up in the flash, so is placed
# in a raw binary file (.bitmapbin).  All these binary files are later
# concatenated to make up the flash content, and a .h file is produced to
# describe the location of each file's content in the flash.
BITMAPS_INT = $(wildcard src/images/*.png)
BITMAPS_INT_HDRS = $(patsubst src/images/%.png,$(BUILD)/images/%_bitmap.h,$(BITMAPS_INT))

BITMAPS_EXT = $(wildcard src/images/external/*.png) $(wildcard src/images/external/$(FLAVOR)/*.png)
BITMAPS_EXT_HDRS = $(patsubst src/images/external/%.png,$(BUILD)/images/external/%_bitmap.h,$(BITMAPS_EXT))
BITMAPS_EXT_BINS = $(patsubst src/images/external/%.png,$(BUILD)/images/external/%_bitmap.bitmapbin,$(BITMAPS_EXT))

BITMAPS_HDRS = $(BITMAPS_INT_HDRS) $(BITMAPS_EXT_HDRS)

GEN_IMAGE_SCRIPT = utils/gen_image.py

# Bitmaps written to the internal flash.
$(BUILD)/images/%_bitmap.h: src/images/%.png
	@mkdir -p $(dir $@)
	$(ECHO_BMPINT)python3 $(GEN_IMAGE_SCRIPT) $< $(basename $@)

# Bitmaps written to the external flash.
$(BUILD)/images/external/%_bitmap.h $(BUILD)/images/external/%_bitmap.bitmapbin: src/images/external/%.png
	@mkdir -p $(dir $@)
	$(ECHO_BMPEXT)python3 $(GEN_IMAGE_SCRIPT) --external-flash $< $(basename $@)

# This one has its own rule because it's rotated.
$(BUILD)/images/font_bitmap.h: src/images/font.png
	@mkdir -p $(dir $@)
	$(ECHO_BMPINT)python3 $(GEN_IMAGE_SCRIPT) -r $< $(basename $@)

# If the gen_image script changes, we want to re-generate the images.
$(BITMAPS_HDRS) $(BITMAPS_EXT_BINS): $(GEN_IMAGE_SCRIPT)

# Rules to generate the external flash content.
#
# Pack files in a single file and generate a "table of contents" as a .h.
EXTERNAL_FLASH_BIN = $(BUILD)/external_flash_$(FLAVOR).flashbin
EXTERNAL_FLASH_HDR = $(BUILD)/external_flash_$(FLAVOR).h

EXTERNAL_FLASH_PACK_SCRIPT = utils/pack_flash.py
EXTERNAL_FLASH_FILES = $(BITMAPS_EXT_BINS)

ifeq ($(FLAVOR), ctf)
EXTERNAL_FLASH_FILES += src/app/flag-external-flash.txt
endif

ifeq ($(FLAVOR), conf)
# Make sure that all the bitmapbin files for the gosecure animation are
# generated before the external flash file is generated.
$(EXTERNAL_FLASH_HDR) $(EXTERNAL_FLASH_BIN): $(BUILD)/gosecure-animation-sequences-stamp

# The bitmapbin files generated for the gosecure animation.  Note that this uses
# shell expansion, so it is evaluated when the command is executed, unlike
# $(wildcard ...).  This is necessary because this list of files is not known
# when this is evaluated.
ADDITIONAL_EXTERNAL_FLASH_FILES = $(BUILD)/gosecure-animation-sequences/*.bitmapbin
endif

$(EXTERNAL_FLASH_HDR) $(EXTERNAL_FLASH_BIN): $(EXTERNAL_FLASH_FILES)
	$(ECHO_PACK)python3 $(EXTERNAL_FLASH_PACK_SCRIPT) $(BUILD)/external_flash $(FLAVOR) $(EXTERNAL_FLASH_FILES) $(ADDITIONAL_EXTERNAL_FLASH_FILES)

$(EXTERNAL_FLASH_HDR) $(EXTERNAL_FLASH_BIN): $(EXTERNAL_FLASH_PACK_SCRIPT)

# Flag that describe the supported CPU features.
MACHINE_FLAGS = -mthumb -mabi=aapcs -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS += $(FLAVOR_CFLAGS) $(BOARD_CFLAGS)
CFLAGS += -g3 -Os
CFLAGS += -std=c99
CFLAGS += $(MACHINE_FLAGS)
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums -fno-omit-frame-pointer -DFLOAT_ABI_HARD -D__HEAP_SIZE=0 -DARM_MATH_CM4
CFLAGS += -DNRF52 -DNRF52832_XXAA
CFLAGS += -DS132 -DSOFTDEVICE_PRESENT

# This tells the Error module of the SDK, in the event of an error, to call our
# app_error_fault_handler with information about the location of the error and
# enter an infinite loop (rather than resetting the MCU).
CFLAGS += -DDEBUG

# Warnings.
CFLAGS += \
	-Wall \
	-Werror \
	-Wno-unused-function \
	-Wno-unused-variable \
	-fmax-errors=1

# Warning flags only enabled for the user code (i.e. our code).
USER_CFLAGS = \
	-Wimplicit \
	-Wold-style-declaration \
	-Wold-style-definition \
	-Wmissing-parameter-type \
	-Wmissing-prototypes \
	-Wstrict-prototypes \
	-Wformat

# Include paths for our code.
CFLAGS += -Iconfig
CFLAGS += -Iinclude
CFLAGS += -Isrc

# For generated header files (e.g. images).
CFLAGS += -I$(BUILD)

# SDK include paths.
CFLAGS += -I$(SDK_PATH)/components
CFLAGS += -I$(SDK_PATH)/components/libraries/atomic_fifo
CFLAGS += -I$(SDK_PATH)/components/libraries/cli
CFLAGS += -I$(SDK_PATH)/components/libraries/experimental_section_vars
CFLAGS += -I$(SDK_PATH)/components/libraries/pwr_mgmt
CFLAGS += -I$(SDK_PATH)/components/ble/ble_services/ble_dis
CFLAGS += -I$(SDK_PATH)/components/ble/common
CFLAGS += -I$(SDK_PATH)/components/ble/nrf_ble_gatt
CFLAGS += -I$(SDK_PATH)/components/ble/peer_manager
CFLAGS += -I$(SDK_PATH)/components/libraries/atomic
CFLAGS += -I$(SDK_PATH)/components/libraries/balloc
CFLAGS += -I$(SDK_PATH)/components/libraries/button
CFLAGS += -I$(SDK_PATH)/components/libraries/cli/uart
CFLAGS += -I$(SDK_PATH)/components/libraries/crc32
CFLAGS += -I$(SDK_PATH)/components/libraries/delay
CFLAGS += -I$(SDK_PATH)/components/libraries/log
CFLAGS += -I$(SDK_PATH)/components/libraries/log/src
CFLAGS += -I$(SDK_PATH)/components/libraries/memobj
CFLAGS += -I$(SDK_PATH)/components/libraries/mutex
CFLAGS += -I$(SDK_PATH)/components/libraries/queue
CFLAGS += -I$(SDK_PATH)/components/libraries/ringbuf
CFLAGS += -I$(SDK_PATH)/components/libraries/scheduler
CFLAGS += -I$(SDK_PATH)/components/libraries/sortlist
CFLAGS += -I$(SDK_PATH)/components/libraries/strerror
CFLAGS += -I$(SDK_PATH)/components/libraries/timer
CFLAGS += -I$(SDK_PATH)/components/libraries/util
CFLAGS += -I$(SDK_PATH)/components/softdevice/common
CFLAGS += -I$(SDK_PATH)/components/softdevice/$(USE_SOFTDEVICE)/headers
CFLAGS += -I$(SDK_PATH)/components/softdevice/$(USE_SOFTDEVICE)/headers/nrf52
CFLAGS += -I$(SDK_PATH)/components/toolchain/cmsis/include
CFLAGS += -I$(SDK_PATH)/integration/nrfx/legacy
CFLAGS += -I$(SDK_PATH)/external/fprintf
CFLAGS += -I$(SDK_PATH)/modules/nrfx/drivers/include
CFLAGS += -I$(SDK_PATH)/modules/nrfx/hal
CFLAGS += -I$(SDK_PATH)/modules/nrfx/mdk
CFLAGS += -I$(SDK_PATH)/modules/nrfx

# Flags used when linking.
LDFLAGS = $(MACHINE_FLAGS)
LDFLAGS += -L $(SDK_PATH)/modules/nrfx/mdk
LDFLAGS += -T src/nsec_badge.ld
LDFLAGS += -Wl,--gc-sections
LDFLAGS += --specs=nano.specs
LDFLAGS += -Wl,-Map=$(BUILD)/$(PROJECT).map

# Download and extract the SDK.
$(SDK_PATH):
	./utils/install-sdk.sh $@ $(SDK_MAJOR_VERSION) $(SDK_MINOR_VERSION) $(SDK_PATCH_VERSION) $(SDK_COMMIT_VERSION)
	# Restart make so that the include of Makefile.common below works and
	# update targets.
	$(MAKE) $(MAKECMDGOALS)

clean-$(SDK_PATH):
	rm -rf ./$(SDK_PATH)
	rm -rf ./$(SDK_VERSION)
	rm -f ./$(SDK_VERSION).zip

install-sdk-doc:
	mkdir -p sdk-doc
	wget -c https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v$(SDK_MAJOR_VERSION).x.x/nRF5_SDK_$(SDK_MAJOR_VERSION).$(SDK_MINOR_VERSION).$(SDK_PATCH_VERSION)_offline_doc.zip \
		-P sdk-doc/
	cd sdk-doc && unzip -n nRF5_SDK_$(SDK_MAJOR_VERSION).$(SDK_MINOR_VERSION).$(SDK_PATCH_VERSION)_offline_doc.zip
	rm sdk-doc/nRF5_SDK_$(SDK_MAJOR_VERSION).$(SDK_MINOR_VERSION).$(SDK_PATCH_VERSION)_offline_doc.zip

# Link the final executable
$(ELF): $(ALL_OBJS)
	$(ECHO_LD)$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)
	$(SILENCE)$(SIZE) $@

# Build our source files.
$(USER_OBJS): | $(SDK_PATH) $(BITMAPS_HDRS) $(EXTERNAL_FLASH_HDR)
$(USER_OBJS): $(BUILD)/%.o: %.c
	@mkdir -p $(@D) $(DEPDIR)/$(dir $*)
	$(ECHO_CC)$(CC) -c -o $@ $< $(CFLAGS) $(USER_CFLAGS) $(DEPFLAGS)

# Build the source files that we have generated.
$(GEN_OBJS): | $(SDK_PATH) $(BITMAPS_HDRS) $(EXTERNAL_FLASH_HDR)
$(GEN_OBJS): $(BUILD)/%.o: $(BUILD)/%.c
	@mkdir -p $(@D) $(DEPDIR)/$(dir $*)
	$(ECHO_CC)$(CC) -c -o $@ $< $(CFLAGS) $(USER_CFLAGS) $(DEPFLAGS)

# Build SDK source files.
$(SDK_C_OBJS): | $(SDK_PATH)
$(SDK_C_OBJS): $(BUILD)/%.o: %.c
	@mkdir -p $(@D) $(DEPDIR)/$(dir $*)
	$(ECHO_CC)$(CC) -c -o $@ $< $(CFLAGS) $(DEPFLAGS)

# Build SDK assembly files.
$(SDK_ASM_OBJS): | $(SDK_PATH)
$(SDK_ASM_OBJS): $(BUILD)/%.o: %.S
	@mkdir -p $(@D) $(DEPDIR)/$(dir $*)
	$(ECHO_AS)$(CC) -c -o $@ $< $(CFLAGS) $(DEPFLAGS)

clean:
	rm -f $(ALL_OBJS) $(BUILD)/$(PROJECT).map $(DEPFILES) $(ELF)

# Include generated .d files.
-include $(DEPFILES)
