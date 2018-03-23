.PHONY: all default clean

default: all

builds:
	mkdir builds

builds/nsec17_stm32_%: builds
	$(MAKE) -C stm32 $@
	cp stm32/$@ $@

builds/nsec18_nrf52_%.elf: builds
	$(MAKE) -C nrf52 FLAVOR=$*
	cp nrf52/$(@:%.elf=%.out) $@

FIRMWARE += builds/nsec18_nrf52_devboard.elf
FIRMWARE += builds/nsec18_nrf52_proto.elf

all: $(FIRMWARE)

clean:
	$(MAKE) -C stm32 clean
	$(MAKE) -C nrf52 clean
	rm -rf builds
