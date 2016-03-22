#include "keymap_common.h"

/*
 * Hasu
 */
const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
   /* Keymap 0: Default Layer
    * ,---------------.
    * |Esc|Fn0|Fn1|BS |
    * |---+---+---+---|
    * |Num|Fn2|Fn3|+  |
    * |---+---+---+---|
    * |7  |8  |9  |-  |
    * |---+---+---|---|
    * |4  |5  |6  |*  |
    * |---+---+---+---|
    * |1  |2  |3  |/  |
    * |---+---+---|---|
    * |=  |0  |.  |Ent|
    * `---------------'
    */
   KEYMAP(
       ESC, FN0, FN1, BSPC, \
       NLCK,FN2, FN3, PPLS, \
       P7,  P8,  P9,  PMNS, \
       P4,  P5,  P6,  PAST, \
       P1,  P2,  P3,  PSLS, \
       EQL, P0,  PDOT,PENT),
    
	KEYMAP(
        ESC, FN0, FN1, BSPC, \
        NLCK,FN2, FN3, PPLS, \
        A,  B,  C,  D, \
        A,  B,  C,  D, \
        A,  B,  C,  D, \
        A,  B,  C,  D),
	

};

/*
 * Fn action definition
 */
const uint16_t PROGMEM fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(1),
    [1] = ACTION_BACKLIGHT_TOGGLE(),
    [2] = ACTION_BACKLIGHT_DECREASE(),
    [3] = ACTION_BACKLIGHT_INCREASE()
};


