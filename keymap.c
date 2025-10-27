/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

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

#include QMK_KEYBOARD_H

// ============================================================================
// CUSTOM KEYCODES
// ============================================================================
enum custom_keycodes {
    RGB_TOG_CUSTOM = SAFE_RANGE,
    GAMING_MODE,
    DEFAULT_MODE,
    ARROW_R,
    ARROW_L                // ->
};

// ============================================================================
// STATE VARIABLES
// ============================================================================
static bool rgb_enabled = true;


// ============================================================================
// LED MAPPING - BASED ON YOUR測試 RESULTS
// ============================================================================
#define UNDERGLOW_LEFT_START 0
#define UNDERGLOW_LEFT_END 6
#define LEFT_KEY_OFFSET 6
#define UNDERGLOW_RIGHT_START 27
#define UNDERGLOW_RIGHT_END 33
#define RIGHT_KEY_OFFSET 27

const uint8_t PROGMEM left_matrix_to_led[4][6] = {
    {        24,  23,  18,  17,  10,   9 },
    {        25,  22,  19,  16,  11,   8 },
    {        26,  21,  20,  15,  12,   7 },
    {       255, 255, 255,  14,  13,   6 }
};

const uint8_t PROGMEM right_matrix_to_led[4][6] = {
    {         9,  10,  17,  18,  23,  24 },
    {         8,  11,  16,  19,  22,  25 },
    {         7,  12,  15,  20,  21,  26 },
    {       255, 255, 255,  6,  13,   14 }
};

uint8_t get_led_from_matrix(uint8_t row, uint8_t col) {
    if (row < 4) {
        if (col < 6) {
            uint8_t led_idx = pgm_read_byte(&left_matrix_to_led[row][col]);
            return led_idx;
        }
    } else if (row < 8) {
        if (col < 6) {
            uint8_t relative_idx = pgm_read_byte(&right_matrix_to_led[row - 4][col]);
            if (relative_idx == 255) return 255;
            return relative_idx + RIGHT_KEY_OFFSET;
        }
    }
    return 255;
}

// ============================================================================
// COLOR CONFIGURATION - CUSTOMIZE THESE (HSV: Hue 0-255, Sat 0-255, Val 0-255)
// ============================================================================

// LAYER 0 (Default Layer) Colors
#define L0_KEY_H        48
#define L0_KEY_S        255
#define L0_KEY_V        60

#define L0_MOD_H        95
#define L0_MOD_S        255
#define L0_MOD_V        80

// LAYER 1 (Numbers & Symbols) Colors
#define L1_NUMBERS_H    95
#define L1_NUMBERS_S    255
#define L1_NUMBERS_V    100

#define L1_BRACKETS_H   170
#define L1_BRACKETS_S   255
#define L1_BRACKETS_V   120

#define L1_SYMBOLS_H    43
#define L1_SYMBOLS_S    255
#define L1_SYMBOLS_V    25

#define L1_MOD_H        130
#define L1_MOD_S        255
#define L1_MOD_V        100

// LAYER 2 (Functions & Navigation) Colors
#define L2_NUMBERS_H    240
#define L2_NUMBERS_S    255
#define L2_NUMBERS_V    50

#define L2_FUNCTION_H   43
#define L2_FUNCTION_S   255
#define L2_FUNCTION_V   25

#define L2_ARROWS_H     170
#define L2_ARROWS_S     255
#define L2_ARROWS_V     120

#define L2_OTHERS_H     43
#define L2_OTHERS_S     255
#define L2_OTHERS_V     25

#define L2_MOD_H        190
#define L2_MOD_S        255
#define L2_MOD_V        100

// LAYER 3 (Gaming) Colors
#define L3_GAMING_H    110
#define L3_GAMING_S    255
#define L3_GAMING_V    100

#define L3_DEFAULT_H    240
#define L3_DEFAULT_S    255
#define L3_DEFAULT_V    50

#define L3_MOD_H        190
#define L3_MOD_S        255
#define L3_MOD_V        100

