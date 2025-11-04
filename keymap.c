#include QMK_KEYBOARD_H
#include "oneshot.h"
#include "transactions.h"

// ============================================================================
// CUSTOM KEYCODES
// ============================================================================
enum custom_keycodes {
    // One-shot modifiers (Callum's implementation)
    OS_SHFT = SAFE_RANGE,
    OS_CTRL,
    OS_ALT,
    OS_CMD,

    // Original custom keycodes
    RGB_TOG_CUSTOM,
    GAMING_MODE,
    DEFAULT_MODE,
    ARROW_R,
    ARROW_L
};

// ============================================================================
// SPLIT KEYBOARD SYNC SETUP
// ============================================================================

// Structure to hold synced data
typedef struct {
    bool rgb_enabled;
} user_runtime_config_t;

// Create instance
static user_runtime_config_t user_state = {
    .rgb_enabled = true
};

// Slave side handler - receives data from master
void user_sync_rgb_enabled_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const user_runtime_config_t *m2s = (const user_runtime_config_t*)in_data;
    user_state.rgb_enabled = m2s->rgb_enabled;
}

// ============================================================================
// STATE VARIABLES
// ============================================================================

// One-shot modifier states
oneshot_state os_shft_state = os_up_unqueued;
oneshot_state os_ctrl_state = os_up_unqueued;
oneshot_state os_alt_state = os_up_unqueued;
oneshot_state os_cmd_state = os_up_unqueued;

// Helper function to check if any oneshot mod is queued (waiting for next key)
bool is_oneshot_active(void) {
    return os_shft_state == os_up_queued ||
           os_ctrl_state == os_up_queued ||
           os_alt_state == os_up_queued ||
           os_cmd_state == os_up_queued;
}

