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

/*
 * scan matrix
 */
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "print.h"
#include "debug.h"
#include "util.h"
#ifdef REDSCARFII_PLUS
#include "rgb.h"
#endif
#include "matrix.h"

#ifndef DEBOUNCE
#   define DEBOUNCE 5
#endif
static uint8_t debouncing = DEBOUNCE;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

static matrix_row_t read_cols(void);
static void init_cols(void);
static void init_rows(void);
static void unselect_rows(void);
static void select_row(uint8_t row);

inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

void matrix_init(void)
{
    // disable JTAG
    MCUCR = (1<<JTD);
    MCUCR = (1<<JTD);

#ifdef REDSCARFII_PLUS
    // initialize rgb led
    rgb_init();
#endif

    // initialize row and col
    init_rows();
    init_cols();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        matrix_debouncing[i] = 0;
    }
}

uint8_t matrix_scan(void)
{
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        select_row(i);
        _delay_us(30);  // without this wait read unstable value.
        matrix_row_t cols = read_cols();
        if (matrix_debouncing[i] != cols) {
            matrix_debouncing[i] = cols;
            if (debouncing) {
                debug("bounce!: "); debug_hex(debouncing); debug("\n");
            }
            debouncing = DEBOUNCE;
        }
        unselect_rows();
    }

    if (debouncing) {
        if (--debouncing) {
            _delay_ms(1);
        } else {
            for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
                matrix[i] = matrix_debouncing[i];
            }
        }
    }

    return 1;
}

bool matrix_is_modified(void)
{
    if (debouncing) return false;
    return true;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        pbin_reverse16(matrix_get_row(row));
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop16(matrix[i]);
    }
    return count;
}

/* Column pin configuration
 * col: 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17
 * pin: F4  F1  F0  B3  D0  D1  D4  D5  D6  D7  F7  F6  D2  D3  B6  B5  B4  F5
 */
static void  init_cols(void)
{
    // Input with pull-up(DDR:0, PORT:1)
    DDRF  &= ~(1<<PF7 | 1<<PF6 | 1<<PF5 | 1<<PF4 | 1<<PF1 | 1<<PF0);
    PORTF |=  (1<<PF7 | 1<<PF6 | 1<<PF5 | 1<<PF4 | 1<<PF1 | 1<<PF0);
    DDRD  &= ~(1<<PD7 | 1<<PD6 | 1<<PD5 | 1<<PD4 | 1<<PD3 | 1<<PD2 | 1<<PD1 | 1<<PD0);
    PORTD |=  (1<<PD7 | 1<<PD6 | 1<<PD5 | 1<<PD4 | 1<<PD3 | 1<<PD2 | 1<<PD1 | 1<<PD0);
    DDRB  &= ~(1<<PB6 | 1<<PB5 | 1<<PB4 | 1<<PB3);
    PORTB |=  (1<<PB6 | 1<<PB5 | 1<<PB4 | 1<<PB3);
}

static matrix_row_t read_cols(void)
{
    return (PINF&(1<<PF4) ? 0 : (1<<0)) |
           (PINF&(1<<PF1) ? 0 : (1<<1)) |
           (PINF&(1<<PF0) ? 0 : (1<<2)) |
           (PINB&(1<<PB3) ? 0 : (1<<3)) |
           (PIND&(1<<PD0) ? 0 : (1<<4)) |
           (PIND&(1<<PD1) ? 0 : (1<<5)) |
           (PIND&(1<<PD4) ? 0 : (1<<6)) |
           (PIND&(1<<PD5) ? 0 : (1<<7)) |
           (PIND&(1<<PD6) ? 0 : (1<<8)) |
           (PIND&(1<<PD7) ? 0 : (1<<9)) |
           (PINF&(1<<PF7) ? 0 : (1<<10)) |
           (PINF&(1<<PF6) ? 0 : (1<<11)) |
           (PIND&(1<<PD2) ? 0 : (1<<12)) |
           (PIND&(1<<PD3) ? 0 : (1<<13)) |
           (PINB&(1<<PB6) ? 0 : (1<<14)) |
           (PINB&(1<<PB5) ? 0 : (1UL<<15)) |
           (PINB&(1<<PB4) ? 0 : (1UL<<16)) |
           (PINF&(1<<PF5) ? 0 : (1UL<<17));
}

/* Row pin configuration
 * row:     0   1   2   3   4   5   x
 * pin: B2  0   1   0   1   0   1   1
 *      B1  0   0   1   1   0   0   1
 *      B0  0   0   0   0   1   1   1
 */
static void init_rows(void)
{
    DDRB  |= (1<<PB2 | 1<<PB1 | 1<<PB0);
}

static void unselect_rows(void)
{
    //PORTB |= (1<<PB2 | 1<<PB1 | 1<<PB0);
    PORTB |= (1<<PB2);
    PORTB |= (1<<PB1);
    PORTB |= (1<<PB0);
}

static void select_row(uint8_t row)
{
    (row & (1<<0)) ? (PORTB |= (1<<PB2)) : (PORTB &= ~(1<<PB2));
    (row & (1<<1)) ? (PORTB |= (1<<PB1)) : (PORTB &= ~(1<<PB1));
    (row & (1<<2)) ? (PORTB |= (1<<PB0)) : (PORTB &= ~(1<<PB0));
}
