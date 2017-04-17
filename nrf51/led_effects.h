//
//  led_effects.h
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2017-04-15.
//
//

#ifndef led_effects_h
#define led_effects_h

#include <stdint.h>

typedef enum {
    NSEC_LED_EFFECT_ALL_OFF,
    NSEC_LED_EFFECT_ALL_ON,
    NSEC_LED_EFFECT_SPIN,
    NSEC_LED_EFFECT_FLASH_ALL,
} nsec_led_effect;

void nsec_led_init(void);
void nsec_led_set_delay(uint32_t milliseconds);
void nsec_led_set_effect(nsec_led_effect effect);

#endif /* led_effects_h */
