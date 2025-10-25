// Copyright 2023 Danny Nguyen (@nooges)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "eeprom.h"
#include "timer.h"
#include "rgb_matrix.h"

// Named HSV tuples for readability
#define WHITE       0, 0
#define RED         0, 255
#define GREEN      85, 255
#define BLUE      170, 255
#define PURPLE    191, 255

// standard alias
#define LSG_S LSG(KC_S)

// mod tap aliases
#define BRM_Z MT(MOD_LCTL, KC_Z)
#define BRM_X MT(MOD_LALT, KC_X)
#define BRM_C MT(MOD_LGUI, KC_C)
#define BRM_COMM MT(MOD_RGUI, KC_COMM)
#define BRM_DOT MT(MOD_RALT, KC_DOT)
#define BRM_SLSH MT(MOD_RCTL, KC_SLSH)

// layer/mod tap aliases
#define NAV_BS LT(NAV, KC_BSPC)
#define NAV_SPC LT(NAV, KC_SPC)
#define NUM_ENT LT(NUM, KC_ENT)
#define MT_TAB MT(MOD_LSFT, KC_TAB)

static uint8_t brightness = 64;
static uint16_t brightness_timer = 0;

enum custom_layers {
     DFLT,
     GAME,
     NAV,
     SYS,
     NUM,
     CFG
};

/**
 * KEYMAP
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [DFLT] = LAYOUT(
    // ┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
        QK_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        KC_TAB,  KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,                               KC_J,    KC_L,    KC_U,    KC_Y,    KC_QUOT, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        KC_ESC,  KC_A,    KC_R,    KC_S,    KC_T,    KC_G,                               KC_M,    KC_N,    KC_E,    KC_I,    KC_O,    _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
        KC_LCTL, BRM_Z,   BRM_X,   BRM_C,   KC_D,    KC_V,    MO(SYS),          MO(CFG), KC_K,    KC_H,    BRM_COMM,BRM_DOT, BRM_SLSH,KC_ENT,
    // └────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                       KC_ESC,  MT_TAB,  NAV_BS,                    NAV_SPC, NUM_ENT, KC_LALT
    //                                └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [GAME] = LAYOUT(
    // ┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
        _______, _______, _______, _______, _______, _______,                            _______, _______, _______, _______, _______, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_I,    KC_Q,    KC_W,    KC_E,    KC_R,                               _______, _______, _______, _______, _______, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_LSFT, KC_A,    KC_S,    KC_D,    KC_F,                               _______, _______, _______, _______, _______, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_M,    KC_Z,    KC_X,    KC_C,    KC_V,    MO(SYS),          MO(CFG), _______, _______, _______, _______, _______, _______,
    // └────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                       _______, MO(NAV), KC_SPC,                    _______, _______, _______
    //                                └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [NAV] = LAYOUT(
    // ┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
        KC_TILDE,KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                            KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_TAB,  KC_BSPC, KC_UP,   KC_DEL,  KC_LBRC,                            KC_RBRC, KC_GRV,  KC_PLUS, KC_PIPE, KC_DQUO, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_LSFT, KC_LEFT, KC_DOWN, KC_RIGHT,KC_LPRN,                            KC_RPRN, KC_MINS, KC_EQL,  KC_BSLS, KC_SCLN, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_LGUI, _______, KC_HOME, KC_END,  KC_LCBR, _______,          _______, KC_RCBR, KC_UNDS, KC_LT,   KC_GT,   KC_QUES, _______,
    // └────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                       _______, _______, _______,                   _______, _______, _______
    //                                └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [SYS] = LAYOUT(
    // ┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
        KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, LSG_S,   KC_VOLD, KC_MUTE, KC_VOLU, KC_BRIU,                            RGB_VAI, _______, _______, _______, _______, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, _______, KC_MPRV, KC_MPLY, KC_MNXT, KC_BRID,                            RGB_VAD, _______, _______, _______, _______, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
        _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______, _______,
    // └────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                       QK_BOOT, _______, _______,                   _______, _______, QK_BOOT
    //                                └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [NUM] = LAYOUT(
    // ┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
        KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                            KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F11,           KC_F12,  KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  _______,
    // └────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                       _______, _______, _______,                   _______, _______, _______
    //                                └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [CFG] = LAYOUT(
    // ┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
        _______, TO(DFLT),TO(GAME),_______, _______, _______,                            _______, _______, _______, _______, _______, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, _______, _______, _______, _______, _______,                            _______, _______, _______, _______, _______, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, _______, _______, _______, _______, _______,                            _______, _______, _______, _______, _______, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
        _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______, _______,
    // └────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                       QK_BOOT, _______, _______,                   _______, _______, QK_BOOT
    //                                └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    //[NEW] = LAYOUT(
    // ┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐

    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤

    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤

    // ├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤

    // └────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘

    //                                └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    //),
};

/**
 * Tapping Term
 * Maximum time between key press and release to be considered a tap
 */
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MT(MOD_RGUI, KC_COMM):
        case MT(MOD_RALT, KC_DOT):
        case MT(MOD_RCTL, KC_SLSH):
            return TAPPING_TERM + 100;
        case MT(MOD_LALT, KC_X):
        case MT(MOD_LGUI, KC_C):
        case MT(MOD_LCTL, KC_Z):
            return TAPPING_TERM + 200;
        default:
            return TAPPING_TERM;
    }
}

