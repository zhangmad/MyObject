#include "keymap_common.h"

// 4x6 Keypad
#ifdef KEYMAP_SECTION_ENABLE
const uint8_t keymaps[KEYMAPS_COUNT][MATRIX_ROWS][MATRIX_COLS] __attribute__ ((section (".keymap.keymaps"))) = {
#else
const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] PROGMEM = {
#endif
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
   [0] = KEYMAP(
       ESC, FN0, FN1, BSPC, \
       NLCK,FN2, FN3, PPLS, \
       P7,  P8,  P9,  PMNS, \
       P4,  P5,  P6,  PAST, \
       P1,  P2,  P3,  PSLS, \
       EQL, P0,  PDOT,PENT)
};

/*
 * Fn action definition
 */
#ifdef KEYMAP_SECTION_ENABLE
const uint16_t fn_actions[FN_ACTIONS_COUNT] __attribute__ ((section (".keymap.fn_actions"))) = {
#else
const uint16_t fn_actions[] PROGMEM = {
#endif
    [0] = ACTION_BACKLIGHT_TOGGLE(),
    [1] = ACTION_BACKLIGHT_DECREASE(),
    [2] = ACTION_BACKLIGHT_INCREASE()
};

#ifdef KEYMAP_IN_EEPROM_ENABLE
uint16_t keys_count(void) {
    return sizeof(keymaps) / sizeof(keymaps[0]) * MATRIX_ROWS * MATRIX_COLS;
}

uint16_t fn_actions_count(void) {
    return sizeof(fn_actions) / sizeof(fn_actions[0]);
}
#endif
