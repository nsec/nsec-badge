#include <stdint.h>

#include "delay.h"

void delay(uint32_t cycle) {
    for (uint32_t i=0; i<cycle; i++) {
        __asm__("nop");
    }
}
