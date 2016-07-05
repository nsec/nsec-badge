# Adapted from https://github.com/stv0g/stm32cube-gcc/

# A name common to all output files (elf, map, hex, bin, lst)
TARGET     = build/nsec16_badge_stm32

# MCU family and type in various capitalizations o_O
MCU_LC     = stm32f072xb
MCU_MC     = STM32F072xB
MCU_UC     = STM32F072CB

# Sources
SRCS	= main.c
SRCS	+= system_stm32f0xx.c
SRCS	+= stm32f0xx_hal_msp.c
SRCS	+= stm32f0xx_it.c
SRCS	+= usbd_cdc_if.c
SRCS	+= usbd_conf.c
SRCS	+= usbd_desc.c
SRCS	+= usb_device.c

# Basic HAL libraries
SRCS	+= stm32f0xx_hal.c
SRCS	+= stm32f0xx_hal_cortex.c
SRCS	+= stm32f0xx_hal_dma.c
SRCS	+= stm32f0xx_hal_flash.c
SRCS	+= stm32f0xx_hal_flash_ex.c
SRCS	+= stm32f0xx_hal_gpio.c
SRCS	+= stm32f0xx_hal_i2c.c
SRCS	+= stm32f0xx_hal_i2c_ex.c
SRCS	+= stm32f0xx_hal_iwdg.c
SRCS	+= stm32f0xx_hal_pcd.c
SRCS	+= stm32f0xx_hal_pcd_ex.c
SRCS	+= stm32f0xx_hal_pwr.c
SRCS	+= stm32f0xx_hal_pwr_ex.c
SRCS	+= stm32f0xx_hal_rcc.c
SRCS	+= stm32f0xx_hal_rcc_ex.c
SRCS	+= stm32f0xx_hal_spi.c
SRCS	+= stm32f0xx_hal_spi_ex.c
SRCS	+= stm32f0xx_hal_tim.c
SRCS	+= stm32f0xx_hal_tim_ex.c
SRCS	+= stm32f0xx_hal_tsc.c
SRCS	+= stm32f0xx_hal_uart.c
SRCS	+= stm32f0xx_hal_uart_ex.c
SRCS	+= stm32f0xx_hal_wwdg.c

# USB library
SRCS	+= usbd_cdc.c
SRCS	+= usbd_ctlreq.c
SRCS	+= usbd_core.c
SRCS	+= usbd_ioreq.c

# CUBE library

CUBE_DIR	= cube
CUBE_URL	= http://www.st.com/resource/en/firmware/stm32cubef0.zip

# Directories

HAL_DIR		= $(CUBE_DIR)/Drivers/STM32F0xx_HAL_Driver
CMSIS_DIR	= $(CUBE_DIR)/Drivers/CMSIS
DEV_DIR		= $(CMSIS_DIR)/Device/ST/STM32F0xx

USB_DIR		= $(CUBE_DIR)/Middlewares/ST/STM32_USB_Device_Library
USB_CORE_DIR	= $(USB_DIR)/Core
USB_CLASS_CDC_DIR	= $(USB_DIR)/Class/CDC

###############################################################################
# Toolchain

PREFIX     = arm-none-eabi
CC         = $(PREFIX)-gcc
AR         = $(PREFIX)-ar
OBJCOPY    = $(PREFIX)-objcopy
OBJDUMP    = $(PREFIX)-objdump
SIZE       = $(PREFIX)-size
GDB        = $(PREFIX)-gdb

###############################################################################
# Options

# Defines
DEFS       = -D$(MCU_MC) -DUSE_HAL_DRIVER

# Include search paths (-I)
INCS	= -IInc
INCS	+= -I$(CMSIS_DIR)/Include
INCS	+= -I$(DEV_DIR)/Include
INCS	+= -I$(HAL_DIR)/Inc
INCS	+= -I$(USB_CORE_DIR)/Inc
INCS	+= -I$(USB_CLASS_CDC_DIR)/Inc

# Compiler flags
CFLAGS     = -Wall -g3 -std=gnu99 -Os
CFLAGS    += -mlittle-endian -mcpu=cortex-m0 -mthumb
CFLAGS    += -ffunction-sections -fdata-sections
CFLAGS    += $(INCS) $(DEFS)

# Linker flags
LDFLAGS    = -g -Wl,--gc-sections -Wl,-Map=$(TARGET).map -Tbuild/$(MCU_UC)_FLASH.ld

# Source search paths
VPATH	= ./Src
VPATH	+= $(HAL_DIR)/Src
VPATH	+= $(DEV_DIR)/Source/
VPATH	+= $(USB_CORE_DIR)/Src
VPATH	+= $(USB_CLASS_CDC_DIR)/Src

OBJS       = $(addprefix obj/,$(SRCS:.c=.o))
DEPS       = $(addprefix dep/,$(SRCS:.c=.d))

# Prettify output
V = 0
ifeq ($V, 0)
	Q = @
	P = > /dev/null
endif

###################################################

.PHONY: all dirs clean

all: dirs template $(TARGET).elf

-include $(DEPS)

dirs: dep obj build
dep obj src build:
	@echo "[MKDIR]   $@"
	$Qmkdir -p $@

obj/%.o : %.c | dirs
	@echo "[CC]      $(notdir $<)"
	$Q$(CC) $(CFLAGS) -c -o $@ $< -MMD -MF dep/$(*F).d

$(TARGET).elf: $(OBJS)
	@echo "[LD]      $(TARGET).elf"
	$Q$(CC) $(CFLAGS) $(LDFLAGS) Src/gcc/startup_$(MCU_LC).s $^ -o $@
	@echo "[OBJDUMP] $(TARGET).lst"
	$Q$(OBJDUMP) -St $(TARGET).elf >$(TARGET).lst
	@echo "[SIZE]    $(TARGET).elf"
	$(SIZE) $(TARGET).elf

cube:
	rm -fr $(CUBE_DIR)
	wget -O /tmp/cube.zip $(CUBE_URL)
	unzip /tmp/cube.zip
	mv STM32Cube* $(CUBE_DIR)
	chmod -R u+w $(CUBE_DIR)
	rm -f /tmp/cube.zip

template: cube
	cp $(CUBE_DIR)/Projects/STM32F072RB-Nucleo/Templates/TrueSTUDIO/STM32F072RB-Nucleo/STM32F072RB_FLASH.ld	build/$(MCU_UC)_FLASH.ld

clean:
	@echo "[RM]      $(TARGET).elf"; rm -f $(TARGET).elf
	@echo "[RM]      $(TARGET).map"; rm -f $(TARGET).map
	@echo "[RM]      $(TARGET).lst"; rm -f $(TARGET).lst
	@echo "[RMDIR]   dep"          ; rm -fr dep
	@echo "[RMDIR]   obj"          ; rm -fr obj
	@echo "[RMDIR]   build"        ; rm -fr build

