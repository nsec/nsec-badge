PHONY: all

all:
	$(MAKE) -C libopencm3 TARGETS=stm32/f0
	$(MAKE) -C blackmagic PROBE_HOST=nsec17
	$(MAKE) -C src BINARY=stm32_nsec17_ctf
	$(MAKE) -C src BINARY=stm32_nsec17_conf

clean:
	$(MAKE) -C libopencm3 clean
	$(MAKE) -C blackmagic clean
	$(MAKE) -C src clean
