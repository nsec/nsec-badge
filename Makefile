.PHONY: all default clean

default: all

builds:
	mkdir builds

builds/nsec17_stm32_%: builds
	$(MAKE) -C stm32 $@
	cp stm32/$@ $@

builds/nsec17_nrf51_%.elf: builds
	$(MAKE) -C nrf51 FLAVOR=$*
	cp nrf51/$(@:%.elf=%.out) $@

FIREWARE  = builds/nsec17_stm32_conf.elf
FIREWARE += builds/nsec17_stm32_ctf.elf
FIREWARE += builds/nsec17_stm32_crossdebug.elf
FIREWARE += builds/nsec17_nrf51_conf.elf
FIREWARE += builds/nsec17_nrf51_speaker.elf
FIREWARE += builds/nsec17_nrf51_admin.elf
FIREWARE += builds/nsec17_nrf51_ctf.elf
FIREWARE += builds/nsec17_nrf51_ctf_rao.elf
FIREWARE += builds/nsec17_nrf51_ctf_namechange.elf

all: $(FIREWARE)

clean:
	$(MAKE) -C stm32 clean
	$(MAKE) -C nrf51 clean
	rm -Rf builds
