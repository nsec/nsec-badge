.PHONY: all default clean

default: all

builds:
	mkdir builds

builds/nsec18_stm32_%: builds
	$(MAKE) -C stm32 $@
	cp stm32/$@ $@

builds/nsec18_nrf52_%.elf: builds
	$(MAKE) -C nrf52 FLAVOR=$*
	cp nrf52/$(@:%.elf=%.out) $@

builds/s132_nrf52_5.0.0_softdevice.hex:
	$(MAKE) -C nrf52 nordicsdk
	cp nrf52/nordicsdk/softdevice/s132/hex/s132_nrf52_5.0.0_softdevice.hex $@

FIRMWARE += builds/nsec18_nrf52_ctf.elf
FIRMWARE += builds/nsec18_nrf52_admin.elf
FIRMWARE += builds/nsec18_nrf52_speaker.elf
FIRMWARE += builds/nsec18_nrf52_conf.elf
FIRMWARE += builds/nsec18_stm32_debugger.elf
FIRMWARE += builds/nsec18_stm32_debugger.bin
FIRMWARE += builds/s132_nrf52_5.0.0_softdevice.hex

all: $(FIRMWARE)

clean:
	$(MAKE) -C stm32 clean
	$(MAKE) -C nrf52 clean
	rm -rf builds
