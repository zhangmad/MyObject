/*
Copyright 2014 Kai Ryu <kai1103@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "backlight.h"
#ifdef SOFTPWM_LED_ENABLE
#include "softpwm_led.h"
#else
#include "breathing_led.h"
#endif
#include "action.h"

#ifdef BACKLIGHT_ENABLE

static const uint8_t backlight_table[] PROGMEM = {
    0, 16, 128, 255
};

inline void backlight_set_raw(uint8_t raw);

#ifdef SOFTPWM_LED_ENABLE
#ifdef FADING_LED_ENABLE
static uint8_t backlight_mode;
#endif 
#endif

/* Backlight pin configuration
 * PWM: PC6(OC3A)
 */
#ifndef SOFTPWM_LED_ENABLE
void backlight_enable(void)
{
    // Turn on PWM
    cli();
    DDRC |= (1<<PC6);
    TCCR3A |= ( (1<<WGM10) | (1<<COM3A1) );
    TCCR3B |= ( (1<<CS31) | (1<<CS30) );
    sei();
}
void backlight_disable(void)
{
    // Turn off PWM
    cli();
    DDRC &= ~(1<<PC6);
    TCCR3A &= ~( (1<<WGM10) | (1<<COM3A1) );
    TCCR3B &= ~( (1<<CS31) | (1<<CS30) );
    sei();
}
#endif

void backlight_set(uint8_t level)
{
#ifdef FADING_LED_ENABLE
    backlight_mode = level;
#endif

#ifdef BREATHING_LED_ENABLE
    switch (level) {
        case 1:
        case 2:
        case 3:
#ifdef SOFTPWM_LED_ENABLE
            softpwm_led_enable();
#ifdef FADING_LED_ENABLE
            fading_led_disable_all();
#endif
            breathing_led_disable_all();
#else
            backlight_enable();
            breathing_led_disable();
#endif
            backlight_set_raw(pgm_read_byte(&backlight_table[level]));
            break;
        case 4:
        case 5:
        case 6:
#ifdef SOFTPWM_LED_ENABLE
            softpwm_led_enable();
#ifdef FADING_LED_ENABLE
            fading_led_disable_all();
#endif
            breathing_led_enable_all();
#else
            backlight_enable();
            breathing_led_enable();
#endif
            breathing_led_set_duration(6 - level);
            break;
#ifdef SOFTPWM_LED_ENABLE
#ifdef FADING_LED_ENABLE
        case 7:
            softpwm_led_enable();
            fading_led_enable_all();
            breathing_led_disable_all();
            fading_led_set_direction_all(FADING_LED_FADE_IN);
            fading_led_set_duration(3);
            break;
        case 8:
            softpwm_led_enable();
            fading_led_enable_all();
            breathing_led_disable_all();
            fading_led_set_direction_all(FADING_LED_FADE_OUT);
            fading_led_set_duration(3);
            break;
#endif
#endif
        case 0:
        default:
#ifdef SOFTPWM_LED_ENABLE
#ifdef FADING_LED_ENABLE
            fading_led_disable_all();
#endif
            breathing_led_disable_all();
            softpwm_led_disable();
#else
            breathing_led_disable();
            backlight_disable();
#endif
            break;
    }
#else
    if (level > 0) {
        backlight_enable();
        backlight_set_raw(pgm_read_byte(&backlight_table[level]));
    }
    else {
        backlight_disable();
    }
#endif

}

#ifndef SOFTPWM_LED_ENABLE
#ifdef BREATHING_LED_ENABLE
void breathing_led_set_raw(uint8_t raw)
{
    backlight_set_raw(raw);
}
#endif
#endif

inline void backlight_set_raw(uint8_t raw)
{
#ifdef SOFTPWM_LED_ENABLE
    softpwm_led_set_all(raw);
#else
    OCR3A = raw;
#endif
}

#ifndef LEDMAP_ENABLE
#ifdef SOFTPWM_LED_ENABLE
void softpwm_led_init(void)
{
    DDRC  |=  (1<<PC6);
}

void softpwm_led_on(uint8_t index)
{
    PORTC |=  (1<<PC6);
}

void softpwm_led_off(uint8_t index)
{
    PORTC &= ~(1<<PC6);
}
#endif
#endif

#ifdef SOFTPWM_LED_ENABLE
#ifdef FADING_LED_ENABLE
void action_keyevent(keyevent_t event)
{
    if (backlight_mode == 7) {
        if (event.pressed) {
            softpwm_led_decrease_all(32);
        }
    }
    if (backlight_mode == 8) {
        if (event.pressed) {
            softpwm_led_increase_all(32);
        }
    }
}
#endif
#endif

#endif
