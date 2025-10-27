// Copyright 2023 Danny Nguyen (@nooges)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum_keycodes.h"
#include QMK_KEYBOARD_H
#include "eeprom.h"
#include "timer.h"

// Named HSV tuples for readability
#define WHITE       0, 0
#define RED         0, 255
#define GREEN      85, 255
#define BLUE      170, 255
#define PURPLE    191, 255

// key tap aliases
#define LSG_S LSG(KC_S)

// home row mod aliases
#define HRM_A MT(MOD_LCTL, KC_A)
#define HRM_R MT(MOD_LALT, KC_R)
#define HRM_S MT(MOD_LGUI, KC_S)
#define HRM_T MT(MOD_LSFT, KC_T)

#define HRM_O MT(MOD_RCTL, KC_O)
#define HRM_I MT(MOD_LALT, KC_I)
#define HRM_E MT(MOD_RGUI, KC_E)
#define HRM_N MT(MOD_RSFT, KC_N)

// layer tap aliases
#define NAV_BS LT(NAV, KC_BSPC)
#define NAV_SPC LT(NAV, KC_SPC)
#define NUM_ENT LT(NUM, KC_ENT)
#define SYS_ESC LT(SYS, KC_ESC)
#define SYS_QUOT LT(SYS, KC_QUOT)

static uint8_t brightness = 64;
static uint16_t brightness_timer = 0;

enum custom_layers {
     DFLT,
     GAME,
     NAV,
     SYS,
     NUM
};

/**
 * KEYMAPs
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [DFLT] = LAYOUT(
    // ┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
        KC_TAB,  KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,                               KC_J,    KC_L,    KC_U,    KC_Y,    KC_QUOT, KC_BSLS,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        SYS_ESC, HRM_A,   HRM_R,   HRM_S,   HRM_T,   KC_G,                               KC_M,    HRM_N,   HRM_E,   HRM_I,   HRM_O,   SYS_QUOT,
    // ├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
        KC_MEH,  KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,    NUM_ENT,          NUM_ENT, KC_K,    KC_H,    KC_COMM, KC_DOT,  KC_SLSH, KC_MEH,
    // └────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                       KC_HYPR, KC_LSFT, NAV_BS,                    NAV_SPC, MO(NUM), KC_HYPR
    //                                └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [GAME] = LAYOUT(
    // ┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
        _______, KC_I,    KC_Q,    KC_W,    KC_E,    KC_R,                               _______, _______, _______, _______, _______, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_LSFT, KC_A,    KC_S,    KC_D,    KC_F,                               _______, _______, _______, _______, _______, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_Z,    KC_X,    KC_C,    KC_M,    KC_V,    NUM_ENT,          _______, _______, _______, _______, _______, _______, _______,
    // └────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                       _______, MO(NAV), KC_SPC,                    _______, _______, _______
    //                                └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [NAV] = LAYOUT(
    // ┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
        _______, _______, KC_BSPC, KC_UP,   KC_DEL,  KC_LBRC,                            KC_RBRC, KC_GRV,  KC_PLUS, KC_PIPE, KC_COLN, KC_PIPE,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_LSFT, KC_LEFT, KC_DOWN, KC_RIGHT,KC_LPRN,                            KC_RPRN, KC_MINS, KC_EQL,  KC_BSLS, KC_SCLN, KC_DQUO,
    // ├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_LGUI, _______, KC_HOME, KC_END,  KC_LCBR, _______,          _______, KC_RCBR, KC_UNDS, KC_LT,   KC_GT,   KC_QUES, _______,
    // └────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                       _______, _______, _______,                   _______, _______, _______
    //                                └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [SYS] = LAYOUT(
    // ┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
        _______, TO(DFLT),TO(GAME),_______, _______, _______,                            _______, _______, _______, _______, _______, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, _______, KC_VOLU, KC_MPLY, KC_MNXT, KC_BRIU,                            RM_VALU, _______, _______, _______, _______, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
        QK_BOOT, _______, KC_VOLD, KC_MUTE, KC_MPRV, KC_BRID, _______,          _______, RM_VALD, _______, _______, _______, _______, QK_BOOT,
    // └────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                       _______, _______, _______,                   _______, _______, _______
    //                                └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [NUM] = LAYOUT(
    // ┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
        _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                            KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
        _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______,
    // ├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
        KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______,          _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
    // └────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                       _______, _______, _______,                   _______, _______, _______
    //                                └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    //[NEW] = LAYOUT(
    // ┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐

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
        // left hand home row mods
        case MT(MOD_LCTL, KC_A):
        case MT(MOD_LGUI, KC_R):
        case MT(MOD_LALT, KC_S):
        case MT(MOD_LSFT, KC_T):
        // right hand home row mods
        case MT(MOD_RCTL, KC_O):
        case MT(MOD_LALT, KC_I):
        case MT(MOD_RGUI, KC_E):
        case MT(MOD_RSFT, KC_N):
            return TAPPING_TERM + 100;
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
        case LT(NAV, KC_SPC):
        case LT(NAV, KC_BSPC):
        // left hand home row mods
        case MT(MOD_LCTL, KC_A):
        case MT(MOD_LGUI, KC_R):
        case MT(MOD_LALT, KC_S):
        case MT(MOD_LSFT, KC_T):
        // right hand home row mods
        case MT(MOD_RCTL, KC_O):
        case MT(MOD_LALT, KC_I):
        case MT(MOD_RGUI, KC_E):
        case MT(MOD_RSFT, KC_N):
            return TAPPING_TERM;
        default:
            return 0;
    }
}

/**
 * Flow Tap
 * biases tap-hold keys toward tap (and enables auto-repeat) if pressed right after another key
 */
uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record, uint16_t prev_keycode) {
    switch (get_tap_keycode(keycode)) {
        case KC_SPC:
        case KC_BSPC:
        case KC_ENT:
        case KC_TAB:
            return 0;
        default:
            return FLOW_TAP_TERM;
    }
}

/**
 * Permissive Hold
 * Favor hold when another key is pressed during the tapping term
 */
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
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
        // left hand home row mods
        case MT(MOD_LCTL, KC_A):
        case MT(MOD_LGUI, KC_R):
        case MT(MOD_LALT, KC_S):
        case MT(MOD_LSFT, KC_T):
        // right hand home row mods
        case MT(MOD_RCTL, KC_O):
        case MT(MOD_LALT, KC_I):
        case MT(MOD_RGUI, KC_E):
        case MT(MOD_RSFT, KC_N):
            return true;
        default:
            return false;
    }
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
 * per keny handling
 */
void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case RM_VALU:
            case RM_VALD:
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
