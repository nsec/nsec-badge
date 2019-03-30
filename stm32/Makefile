PHONY: all

builds:
	mkdir builds

builds/nsec19_stm32_debugger.elf: builds
	$(MAKE) -C blackmagic PROBE_HOST=nsec19
	cp blackmagic/src/blackmagic builds/nsec19_stm32_debugger.elf

builds/nsec19_stm32_soldering.elf: builds
	$(MAKE) -C blackmagic clean PROBE_HOST=nsec19
	$(MAKE) -C blackmagic PROBE_HOST=nsec19 TRACK=soldering
	cp blackmagic/src/blackmagic builds/nsec19_stm32_soldering.elf

builds/%.bin: builds/%.elf
	arm-none-eabi-objcopy -O binary $^ $@

all: builds/nsec19_stm32_debugger.elf \
     builds/nsec19_stm32_debugger.bin \
     builds/nsec19_stm32_soldering.elf \
     builds/nsec19_stm32_soldering.bin

clean:
	$(MAKE) -C blackmagic clean PROBE_HOST=nsec19
	rm -rf builds
