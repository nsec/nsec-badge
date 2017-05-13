PHONY: all

all:
	$(MAKE) -C libopencm3 TARGETS=stm32/f0
	$(MAKE) -C src

clean:
	$(MAKE) -C libopencm3 clean
	$(MAKE) -C src clean
