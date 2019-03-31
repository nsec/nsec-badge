PHONY: all

builds:
	mkdir builds

builds/nsec19_stm32_debugger.elf: blackmagic/src/blackmagic builds
	$(MAKE) -C blackmagic PROBE_HOST=nsec19
	cp blackmagic/src/blackmagic builds/nsec19_stm32_debugger.elf

builds/nsec19_stm32_soldering.elf: blackmagic/src/blackmagic-soldering builds
	$(MAKE) -C blackmagic PROBE_HOST=nsec19
	cp blackmagic/src/blackmagic-soldering builds/nsec19_stm32_soldering.elf

blackmagic/src/blackmagic blackmagic/src/blackmagic-soldering:
	$(MAKE) -C blackmagic PROBE_HOST=nsec19

builds/%.bin: builds/%.elf
	arm-none-eabi-objcopy -O binary $^ $@

all: builds/nsec19_stm32_debugger.elf \
     builds/nsec19_stm32_debugger.bin \
     builds/nsec19_stm32_soldering.elf \
     builds/nsec19_stm32_soldering.bin

clean:
	$(MAKE) -C blackmagic clean PROBE_HOST=nsec19
	rm -rf builds
