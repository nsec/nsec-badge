PHONY: all

builds:
	mkdir builds

builds/nsec19_stm32_debugger.elf: builds
	$(MAKE) -C blackmagic PROBE_HOST=nsec19
	cp blackmagic/src/blackmagic builds/nsec19_stm32_debugger.elf

builds/%.bin: builds/%.elf
	arm-none-eabi-objcopy -O binary $^ $@

all: builds/nsec19_stm32_debugger.elf \
     builds/nsec19_stm32_debugger.bin

clean:
	$(MAKE) -C blackmagic clean PROBE_HOST=nsec19
	rm -rf builds