/**
 * Quick Tap Term
 * Time to treat a tap-hold key as a tap if a second key is pressed quickly after it
 * Enables tap-repeat and prevents accidental holds
 */
uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MT(MOD_LSFT, KC_TAB):
        case LT(NAV, KC_SPC):
        case LT(NAV, KC_BSPC):
            return 200;
        case MT(MOD_LALT, KC_X):
        case MT(MOD_LGUI, KC_C):
        case MT(MOD_LCTL, KC_Z):
            return 300;
        default:
            return 0;
    }
}

/**
 * Get hold on other key press
 * If a key is held when another is pressed, treat it as a hold, even if it's within the tapping term
 */
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MT(MOD_LSFT, KC_TAB):
            return true;
        default:
            return false;
    }
}

/**
 * Permissive Hold
 * Favor hold when another key is pressed during the tapping term
 */
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MT(MOD_LSFT, KC_TAB):
        case LT(NAV, KC_SPC):
        case LT(NAV, KC_BSPC):
            return true;
        default:
            return false;
    }
}

/**
 * Retro Tapping
 * Wait until a key is released to determine if it should be a tap or hold action
 */
bool get_retro_tapping(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MT(MOD_LCTL, KC_Z):
        case MT(MOD_LALT, KC_X):
        case MT(MOD_LGUI, KC_C):
        case MT(MOD_RGUI, KC_COMM):
        case MT(MOD_RALT, KC_DOT):
        case MT(MOD_RCTL, KC_SLSH):
            return true;
        default:
            return false;
    }
}

/**
 * Auto Shift
 * Selectively enable autoshift for specific layers & keys
 */
bool get_auto_shifted_key(uint16_t keycode, keyrecord_t *record) {
    uint8_t layer = get_highest_layer(layer_state);
    switch(layer) {
        case NAV:
            switch (keycode) {
                case KC_GRV:
                case KC_MINS:
                case KC_EQL:
                case KC_BSLS:
                case KC_SCLN:
                    return true;
            }
            break;
        case NUM:
            switch (keycode) {
                case KC_1:
                case KC_2:
                case KC_3:
                case KC_4:
                case KC_5:
                case KC_6:
                case KC_7:
                case KC_8:
                case KC_9:
                case KC_0:
                    return true;
            }
            break;
    }
    return false;
}

/**
 * RGB SETTINGS
 */
void keyboard_post_init_user(void) {
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    eeconfig_read_rgb_matrix(&rgb_matrix_config);
    brightness = rgb_matrix_get_val();
    rgb_matrix_sethsv_noeeprom(WHITE, brightness);
}

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);
    brightness = rgb_matrix_get_val();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);

    switch (layer) {
        case DFLT:
            rgb_matrix_sethsv_noeeprom(WHITE, brightness);
            break;
        case GAME:
            rgb_matrix_sethsv_noeeprom(RED, brightness);
            break;
        case NAV:
            rgb_matrix_sethsv_noeeprom(BLUE, brightness);
            break;
        case NUM:
            rgb_matrix_sethsv_noeeprom(PURPLE, brightness);
            break;
    }
    return state;
}

/**
 * per key handling
 */

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case RGB_VAI:
            case RGB_VAD:
                brightness = rgb_matrix_get_val();
                brightness_timer = timer_read();  // debounce
                break;
        }
    }
}

void matrix_scan_user(void) {
    // save brighness to eeprom after 1s of no changes
    if (brightness_timer && timer_elapsed(brightness_timer) > 1000) {
        eeconfig_update_rgb_matrix(&rgb_matrix_config);  // writes current config from internal state
        brightness_timer = 0;
    }
}
