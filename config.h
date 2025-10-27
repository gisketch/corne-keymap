/*
This is the c configuration file for the keymap

Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2015 Jack Humbert

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

#pragma once

// RGB Matrix Configuration
#ifdef RGB_MATRIX_ENABLE

    #define RGB_MATRIX_SPLIT { 27, 27 }  // for 54 total LEDs
    #define SPLIT_LAYER_STATE_ENABLE
    #define SPLIT_TRANSPORT_MIRROR

    // Enable reactive effects
    #define RGB_MATRIX_KEYPRESSES
    #define RGB_MATRIX_FRAMEBUFFER_EFFECTS

    // Enable specific effects
    #define ENABLE_RGB_MATRIX_SOLID_COLOR
    #define ENABLE_RGB_MATRIX_BREATHING
    #define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE

    // Performance settings
    #define RGB_MATRIX_LED_PROCESS_LIMIT (RGB_MATRIX_LED_COUNT + 4) / 5
    #define RGB_MATRIX_LED_FLUSH_LIMIT 16
    // RGB_MATRIX_MAXIMUM_BRIGHTNESS is defined in info.json as 120

    // Default settings - Reduced for power safety
    #define RGB_MATRIX_DEFAULT_VAL 80  // 66% brightness for layer 0
    #define RGB_MATRIX_HUE_STEP 8
    #define RGB_MATRIX_SAT_STEP 8
    #define RGB_MATRIX_VAL_STEP 8
    #define RGB_MATRIX_SPD_STEP 10

    // Disable RGB when suspended
    #define RGB_DISABLE_WHEN_USB_SUSPENDED

    // Add timeout to save power
    #define RGB_MATRIX_TIMEOUT 300000  // 5 minutes
#endif

#define DYNAMIC_KEYMAP_LAYER_COUNT 6

// Tapping term for tap/hold keys (ESC/Shift)
#define TAPPING_TERM 200

// Vial Configuration
#define VIAL_KEYBOARD_UID {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF}
#define VIAL_UNLOCK_COMBO_ROWS { 0, 4 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 0 }
// Press top-left keys on both halves to unlock Vial