// Underglow color
#define UNDERGLOW_H     110
#define UNDERGLOW_S     255
#define UNDERGLOW_V     100

// Layer indicator color (for held layer keys)
#define LAYER_IND_H     110
#define LAYER_IND_S     255
#define LAYER_IND_V     120

// Breathing effect speed
#define BREATHING_SPEED 70

// ============================================================================
// KEYMAP DEFINITION
// ============================================================================
//


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Layer 0: Colemak-DH (Default)
    [0] = LAYOUT_split_3x6_3(
       KC_TAB,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,                         KC_J,    KC_L,    KC_U,    KC_Y, KC_SCLN,  KC_DEL,
      MT(MOD_LSFT, KC_ESC), KC_A,  KC_R,    KC_S,    KC_T,    KC_G,                         KC_M,    KC_N,    KC_E,    KC_I,    KC_O,  KC_ENT,
      KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,                         KC_K,    KC_H, KC_COMM,  KC_DOT, KC_SLSH, KC_QUOT,
                                          KC_LALT,   MO(1),  KC_SPC,    KC_BSPC,   MO(2), KC_LSFT
    ),

    // // Layer 1: Numbers & Symbols (Colemak)
    // [1] = LAYOUT_split_3x6_3(
    //    KC_TAB, KC_EXLM,   KC_AT, KC_LPRN, KC_RPRN, KC_BSLS,                      KC_QUOT,    KC_7,    KC_8,    KC_9, KC_ASTR,  KC_GRV,
    //   MT(MOD_LSFT, KC_ESC), KC_HASH,  KC_DLR, KC_LCBR, KC_RCBR,  KC_GRV,                      KC_SCLN,    KC_4,    KC_5,    KC_6,  KC_EQL, KC_MINS,
    //   KC_LCTL, KC_PERC, KC_CIRC, KC_LBRC, KC_RBRC, KC_TILD,                      KC_AMPR,    KC_1,    KC_2,    KC_3, KC_SLSH, KC_QUOT,
    //                                       KC_LALT, _______,  KC_SPC,    KC_BSPC,   MO(3),    KC_0
    // ),
    //
    [1] = LAYOUT_split_3x6_3(
       KC_TAB, KC_EXLM,   KC_AT, KC_LPRN, KC_RPRN, KC_PIPE,                      KC_COLN,    KC_7,    KC_8,    KC_9, KC_PLUS,  KC_GRV,
      LSFT_T(KC_ESC), KC_HASH,  KC_DLR, KC_LCBR, KC_RCBR, ARROW_R,                      KC_SCLN,    KC_4,    KC_5,    KC_6,  KC_EQL, KC_UNDS,
      KC_LCTL, KC_PERC, KC_AMPR, KC_LBRC, KC_RBRC, ARROW_L,                      KC_QUOT,    KC_1,    KC_2,    KC_3, KC_MINS, KC_BSLS,
                                          KC_LALT, _______,  KC_SPC,    KC_BSPC,   MO(3),    KC_0
),

    // Layer 2: Functions & Navigation (Colemak)
    [2] = LAYOUT_split_3x6_3(
       KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_PGUP,
      KC_LSFT, XXXXXXX,   KC_F5,   KC_F6,   KC_F7,   KC_F8,                      KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, RGB_VAD, RGB_VAI,
      KC_LCTL, XXXXXXX,   KC_F9,  KC_F10,  KC_F11,  KC_F12,                      KC_HOME,   KC_F1,   KC_F2,   KC_F3,   KC_F4, KC_PGDN,
                                          KC_LALT,   MO(3),  KC_SPC,    KC_BSPC, _______, KC_LSFT
    ),

    // Layer 3: Settings (accessible from both modes)
    [3] = LAYOUT_split_3x6_3(
      QK_BOOT, RGB_TOG_CUSTOM, GAMING_MODE, DEFAULT_MODE, XXXXXXX, XXXXXXX,                      XXXXXXX, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, XXXXXXX,
      XXXXXXX, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX, XXXXXXX,                      XXXXXXX, KC_VOLD, KC_VOLU, XXXXXXX, XXXXXXX, XXXXXXX,
      XXXXXXX, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                          XXXXXXX, _______,  XXXXXXX,    XXXXXXX, _______, XXXXXXX
    ),

    // Layer 4: Gaming Layer 0 (QWERTY)
    [4] = LAYOUT_split_3x6_3(
       KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,  KC_DEL,
      KC_LSFT,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN,  KC_ENT,
      KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_QUOT,
                                          KC_LALT,   MO(5),  KC_SPC,    KC_BSPC,   MO(2), KC_ESC
    ),

    // Layer 5: Gaming Layer 1 (Numbers & Arrows)
    [5] = LAYOUT_split_3x6_3(
       KC_TAB,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,    KC_0,  KC_GRV,
       KC_ESC, KC_HASH,  KC_DLR, KC_LCBR, KC_RCBR,  KC_GRV,                      XXXXXXX,    KC_4,    KC_UP,    KC_6,  KC_EQL, KC_MINS,
      KC_LCTL, KC_PERC, KC_CIRC, KC_LBRC, KC_RBRC, KC_TILD,                      KC_AMPR,    KC_LEFT,    KC_DOWN, KC_RIGHT,  KC_DOT, KC_QUOT,
                                          KC_LALT, _______,  KC_SPC,    KC_BSPC,   MO(3),    KC_0
    )
};

