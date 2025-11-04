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

    // // Disable RGB when suspended
    // #define RGB_DISABLE_WHEN_USB_SUSPENDED
    //
    // // Add timeout to save power
    // #define RGB_MATRIX_TIMEOUT 300000  // 5 minutes
#endif

// Enable custom split data sync for rgb_enabled variable and OSM states
#define SPLIT_TRANSACTION_IDS_USER USER_SYNC_RGB_ENABLED, USER_SYNC_OSM_STATE

#define DYNAMIC_KEYMAP_LAYER_COUNT 6

// // Tapping term for tap/hold keys (ESC/Shift)
// #define TAPPING_TERM 150
// // Optional: Add settings for better mod behavior
// #define PERMISSIVE_HOLD
// #define HOLD_ON_OTHER_KEY_PRESS
