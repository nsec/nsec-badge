.PHONY: all nrf stm default clean

default: all

builds:
	mkdir builds

builds/nsec19_stm32_%: builds
	$(MAKE) -C stm32 $@
	cp stm32/$@ $@

builds/nsec_nrf52_brain_%.elf: builds
	$(MAKE) -C nrf52 FLAVOR=$*
	cp nrf52/builds/brain-$*/nsec_nrf52_brain_$*.elf $@

builds/%_softdevice.hex:
	$(MAKE) -C nrf52 nordicsdk
	cp nrf52/nordicsdk/components/softdevice/s132/hex/$(notdir $@) $@

FIRMWARE_NRF += builds/nsec_nrf52_brain_bar_beacon.elf
FIRMWARE_NRF += builds/nsec_nrf52_brain_ctf.elf
FIRMWARE_NRF += builds/nsec_nrf52_brain_conf.elf
FIRMWARE_NRF += builds/nsec_nrf52_brain_soldering.elf
FIRMWARE_NRF += builds/s132_nrf52_7.0.1_softdevice.hex

FIRMWARE_STM += builds/nsec19_stm32_debugger.elf
FIRMWARE_STM += builds/nsec19_stm32_debugger.bin
FIRMWARE_STM += builds/nsec19_stm32_soldering.elf
FIRMWARE_STM += builds/nsec19_stm32_soldering.bin


all: $(FIRMWARE_NRF) $(FIRMWARE_STM)
nrf: $(FIRMWARE_NRF)
stm: $(FIRMWARE_STM)

clean:
	$(MAKE) -C stm32 clean
	$(MAKE) -C nrf52 clean
	rm -rf builds