// ============================================================================
// CUSTOM KEYCODE PROCESSING
// ============================================================================

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case RGB_TOG_CUSTOM:
            if (record->event.pressed) {
                rgb_enabled = !rgb_enabled;
                if (!rgb_enabled) {
                    rgb_matrix_set_color_all(0, 0, 0);
                }
            }
            return false;
        case GAMING_MODE:
            if (record->event.pressed) {
                // Switch to gaming layer permanently
                layer_move(4);
            }
            return false;
        case DEFAULT_MODE:
            if (record->event.pressed) {
                // Switch back to default Colemak layer
                layer_move(0);
            }
            return false;
        case ARROW_R:
            if (record->event.pressed) {
                SEND_STRING("=>");
            }
            return false;
        case ARROW_L:
            if (record->event.pressed) {
                SEND_STRING("->");
            }
            return false;
    }
    return true;
}


// ============================================================================
// KEY CATEGORIZATION
// ============================================================================

bool is_layer0_mod(uint8_t row, uint8_t col) {
    if ((row == 0 && col == 0) ||  // TAB
        (row == 1 && col == 0) ||  // ESC
        (row == 2 && col == 0) ||  // CTRL
        (row == 3 && col == 3) ||  // ALT
        (row == 3 && col == 4) ||  // LAYER1
        (row == 3 && col == 5) ||  // SPC
        (row == 4 && col == 5) ||  // DEL
        (row == 5 && col == 5) ||  // ENT
        (row == 6 && col == 5) ||  // '
        (row == 7 && col == 3) ||  // BKSP
        (row == 7 && col == 4) ||  // LAYER2
        (row == 7 && col == 5)) {  // SHFT
        return true;
    }
    return false;
}

uint16_t get_layer1_keycode(uint8_t row, uint8_t col) {
    const uint16_t layer1_actual[8][6] = {
        {KC_TAB, KC_EXLM, KC_AT, KC_LPRN, KC_RPRN, KC_PIPE},
        {KC_ESC, KC_HASH, KC_DLR, KC_LCBR, KC_RCBR, KC_GRV},
        {KC_LCTL, KC_PERC, KC_CIRC, KC_LBRC, KC_RBRC, KC_TILD},
        {KC_NO, KC_NO, KC_NO, KC_LALT, KC_TRNS, KC_SPC},
        {KC_QUOT, KC_7, KC_8, KC_9, KC_ASTR, KC_GRV},
        {KC_NO, KC_4, KC_5, KC_6, KC_EQL, KC_MINS},
        {KC_AMPR, KC_1, KC_2, KC_3, KC_SLSH, KC_QUOT},
        {KC_NO, KC_NO, KC_NO, KC_BSPC, MO(3), KC_0}
    };
    if (row < 8 && col < 6) return layer1_actual[row][col];
    return KC_NO;
}

