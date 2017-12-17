PHONY: all

all:
	$(MAKE) -C libopencm3 TARGETS=stm32/f0
	$(MAKE) -C blackmagic PROBE_HOST=nsec17
	$(MAKE) -C src FLAVOR=ctf
	$(MAKE) -C src FLAVOR=conf

clean:
	$(MAKE) -C libopencm3 clean
	$(MAKE) -C blackmagic clean
	$(MAKE) -C src clean
