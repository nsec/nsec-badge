PHONY: all

builds:
	mkdir builds

builds/nsec18_stm32_debugger.elf: builds
	$(MAKE) -C libopencm3 TARGETS=stm32/f0
	$(MAKE) -C blackmagic clean PROBE_HOST=nsec18
	$(MAKE) -C blackmagic PROBE_HOST=nsec18
	cp blackmagic/src/blackmagic builds/nsec18_stm32_debugger.elf

builds/nsec17_stm32_crossdebug.elf: builds
	$(MAKE) -C libopencm3 TARGETS=stm32/f0
	$(MAKE) -C blackmagic clean PROBE_HOST=nsec18
	$(MAKE) -C blackmagic PROBE_HOST=nsec18 FLAVOR=crossdebug
	cp blackmagic/src/blackmagic builds/nsec18_stm32_crossdebug.elf

builds/%.bin: builds/%.elf
	arm-none-eabi-objcopy -O binary $^ $@

all: builds/nsec18_stm32_debugger.elf \
     builds/nsec18_stm32_debugger.bin

clean:
	$(MAKE) -C libopencm3 clean
	$(MAKE) -C blackmagic clean PROBE_HOST=nsec18
	rm -rf builds