typedef enum {
    L1_CAT_NUMBER,
    L1_CAT_BRACKET,
    L1_CAT_SYMBOL,
    L1_CAT_MOD
} layer1_category_t;

layer1_category_t get_layer1_category(uint16_t keycode) {
    // Check numbers
    if (keycode == KC_0 || keycode == KC_1 || keycode == KC_2 ||
        keycode == KC_3 || keycode == KC_4 || keycode == KC_5 ||
        keycode == KC_6 || keycode == KC_7 || keycode == KC_8 ||
        keycode == KC_9) {
        return L1_CAT_NUMBER;
    }

    // Check mod keys first
    if (keycode == KC_TAB || keycode == KC_ESC || keycode == KC_LCTL ||
        keycode == KC_LALT || keycode == KC_SPC || keycode == KC_BSPC ||
        keycode == KC_TRNS || keycode == MO(3) || keycode == KC_NO) {
        return L1_CAT_MOD;
    }

    // Check brackets
    if (keycode == KC_LPRN || keycode == KC_RPRN ||
        keycode == KC_LBRC || keycode == KC_RBRC ||
        keycode == KC_LCBR || keycode == KC_RCBR) {
        return L1_CAT_BRACKET;
    }

    return L1_CAT_SYMBOL;
}

uint16_t get_layer2_keycode(uint8_t row, uint8_t col) {
    const uint16_t layer2_actual[8][6] = {
        {KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5},
        {KC_LSFT, KC_NO, KC_F5, KC_F6, KC_F7, KC_F8},
        {KC_LCTL, KC_NO, KC_F9, KC_F10, KC_F11, KC_F12},
        {KC_NO, KC_NO, KC_NO, KC_LALT, MO(3), KC_SPC},
        {KC_6, KC_7, KC_8, KC_9, KC_0, KC_PGUP},
        {KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, RGB_VAD, RGB_VAI},
        {KC_HOME, KC_F1, KC_F2, KC_F3, KC_F4, KC_PGDN},
        {KC_NO, KC_NO, KC_NO, KC_BSPC, KC_TRNS, KC_LSFT}
    };
    if (row < 8 && col < 6) return layer2_actual[row][col];
    return KC_NO;
}

typedef enum {
    L2_CAT_NUMBER,
    L2_CAT_FUNCTION,
    L2_CAT_ARROW,
    L2_CAT_MOD,
    L2_CAT_OTHER
} layer2_category_t;

layer2_category_t get_layer2_category(uint16_t keycode) {
    // Check mod keys first
    if (keycode == KC_ESC || keycode == KC_LSFT || keycode == KC_LCTL ||
        keycode == KC_LALT || keycode == KC_SPC || keycode == KC_BSPC ||
        keycode == KC_TRNS || keycode == MO(3) || keycode == KC_NO) {
        return L2_CAT_MOD;
    }

    // Check numbers
    if (keycode == KC_0 || keycode == KC_1 || keycode == KC_2 ||
        keycode == KC_3 || keycode == KC_4 || keycode == KC_5 ||
        keycode == KC_6 || keycode == KC_7 || keycode == KC_8 ||
        keycode == KC_9) {
        return L2_CAT_NUMBER;
    }

    if (keycode >= KC_F1 && keycode <= KC_F12) {
        return L2_CAT_FUNCTION;
    }

    if (keycode == KC_LEFT || keycode == KC_DOWN ||
        keycode == KC_UP || keycode == KC_RGHT) {
        return L2_CAT_ARROW;
    }

    return L2_CAT_OTHER;
}

// ============================================================================
// LAYER 3: SETTINGS
// ============================================================================

typedef enum {
    L3_CAT_RGB,
    // L3_CAT_MEDIA,
    L3_CAT_GAMING,
    L3_CAT_DEFAULT,
    L3_CAT_SYSTEM,
    L3_CAT_MOD,
    L3_CAT_OTHER
} layer3_category_t;

