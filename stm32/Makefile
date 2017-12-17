PHONY: all

builds:
	mkdir builds

builds/nsec17_stm32_ctf.elf: builds
	$(MAKE) -C libopencm3 TARGETS=stm32/f0
	$(MAKE) -C src FLAVOR=ctf
	cp src/nsec17_stm32_ctf.elf builds/nsec17_stm32_ctf.elf

builds/nsec17_stm32_conf.elf: builds
	$(MAKE) -C libopencm3 TARGETS=stm32/f0
	$(MAKE) -C blackmagic clean PROBE_HOST=nsec17
	$(MAKE) -C blackmagic PROBE_HOST=nsec17
	$(MAKE) -C src clean FLAVOR=conf
	$(MAKE) -C src FLAVOR=conf
	cp src/nsec17_stm32_conf.elf builds/nsec17_stm32_conf.elf

builds/nsec17_stm32_crossdebug.elf: builds
	$(MAKE) -C libopencm3 TARGETS=stm32/f0
	$(MAKE) -C blackmagic clean PROBE_HOST=nsec17
	$(MAKE) -C blackmagic PROBE_HOST=nsec17 FLAVOR=crossdebug
	$(MAKE) -C src clean FLAVOR=conf
	$(MAKE) -C src FLAVOR=conf
	cp src/nsec17_stm32_conf.elf builds/nsec17_stm32_crossdebug.elf

builds/%.bin: builds/%.elf
	arm-none-eabi-objcopy -O binary $^ $@

all: builds/nsec17_stm32_ctf.elf \
     builds/nsec17_stm32_conf.elf \
     builds/nsec17_stm32_crossdebug.elf \
     builds/nsec17_stm32_ctf.bin \
     builds/nsec17_stm32_conf.bin \
     builds/nsec17_stm32_crossdebug.bin

clean:
	$(MAKE) -C libopencm3 clean
	$(MAKE) -C blackmagic clean PROBE_HOST=nsec17
	$(MAKE) -C src clean FLAVOR=ctf
	$(MAKE) -C src clean FLAVOR=conf
	rm -R builds
