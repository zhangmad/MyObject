/*
Copyright 2013,2014 Kai Ryu <kai1103@gmail.com>

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
#include "softpwm_led.h"
#include "action.h"

#ifdef BACKLIGHT_ENABLE

static uint8_t backlight_mode;

static const uint8_t backlight_table[] PROGMEM = {
    0, 16, 128, 255
};

/* Backlight pin configuration
 * PWM:  PB5 (RevRS)
 * GPIO: PF7  PF6  PF5
 */
void backlight_set(uint8_t level)
{
    backlight_mode = level;
    switch (level) {
        case 1:
        case 2:
        case 3:
            softpwm_led_enable();
            fading_led_disable_all();
            breathing_led_disable_all();
            softpwm_led_set_all(pgm_read_byte(&backlight_table[level]));
            break;
        case 4:
        case 5:
        case 6:
            softpwm_led_enable();
            breathing_led_enable_all();
            fading_led_disable_all();
            breathing_led_set_duration(6 - level);
            break;
        case 7:
            softpwm_led_enable();
            fading_led_enable_all();
            breathing_led_disable_all();
            fading_led_set_direction_all(FADING_LED_FADE_IN);
            fading_led_set_duration(4);
            break;
        case 8:
            softpwm_led_enable();
            fading_led_enable_all();
            breathing_led_disable_all();
            fading_led_set_direction_all(FADING_LED_FADE_OUT);
            fading_led_set_duration(2);
            break;
        case 0:
        default:
            fading_led_disable_all();
            breathing_led_disable_all();
            softpwm_led_disable();
            break;
    }
}

#ifndef LEDMAP_ENABLE
void softpwm_led_init(void)
{
    DDRB  |=  (1<<PB5);
    DDRF  |=  (1<<PF7 | 1<<PF6 | 1<<PF5);
}

void softpwm_led_on(uint8_t index)
{
    PORTB |=  (1<<PB5);
    PORTF &= ~(1<<PF7 | 1<<PF6 | 1<<PF5);
}

void softpwm_led_off(uint8_t index)
{
    PORTB &= ~(1<<PB5);
    PORTF |=  (1<<PF7 | 1<<PF6 | 1<<PF5);
}
#endif

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