uint16_t get_layer3_keycode(uint8_t row, uint8_t col) {
    const uint16_t layer3_actual[8][6] = {
        {QK_BOOT, RGB_TOG_CUSTOM, GAMING_MODE, DEFAULT_MODE, XXXXXXX, XXXXXXX},
        {XXXXXXX, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX, XXXXXXX},
        {XXXXXXX, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX, XXXXXXX},
        {XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX},
        {XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX},
        {XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX},
        {XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX},
        {XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX}
    };
    if (row < 8 && col < 6) return layer3_actual[row][col];
    return KC_NO;
}

layer3_category_t get_layer3_category(uint16_t keycode) {
    // RGB controls
    if (keycode == RGB_TOG_CUSTOM) {
        return L3_CAT_RGB;
    }

    // System / Mode controls
    if (keycode == QK_BOOT) {
        return L3_CAT_SYSTEM;
    }

    // gaming mode
    if (keycode == GAMING_MODE) {
        return L3_CAT_GAMING;
    }

    // default mode
    if (keycode == DEFAULT_MODE) {
        return L3_CAT_DEFAULT;
    }

    // Mod / navigation keys
    if (keycode == KC_TRNS || keycode == KC_BSPC || keycode == KC_SPC ||
        keycode == KC_LALT || keycode == KC_LSFT || keycode == KC_NO) {
        return L3_CAT_MOD;
    }

    return L3_CAT_OTHER;
}

// ============================================================================
// LAYER 4: GAMING LAYER 0 (QWERTY)
// ============================================================================

typedef enum {
    L4_CAT_MOD,
    L4_CAT_OTHER
} layer4_category_t;

uint16_t get_layer4_keycode(uint8_t row, uint8_t col) {
    const uint16_t layer4_actual[8][6] = {
        {KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T},
        {KC_LSFT, KC_A, KC_S, KC_D, KC_F, KC_G},
        {KC_LCTL, KC_Z, KC_X, KC_C, KC_V, KC_B},
        {KC_LALT, MO(5), KC_SPC, KC_BSPC, MO(2), KC_LSFT},
        {KC_Y, KC_U, KC_I, KC_O, KC_P, KC_DEL},
        {KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_ENT},
        {KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_QUOT},
        {KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO}
    };
    if (row < 8 && col < 6) return layer4_actual[row][col];
    return KC_NO;
}

layer4_category_t get_layer4_category(uint16_t keycode) {
    if (keycode == KC_TAB || keycode == KC_ENT || keycode == KC_SPC ||
        keycode == KC_BSPC || keycode == KC_LALT || keycode == KC_LSFT ||
        keycode == KC_LCTL || keycode == KC_TRNS || keycode == MO(2) ||
        keycode == MO(5)) {
        return L4_CAT_MOD;
    }
    return L4_CAT_OTHER;
}

// // ============================================================================
// // LAYER 5: GAMING LAYER 1 (NUMBERS & ARROWS)
// // ============================================================================
//
// typedef enum {
//     L5_CAT_NUMBER,
//     L5_CAT_ARROW,
//     L5_CAT_MOD,
//     L5_CAT_OTHER
// } layer5_category_t;
//
// uint16_t get_layer5_keycode(uint8_t row, uint8_t col) {
//     const uint16_t layer5_actual[8][6] = {
//         {KC_TAB, KC_1, KC_2, KC_3, KC_4, KC_5},
//         {KC_ESC, KC_HASH, KC_DLR, KC_LCBR, KC_RCBR, KC_GRV},
//         {KC_LCTL, KC_PERC, KC_CIRC, KC_LBRC, KC_RBRC, KC_TILD},
//         {KC_LALT, _______, KC_SPC, KC_BSPC, MO(3), KC_0},
//         {KC_6, KC_7, KC_8, KC_9, KC_0, KC_GRV},
//         {XXXXXXX, KC_4, KC_UP, KC_6, KC_EQL, KC_MINS},
//         {KC_AMPR, KC_LEFT, KC_DOWN, KC_RIGHT, KC_DOT, KC_QUOT},
//         {KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO}
//     };
//     if (row < 8 && col < 6) return layer5_actual[row][col];
//     return KC_NO;
// }
//
// layer5_category_t get_layer5_category(uint16_t keycode) {
//     // Mod keys
//     if (keycode == KC_ESC || keycode == KC_LCTL || keycode == KC_LALT ||
//         keycode == KC_SPC || keycode == KC_BSPC || keycode == KC_TRNS ||
//         keycode == MO(3) || keycode == KC_NO) {
//         return L5_CAT_MOD;
//     }
//
//     // Numbers
//     if (keycode >= KC_0 && keycode <= KC_9) {
//         return L5_CAT_NUMBER;
//     }
//
//     // Arrows
//     if (keycode == KC_LEFT || keycode == KC_DOWN ||
//         keycode == KC_UP || keycode == KC_RGHT) {
//         return L5_CAT_ARROW;
//     }
//
//     return L5_CAT_OTHER;
// }
//
//
// ============================================================================
// RGB MATRIX EFFECTS
// ============================================================================