void update_oneshot(
    oneshot_state *state,
    uint16_t mod,
    uint16_t trigger,
    uint16_t keycode,
    keyrecord_t *record
) {
    if (keycode == trigger) {
        if (record->event.pressed) {
            // Trigger keydown
            if (*state == os_up_unqueued) {
                register_code(mod);
            }
            *state = os_down_unused;
        } else {
            // Trigger keyup
            switch (*state) {
            case os_down_unused:
                // If we didn't use the mod while trigger was held, queue it.
                *state = os_up_queued;
                break;
            case os_down_used:
                // If we did use the mod while trigger was held, unregister it.
                *state = os_up_unqueued;
                unregister_code(mod);
                break;
            default:
                break;
            }
        }
    } else {
        if (record->event.pressed) {
            if (is_oneshot_cancel_key(keycode) && *state != os_up_unqueued) {
                // Cancel oneshot on designated cancel keydown.
                *state = os_up_unqueued;
                unregister_code(mod);
            }
        } else {
            if (!is_oneshot_ignored_key(keycode)) {
                // On non-ignored keyup, consider the oneshot used.
                switch (*state) {
                case os_down_unused:
                    *state = os_down_used;
                    break;
                case os_up_queued:
                    *state = os_up_unqueued;
                    unregister_code(mod);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

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
// COLOR CONFIGURATION
// ============================================================================

// LAYER 0 (Default Layer) Colors
#define L0_KEY_H        130
#define L0_KEY_S        0
#define L0_KEY_V        100

#define L0_MOD_H        130
#define L0_MOD_S        255
#define L0_MOD_V        100

// ONE-SHOT MODIFIER COLORS
#define OSM_QUEUED_H    170  // Purple/Blue when oneshot is queued/waiting
#define OSM_QUEUED_S    255
#define OSM_QUEUED_V    150

#define OSM_ACTIVE_H    43   // Yellow/Orange when modifier is active
#define OSM_ACTIVE_S    255
#define OSM_ACTIVE_V    120

// LAYER 1 (Numbers & Symbols) Colors
#define L1_NUMBERS_H    95
#define L1_NUMBERS_S    255
#define L1_NUMBERS_V    100

#define L1_BRACKETS_H   180
#define L1_BRACKETS_S   255
#define L1_BRACKETS_V   120

#define L1_SYMBOLS_H    43
#define L1_SYMBOLS_S    0
#define L1_SYMBOLS_V    100

#define L1_MOD_H        95
#define L1_MOD_S        255
#define L1_MOD_V        100

// LAYER 2 (Functions & Navigation) Colors
#define L2_NUMBERS_H    240
#define L2_NUMBERS_S    255
#define L2_NUMBERS_V    100

#define L2_FUNCTION_H   43
#define L2_FUNCTION_S   0
#define L2_FUNCTION_V   100

#define L2_ARROWS_H     180
#define L2_ARROWS_S     255
#define L2_ARROWS_V     120

#define L2_OTHERS_H     43
#define L2_OTHERS_S     0
#define L2_OTHERS_V     100

#define L2_MOD_H        190
#define L2_MOD_S        200
#define L2_MOD_V        100

// LAYER 3 (Gaming) Colors
#define L3_GAMING_H    110
#define L3_GAMING_S    255
#define L3_GAMING_V    100

#define L3_DEFAULT_H    240
#define L3_DEFAULT_S    255
#define L3_DEFAULT_V    50

#define L3_MOD_H        0
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

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Layer 0: Colemak-DH (Default)
    [0] = LAYOUT_split_3x6_3(
      KC_TAB,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,                         KC_J,    KC_L,    KC_U,    KC_Y, KC_SCLN,  KC_DEL,
      KC_ESC, KC_A,  KC_R,    KC_S,    KC_T,    KC_G,                         KC_M,    KC_N,    KC_E,    KC_I,    KC_O,  KC_ENT,
      KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,                         KC_K,    KC_H, KC_COMM,  KC_DOT, KC_SLSH, KC_QUOT,
                                          KC_LALT,   MO(1),  KC_SPC,    OS_SHFT,   MO(2), KC_BSPC
    ),
    // // Layer 1: Numbers & Symbols (Colemak)
    [1] = LAYOUT_split_3x6_3(
      KC_TAB, KC_EXLM,   KC_AT, KC_LPRN, KC_RPRN, KC_PIPE,                      KC_UNDS, KC_MINS, KC_PLUS, KC_EQL, KC_SLSH,  KC_DEL,
      KC_ESC, KC_HASH,  KC_DLR, KC_LCBR, KC_RCBR, ARROW_R,                      KC_SCLN, KC_COLN, KC_DQUO, KC_QUOT,  KC_GRV, KC_ENT,
      KC_LCTL, KC_PERC, KC_AMPR, KC_LBRC, KC_RBRC, ARROW_L,                     KC_TILD, KC_ASTR,  KC_LABK, KC_RABK,  KC_QUES, KC_BSLS,
                                          KC_LALT, _______,  KC_SPC,    OS_SHFT,   MO(3), KC_BSPC
    ),

// Layer 2: Functions & Navigation (Colemak)
    [2] = LAYOUT_split_3x6_3(
      KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_PGUP,
      KC_LSFT, OS_CTRL, OS_SHFT, OS_CMD, OS_ALT, XXXXXXX,                         KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, XXXXXXX, XXXXXXX,
      KC_LCTL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_HOME, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_PGDN,
                                          KC_LALT,   MO(3),  KC_SPC,    OS_SHFT, _______, KC_BSPC
    ),
// Layer 3: Settings (accessible from both modes)
    [3] = LAYOUT_split_3x6_3(
      QK_BOOT, RGB_TOG_CUSTOM, GAMING_MODE, DEFAULT_MODE, KC_ACL0, KC_ACL1,                       KC_ACL2, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, XXXXXXX,
      XXXXXXX,   KC_MS_L, KC_MS_U, KC_MS_D, KC_MS_R, KC_BTN1,                      KC_BTN2, KC_WH_D, KC_WH_U, KC_RSFT,  XXXXXXX, XXXXXXX,
      KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5, KC_F6,                         KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12,
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

    //
    // // Layer 1: Numbers & Symbols
    // // Changes:
    // // - Right home row (N, E, I, O) replaced with OS_CMD, OS_ALT, OS_CTRL, OS_SHFT
    // // - Symbols rearranged to accommodate one-shot mods
    // [1] = LAYOUT_split_3x6_3(
    //   KC_TAB, KC_EXLM,   KC_AT, KC_LPRN, KC_RPRN, KC_PIPE,                      KC_COLN, KC_MINS, KC_PLUS, KC_ASTR, KC_SLSH,  KC_DEL,
    //    KC_ESC, KC_HASH,  KC_DLR, KC_LCBR, KC_RCBR, ARROW_R,                     KC_SCLN,  OS_CMD,  OS_ALT, OS_CTRL, OS_SHFT,  KC_ENT,
    //   KC_LCTL, KC_PERC, KC_AMPR, KC_LBRC, KC_RBRC, ARROW_L,                     KC_TILD,  KC_EQL, KC_DQUO, KC_UNDS,  KC_GRV, KC_BSLS,
    //                                       KC_LALT, _______,  KC_SPC,    KC_BSPC,   MO(3), OS_SHFT
    // ),
    //
    // // Layer 2: Functions & Navigation
    // // Changes:
    // // - Right home row replaced with OS_CTRL, OS_SHFT, OS_CMD, OS_ALT
    // // - Function keys removed from right side to make room
    // [2] = LAYOUT_split_3x6_3(
    //    KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_PGUP,
    //   KC_LSFT, XXXXXXX,   KC_F5,   KC_F6,   KC_F7,   KC_F8,                     OS_CTRL, OS_SHFT,  OS_CMD,  OS_ALT, RGB_VAD, RGB_VAI,
    //   KC_LCTL, XXXXXXX,   KC_F9,  KC_F10,  KC_F11,  KC_F12,                     KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, KC_HOME, KC_PGDN,
    //                                       KC_LALT,   MO(3),  KC_SPC,    KC_BSPC, _______, OS_SHFT
    // ),

};

// ============================================================================
// ONE-SHOT MODIFIER IMPLEMENTATION
// ============================================================================

// Define which keys cancel oneshot mods
bool is_oneshot_cancel_key(uint16_t keycode) {
    switch (keycode) {
    // case MO(1):  // Layer 1 key
    // case MO(2):  // Layer 2 key
    case MO(3):  // Layer 3 key
    case MO(5):  // Layer 5 key
    case KC_BSPC:
        return true;
    default:
        return false;
    }
}

// Define which keys are ignored when determining if oneshot was used
bool is_oneshot_ignored_key(uint16_t keycode) {
    switch (keycode) {
    case MO(1):
    case MO(2):
    case MO(3):
    case MO(5):
    case KC_LSFT:
    case KC_LCTL:
    case KC_LALT:
    case KC_LGUI:  // KC_LGUI and KC_LCMD are the same
    case OS_SHFT:
    case OS_CTRL:
    case OS_ALT:
    case OS_CMD:
        return true;
    default:
        return false;
    }
}

// ============================================================================
// CUSTOM KEYCODE PROCESSING
// ============================================================================

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Process one-shot modifiers
    update_oneshot(&os_shft_state, KC_LSFT, OS_SHFT, keycode, record);
    update_oneshot(&os_ctrl_state, KC_LCTL, OS_CTRL, keycode, record);
    update_oneshot(&os_alt_state, KC_LALT, OS_ALT, keycode, record);
    update_oneshot(&os_cmd_state, KC_LGUI, OS_CMD, keycode, record);

    switch (keycode) {
        case RGB_TOG_CUSTOM:
            if (record->event.pressed) {
                user_state.rgb_enabled = !user_state.rgb_enabled;
                if (!user_state.rgb_enabled) {
                    rgb_matrix_set_color_all(0, 0, 0);
                }
            }
            return false;

        case GAMING_MODE:
            if (record->event.pressed) {
                layer_move(4);  // Switch to gaming layer
            }
            return false;

        case DEFAULT_MODE:
            if (record->event.pressed) {
                layer_move(0);  // Switch to default layer
            }
            return false;

        case ARROW_R:  // ->
            if (record->event.pressed) {
                SEND_STRING("->");
            }
            return false;

        case ARROW_L:  // <-
            if (record->event.pressed) {
                SEND_STRING("<-");
            }
            return false;
    }

    return true;
}

// ============================================================================
// RGB MATRIX EFFECTS
// ============================================================================

#ifdef RGB_MATRIX_ENABLE

// Breathing effect helper
static uint8_t breathing_brightness(uint8_t base_val, uint16_t offset_time) {
    uint16_t elapsed = timer_elapsed(offset_time);
    uint8_t phase = (elapsed / 3) & 0xFF;  // Much faster cycle
    float breathing = (sin((phase / 255.0) * 2 * 3.14159) + 1.0) / 2.0;
    return (uint8_t)(base_val * breathing);
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
        {KC_AMPR, KC_1, KC_LABK, KC_RABK, KC_SLSH, KC_QUOT},
        {KC_NO, KC_NO, KC_NO, KC_BSPC, MO(3), KC_BSPC}
    };
    if (row < 8 && col < 6) return layer1_actual[row][col];
    return KC_NO;
}

typedef enum {
    // L1_CAT_NUMBER,
    L1_CAT_BRACKET,
    L1_CAT_SYMBOL,
    L1_CAT_MOD
} layer1_category_t;

layer1_category_t get_layer1_category(uint16_t keycode) {
    // Check numbers
    // if (keycode == KC_0 || keycode == KC_1 || keycode == KC_2 ||
    //     keycode == KC_3 || keycode == KC_4 || keycode == KC_5 ||
    //     keycode == KC_6 || keycode == KC_7 || keycode == KC_8 ||
    //     keycode == KC_9) {
    //     return L1_CAT_NUMBER;
    // }

    // Check mod keys first
    if (keycode == KC_TAB || keycode == KC_ESC || keycode == KC_LCTL ||
        keycode == KC_LALT || keycode == KC_SPC || keycode == KC_BSPC ||
        keycode == KC_TRNS || keycode == MO(3) || keycode == KC_NO) {
        return L1_CAT_MOD;
    }

    // Check brackets
    if (keycode == KC_LPRN || keycode == KC_RPRN ||
        keycode == KC_LBRC || keycode == KC_RBRC ||
        keycode == KC_LABK || keycode == KC_RABK ||
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

// ============================================================================
// RGB MATRIX INDICATOR CALLBACK
// ============================================================================

void keyboard_post_init_user(void) {
    // Register the sync handler for split keyboard
    transaction_register_rpc(USER_SYNC_RGB_ENABLED, user_sync_rgb_enabled_slave_handler);

    // Initialize RGB
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_REACTIVE_SIMPLE);
    rgb_matrix_sethsv_noeeprom(L0_KEY_H, L0_KEY_S, L0_KEY_V);
}

// Sync custom data between split halves
void housekeeping_task_user(void) {
    if (is_keyboard_master()) {
        // Send the current state to slave
        transaction_rpc_send(USER_SYNC_RGB_ENABLED, sizeof(user_state), &user_state);
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);
    switch (layer) {
        case 0:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_REACTIVE_SIMPLE);
            if (user_state.rgb_enabled) {
                rgb_matrix_sethsv_noeeprom(L0_KEY_H, L0_KEY_S, L0_KEY_V);
            } else {
                rgb_matrix_sethsv_noeeprom(0, 0, 0);
            }
            break;
        case 1:
        case 5:
        case 2:
        case 3:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_BREATHING);
            rgb_matrix_sethsv_noeeprom(0, 0, 0);
            rgb_matrix_set_speed_noeeprom(BREATHING_SPEED);
            break;
        case 4:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_REACTIVE_SIMPLE);
            rgb_matrix_sethsv_noeeprom(L0_MOD_H, L0_MOD_S, L0_MOD_V);
            break;
    }
    return state;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = get_highest_layer(layer_state);

    // Handle underglow
    HSV underglow_hsv = {UNDERGLOW_H, UNDERGLOW_S, UNDERGLOW_V};
    RGB underglow_rgb = user_state.rgb_enabled ? hsv_to_rgb(underglow_hsv) : (RGB){0, 0, 0};
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
                    RGB rgb = user_state.rgb_enabled ? hsv_to_rgb(hsv) : (RGB){0, 0, 0};
                    rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
                }
            }
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
                    // case L1_CAT_NUMBER:
                    //     hsv = (HSV){L1_NUMBERS_H, L1_NUMBERS_S, L1_NUMBERS_V};
                    //     apply_breathing = true;  // Numbers breathe
                    //     break;
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
                if (apply_breathing && user_state.rgb_enabled) {
                    hsv.v = breathing_brightness(hsv.v, 0);
                }
                RGB rgb = user_state.rgb_enabled ? hsv_to_rgb(hsv) : (RGB){0, 0, 0};
                rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
            }
        }
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
                        hsv = (HSV){L1_NUMBERS_H, L1_NUMBERS_S, L1_NUMBERS_V};
                        apply_breathing = true;
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
                if (apply_breathing && user_state.rgb_enabled) {
                    hsv.v = breathing_brightness(hsv.v, 0);
                }
                RGB rgb = user_state.rgb_enabled ? hsv_to_rgb(hsv) : (RGB){0, 0, 0};
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
                        hsv = (HSV){0, 0, 100};
                        apply_breathing = true;
                        break;
                }
                if (apply_breathing && user_state.rgb_enabled) {
                    hsv.v = breathing_brightness(hsv.v, 0);
                }
                RGB rgb = user_state.rgb_enabled ? hsv_to_rgb(hsv) : (RGB){0, 0, 0};
                rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
            }
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
                if (apply_breathing && user_state.rgb_enabled) {
                    hsv.v = breathing_brightness(hsv.v, 0);
                }
                RGB rgb = user_state.rgb_enabled ? hsv_to_rgb(hsv) : (RGB){0, 0, 0};
                rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
            }
        }
        // uint8_t layer1_led = get_led_from_matrix(3, 4);
        // uint8_t layer2_led = get_led_from_matrix(7, 4);
        // if (layer1_led != 255) rgb_matrix_set_color(layer1_led, 0, 0, 0);
        // if (layer2_led != 255) {
        //     HSV hsv = {LAYER_IND_H, LAYER_IND_S, LAYER_IND_V};
        //     RGB rgb = hsv_to_rgb(hsv);
        //     rgb_matrix_set_color(layer2_led, rgb.r, rgb.g, rgb.b);
        // }
    }
    return false;
}

#endif // RGB_MATRIX_ENABLE