#ifdef RGB_MATRIX_ENABLE

void keyboard_post_init_user(void) {
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_REACTIVE_SIMPLE);
    rgb_matrix_sethsv_noeeprom(L0_KEY_H, L0_KEY_S, L0_KEY_V);
}

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);
    switch (layer) {
        case 0:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_REACTIVE_SIMPLE);
            rgb_matrix_sethsv_noeeprom(L0_KEY_H, L0_KEY_S, L0_KEY_V);
            break;
        case 1:
        case 5:
        case 2:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_BREATHING);
            rgb_matrix_sethsv_noeeprom(0, 0, 0);
            rgb_matrix_set_speed_noeeprom(BREATHING_SPEED);
            break;
        case 3:
            rgb_matrix_sethsv_noeeprom(0, 0, 0);
            rgb_matrix_set_speed_noeeprom(BREATHING_SPEED);
            rgb_matrix_mode_noeeprom(RGB_MATRIX_NONE);
            break;
        case 4:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_REACTIVE_SIMPLE);
            rgb_matrix_sethsv_noeeprom(L0_MOD_H, L0_MOD_S, L0_MOD_V);
            break;
    }
    return state;
}

// Fast breathing effect that resets on shift press
static uint8_t breathing_brightness(uint8_t base_val, uint16_t offset_time) {
    uint16_t elapsed = timer_elapsed(offset_time);
    uint8_t phase = (elapsed / 3) & 0xFF;  // Much faster cycle
    float breathing = (sin((phase / 255.0) * 2 * 3.14159) + 1.0) / 2.0;
    return (uint8_t)(base_val * breathing);
}


bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {

    if (!rgb_enabled) {
        rgb_matrix_set_color_all(0, 0, 0);
        return false;
    }

    uint8_t layer = get_highest_layer(layer_state);

    // Handle underglow
    HSV underglow_hsv = {UNDERGLOW_H, UNDERGLOW_S, UNDERGLOW_V};
    RGB underglow_rgb = hsv_to_rgb(underglow_hsv);

    for (uint8_t i = UNDERGLOW_LEFT_START; i < UNDERGLOW_LEFT_END; i++) {
        if (i >= led_min && i < led_max) {
            rgb_matrix_set_color(i, underglow_rgb.r, underglow_rgb.g, underglow_rgb.b);
        }
    }
    for (uint8_t i = UNDERGLOW_RIGHT_START; i < UNDERGLOW_RIGHT_END; i++) {
        if (i >= led_min && i < led_max) {
            rgb_matrix_set_color(i, underglow_rgb.r, underglow_rgb.g, underglow_rgb.b);
        }
    }

    if (layer == 0) {
        // Normal layer 0 (no breathing)
        for (uint8_t row = 0; row < 8; row++) {
            for (uint8_t col = 0; col < 6; col++) {
                uint8_t led = get_led_from_matrix(row, col);
                if (led == 255 || led >= led_max || led < led_min) continue;

                if (is_layer0_mod(row, col)) {
                    HSV hsv = {L0_MOD_H, L0_MOD_S, L0_MOD_V};
                    RGB rgb = hsv_to_rgb(hsv);
                    rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
                }
            }
        }

        uint8_t layer1_led = get_led_from_matrix(3, 4);
        uint8_t layer2_led = get_led_from_matrix(7, 4);
        if (layer1_led != 255 && layer1_led >= led_min && layer1_led < led_max) {
            rgb_matrix_set_color(layer1_led, 0, 0, 0);
        }
        if (layer2_led != 255 && layer2_led >= led_min && layer2_led < led_max) {
            rgb_matrix_set_color(layer2_led, 0, 0, 0);
        }
    }
    else if (layer == 1) {
        for (uint8_t row = 0; row < 8; row++) {
            for (uint8_t col = 0; col < 6; col++) {
                uint16_t keycode = get_layer1_keycode(row, col);
                if (keycode == KC_NO) continue;

                layer1_category_t cat = get_layer1_category(keycode);
                uint8_t led = get_led_from_matrix(row, col);
                if (led == 255 || led >= led_max || led < led_min) continue;

                HSV hsv;
                bool apply_breathing = false;

                switch (cat) {
                    case L1_CAT_NUMBER:
                        hsv = (HSV){L1_NUMBERS_H, L1_NUMBERS_S, L1_NUMBERS_V};
                        apply_breathing = true;  // Numbers breathe
                        break;
                    case L1_CAT_BRACKET:
                        hsv = (HSV){L1_BRACKETS_H, L1_BRACKETS_S, L1_BRACKETS_V};
                        apply_breathing = true;  // Brackets breathe
                        break;
                    case L1_CAT_SYMBOL:
                        hsv = (HSV){L1_SYMBOLS_H, L1_SYMBOLS_S, L1_SYMBOLS_V};
                        break;
                    case L1_CAT_MOD:
                        hsv = (HSV){L1_MOD_H, L1_MOD_S, L1_MOD_V};
                        break;
                }

                if (apply_breathing) {
                    hsv.v = breathing_brightness(hsv.v, 0);
                }

                RGB rgb = hsv_to_rgb(hsv);
                rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
            }
        }

        uint8_t layer1_led = get_led_from_matrix(3, 4);
        uint8_t layer2_led = get_led_from_matrix(7, 4);
        if (layer1_led != 255) {
            HSV hsv = {LAYER_IND_H, LAYER_IND_S, LAYER_IND_V};
            RGB rgb = hsv_to_rgb(hsv);
            rgb_matrix_set_color(layer1_led, rgb.r, rgb.g, rgb.b);
        }
        if (layer2_led != 255) rgb_matrix_set_color(layer2_led, 0, 0, 0);
    }
    else if (layer == 2) {
        for (uint8_t row = 0; row < 8; row++) {
            for (uint8_t col = 0; col < 6; col++) {
                uint16_t keycode = get_layer2_keycode(row, col);
                if (keycode == KC_NO) continue;

                layer2_category_t cat = get_layer2_category(keycode);
                uint8_t led = get_led_from_matrix(row, col);
                if (led == 255 || led >= led_max || led < led_min) continue;


                HSV hsv;
                bool apply_breathing = false;

                switch (cat) {
                    case L2_CAT_NUMBER:
                        hsv = (HSV){L2_NUMBERS_H, L2_NUMBERS_S, L2_NUMBERS_V};
                        break;
                    case L2_CAT_FUNCTION:
                        hsv = (HSV){L2_FUNCTION_H, L2_FUNCTION_S, L2_FUNCTION_V};
                        break;
                    case L2_CAT_ARROW:
                        hsv = (HSV){L2_ARROWS_H, L2_ARROWS_S, L2_ARROWS_V};
                        apply_breathing = true;  // Arrows breathe
                        break;
                    case L2_CAT_MOD:
                        hsv = (HSV){L2_MOD_H, L2_MOD_S, L2_MOD_V};
                        break;
                    case L2_CAT_OTHER:
                        hsv = (HSV){L2_OTHERS_H, L2_OTHERS_S, L2_OTHERS_V};
                        break;
                }

                if (apply_breathing) {
                    hsv.v = breathing_brightness(hsv.v, 0);
                }

                RGB rgb = hsv_to_rgb(hsv);
                rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
            }
        }
    }
    else if (layer == 3) {
        for (uint8_t row = 0; row < 8; row++) {
            for (uint8_t col = 0; col < 6; col++) {
                uint16_t keycode = get_layer3_keycode(row, col);
                if (keycode == KC_NO) continue;

                layer3_category_t cat = get_layer3_category(keycode);
                uint8_t led = get_led_from_matrix(row, col);
                if (led == 255 || led >= led_max || led < led_min) continue;

                HSV hsv;
                bool apply_breathing = false;

                switch (cat) {
                    case L3_CAT_SYSTEM:
                        hsv = (HSV){0, 255, 100}; // RED
                        break;
                    case L3_CAT_GAMING:
                        hsv = (HSV){L0_MOD_H, L0_MOD_S, L0_MOD_V};
                        apply_breathing = true;
                        break;
                    case L3_CAT_DEFAULT:
                        hsv = (HSV){L0_KEY_H, L0_KEY_S, L0_KEY_V};
                        apply_breathing = true;
                        break;
                    case L3_CAT_RGB:
                        hsv = (HSV){144, 255, 100};
                        break;
                    case L3_CAT_MOD:
                        hsv = (HSV){214, 255, 100};
                        break;
                    case L3_CAT_OTHER:
                        hsv = (HSV){0, 0, 0};
                        break;
                }

                if (apply_breathing) {
                    hsv.v = breathing_brightness(hsv.v, 0);
                }

                RGB rgb = hsv_to_rgb(hsv);
                rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
            }
        }

        uint8_t layer1_led = get_led_from_matrix(3, 4);
        uint8_t layer2_led = get_led_from_matrix(7, 4);
        if (layer1_led != 255) rgb_matrix_set_color(layer1_led, 0, 0, 0);
        if (layer2_led != 255) {
            HSV hsv = {LAYER_IND_H, LAYER_IND_S, LAYER_IND_V};
            RGB rgb = hsv_to_rgb(hsv);
            rgb_matrix_set_color(layer2_led, rgb.r, rgb.g, rgb.b);
        }
    }
    else if (layer == 4) {
        for (uint8_t row = 0; row < 8; row++) {
            for (uint8_t col = 0; col < 6; col++) {
                uint16_t keycode = get_layer4_keycode(row, col);
                if (keycode == KC_NO) continue;

                layer4_category_t cat = get_layer4_category(keycode);
                uint8_t led = get_led_from_matrix(row, col);
                if (led == 255 || led >= led_max || led < led_min) continue;

                HSV hsv;
                bool apply_breathing = false;

                switch (cat) {
                    case L4_CAT_MOD:
                        hsv = (HSV){L0_KEY_H, L0_KEY_S, L0_KEY_V};
                        break;
                    case L4_CAT_OTHER:
                        hsv = (HSV){L0_MOD_H, L0_MOD_S, L0_MOD_V};
                        break;
                }

                if (apply_breathing) {
                    hsv.v = breathing_brightness(hsv.v, 0);
                }

                RGB rgb = hsv_to_rgb(hsv);
                rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
            }
        }

        uint8_t layer1_led = get_led_from_matrix(3, 4);
        uint8_t layer2_led = get_led_from_matrix(7, 4);
        if (layer1_led != 255) rgb_matrix_set_color(layer1_led, 0, 0, 0);
        if (layer2_led != 255) {
            HSV hsv = {LAYER_IND_H, LAYER_IND_S, LAYER_IND_V};
            RGB rgb = hsv_to_rgb(hsv);
            rgb_matrix_set_color(layer2_led, rgb.r, rgb.g, rgb.b);
        }
    }

    return false;
}

#endif // RGB_MATRIX_ENABLE
