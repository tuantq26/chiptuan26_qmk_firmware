#include QMK_KEYBOARD_H
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "layers.h"
#include "keycodes.h"
#include "config.h"

bool     is_alt_tab_active = false;
uint16_t alt_tab_timer     = 0;

uint16_t w_2_timer    = 0;
uint16_t q_1_timer    = 0;
uint16_t time_hold_kc = 0;

uint8_t mod_state;

enum { TD_Q_TAB = 0, TD_RESET };

void safe_reset(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count >= 3) {
        // Reset the keyboard if you tap the key more than three times
        reset_keyboard();
        reset_tap_dance(state);
    }
}

qk_tap_dance_action_t tap_dance_actions[] = {[TD_Q_TAB] = ACTION_TAP_DANCE_DOUBLE(KC_Q, KC_TAB), [TD_RESET] = ACTION_TAP_DANCE_FN(safe_reset)};

LEADER_EXTERNS();

// Declare a boolean variable to keep track of whether any sequence
// will have been matched.
bool did_leader_succeed;

void matrix_scan_user(void) {
    if (is_alt_tab_active) {
        if (timer_elapsed(alt_tab_timer) > 1000) {
            unregister_code(KC_LALT);
            is_alt_tab_active = false;
        }
    }

    // Leader Key
    LEADER_DICTIONARY() {
        // Initialize did_leader_succeed as well as leading to be false
        did_leader_succeed = leading = false;
        // Replace the sequences below with your own sequences.
        SEQ_ONE_KEY(KC_T) {
            SEND_STRING(SS_LCTRL(SS_LSFT("t")));
            // In each sequence, set our flag to true. This way, we'll
            // know when any sequence was matched.
            did_leader_succeed = true;
        }
        SEQ_TWO_KEYS(KC_N, KC_T) {
            SEND_STRING(SS_LCTRL("t"));
            did_leader_succeed = true;
        }
        // Call leader_end at the end of the function, instead of at
        // the start. This way, we're sure we have set did_leader_succeed.
        leader_end();
    }
}

void leader_end(void) {
    if (did_leader_succeed) {
        // If any sequence was matched, did_leader_succeed will have
        // been set to true up in the matrix_scan_user function.
        // Put your code for a matched leader key sequence here.
    } else {
        // If no sequence was matched, did_leader_succeed will not
        // have been set to true anywhere, so we'll end up here.
        // Put your code for an unmatched leader key sequence here.
    }
}

uint16_t mapToNumber(uint16_t keycode) {
    uint16_t kcNumber;

    switch (keycode) {
        case MY_Q:
            kcNumber = KC_1;
            break;
        case MY_W:
            kcNumber = KC_2;
            break;
        case MY_E:
            kcNumber = KC_3;
            break;
        case MY_R:
            kcNumber = KC_4;
            break;
        case MY_T:
            kcNumber = KC_5;
            break;
        case MY_Y:
            kcNumber = KC_6;
            break;
        case MY_U:
            kcNumber = KC_7;
            break;
        case MY_I:
            kcNumber = KC_8;
            break;
        case MY_O:
            kcNumber = KC_9;
            break;
        case MY_P:
            kcNumber = KC_0;
            break;

        default:
            break;
    }
    return kcNumber;
}

uint16_t mapToCharacterOfAlphabet(uint16_t keycode) {
    uint16_t character;

    switch (keycode) {
        case MY_Q:
            character = KC_Q;
            break;
        case MY_W:
            character = KC_W;
            break;
        case MY_E:
            character = KC_E;
            break;
        case MY_R:
            character = KC_R;
            break;
        case MY_T:
            character = KC_T;
            break;
        case MY_Y:
            character = KC_Y;
            break;
        case MY_U:
            character = KC_U;
            break;
        case MY_I:
            character = KC_I;
            break;
        case MY_O:
            character = KC_O;
            break;
        case MY_P:
            character = KC_P;
            break;

        default:
            break;
    }
    return character;
}

void typeString(uint16_t keycode) {
    switch (keycode) {
        case MY_PASSWORD_1:
            SEND_STRING("ckan26298");
            break;

        case MY_PASSWORD_1_UPPERCASE:
            SEND_STRING("cKan26298");
            break;

        case MY_PASSWORD_1_UPPERCASE_SYMBOL:
            SEND_STRING("cKan26298@");
            break;

        case MY_PASSWORD_2:
            SEND_STRING("vinhan26298");
            break;

        case MY_PASSWORD_2_UPPERCASE_SYMBOL:
            SEND_STRING("vInhan");
            SEND_STRING("26298@");
            break;
        default:
            break;
    }
}

bool process_number_row(uint16_t keycode, keyrecord_t *record) {
    uint16_t character = mapToCharacterOfAlphabet(keycode);
    if (record->event.pressed) {
        time_hold_kc = timer_read();
        tap_code(character); // or SEND_STRING(SS_TAP(X_Code));
        return false;
    } else {
        uint16_t kcNumber = mapToNumber(keycode);
        if (timer_elapsed(time_hold_kc) > TAPPING_TERM_LONG) {
            tap_code(KC_BSPC);
            // or: tap_code(KC_EXLM);
            register_code(KC_LSFT);
            tap_code(kcNumber);
            unregister_code(KC_LSFT);
            return false;
        }
        if (timer_elapsed(time_hold_kc) > TAPPING_TERM) {
            tap_code(KC_BSPC);
            tap_code(kcNumber);
            return false;
        }
    }
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint16_t my_hash_timer;

    // Store the current modifier state in the variable for later reference
    mod_state = get_mods();

    switch (keycode) {
        case MY_HASH:
            if (record->event.pressed) {
                my_hash_timer = timer_read();
                register_code(KC_SPC); // Change the key to be held here
            } else {
                unregister_code(KC_SPC); // Change the key that was held here, too!
                if (timer_elapsed(my_hash_timer) < TAPPING_TERM) {
                    SEND_STRING(SS_TAP(X_BSPC) "vinhan26298"); // Change the character(s) to be sent on tap here
                }
            }
            return false;

        case KC_ESC: {
            unregister_code(KC_TAB);
            // Detect the activation of only Left Alt
            if ((get_mods() & MOD_BIT(KC_LALT)) == MOD_BIT(KC_LALT)) {
                if (record->event.pressed) {
                    // No need to register KC_LALT because it's already active.
                    // The Alt modifier will apply on this KC_TAB.
                    register_code(KC_TAB);
                } else {
                    unregister_code(KC_TAB);
                }
                // Do not let QMK process the .keycode further
                return false;
            } else if (mod_state & MOD_MASK_CTRL) {
                if (record->event.pressed) {
                    register_code(KC_TAB);
                } else {
                    unregister_code(KC_TAB);
                }
                return false;
            } else if (mod_state & MOD_MASK_SHIFT) {
                if (record->event.pressed) {
                    register_code(KC_TAB);
                } else {
                    unregister_code(KC_TAB);
                }
                return false;
            } else if ((get_mods() & MOD_BIT(KC_LGUI)) == MOD_BIT(KC_LGUI)) {
                if (record->event.pressed) {
                    register_code(KC_TAB);
                } else {
                    unregister_code(KC_TAB);
                }
                return false;
            }
            return true;
        }

        case KC_BSPC: {
            // Initialize a boolean variable that keeps track
            // of the delete key status: registered or not?
            static bool delkey_registered;
            if (record->event.pressed) {
                // Detect the activation of either shift keys
                if (mod_state & MOD_MASK_ALT) {
                    // First temporarily canceling both shifts so that
                    // shift isn't applied to the KC_DEL keycode
                    del_mods(MOD_MASK_ALT);
                    register_code(KC_DEL);
                    // Update the boolean variable to reflect the status of KC_DEL
                    delkey_registered = true;
                    // Reapplying modifier state so that the held shift key(s)
                    // still work even after having tapped the Backspace/Delete key.
                    set_mods(mod_state);
                    return false;
                }
                if (mod_state & MOD_MASK_SHIFT) {
                    del_mods(MOD_MASK_SHIFT);
                    tap_code(KC_BSPC);
                    // SEND_STRING(SS_LSFT(SS_TAP(X_HOME) SS_TAP(X_BSPC)));
                    set_mods(mod_state);
                    return false;
                }
            } else { // on release of KC_BSPC
                // In case KC_DEL is still being sent even after the release of KC_BSPC
                if (delkey_registered) {
                    unregister_code(KC_DEL);
                    delkey_registered = false;
                    return false;
                }
            }
            // Let QMK process the KC_BSPC keycode as usual outside of shift
            return true;
        }

        case BSPC_FOR_MAC: {
            static bool delkey_registered;
            if (record->event.pressed) {
                if (mod_state & MOD_MASK_CTRL) {
                    del_mods(MOD_MASK_CTRL);
                    register_code(KC_DEL);
                    delkey_registered = true;
                    set_mods(mod_state);
                    return false;
                }
                register_code(KC_BSPC);
                return false;
            } else {
                if (delkey_registered) {
                    unregister_code(KC_DEL);
                    delkey_registered = false;
                    return false;
                }else{
                    unregister_code(KC_BSPC);
                    return false;
                }
            }
            return false;
        }

        case MY_ALT_TAB:
            if (record->event.pressed) {
                if (!is_alt_tab_active) {
                    is_alt_tab_active = true;
                    register_code(KC_LALT);
                }
                alt_tab_timer = timer_read();
                register_code(KC_TAB);
            } else {
                unregister_code(KC_TAB);
            }
            break;

            return true; // We handled this keypress

        case MY_Q:
            return process_number_row(keycode, record);
            break;

        case MY_W:
            return process_number_row(keycode, record);
            break;

        case MY_E:
            return process_number_row(keycode, record);
            break;

        case MY_R:
            return process_number_row(keycode, record);
            break;

        case MY_T:
            return process_number_row(keycode, record);
            break;

        case MY_Y:
            return process_number_row(keycode, record);
            break;

        case MY_U:
            return process_number_row(keycode, record);
            break;

        case MY_I:
            return process_number_row(keycode, record);
            break;

        case MY_O:
            return process_number_row(keycode, record);
            break;

        case MY_P:
            return process_number_row(keycode, record);
            break;

        case CLOSE_ONE_TAB_ANDROID_STUDIO:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                SEND_STRING(SS_LCTL(SS_LALT("1")));
                return false;
            }
            break;
            return true;

        case CLOSE_OTHER_TAB_ANDROID_STUDIO:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                SEND_STRING(SS_LCTL(SS_LALT("2")));
                return false;
            }
            break;
            return true;

        case CLOSE_ALL_TAB_ANDROID_STUDIO:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                SEND_STRING(SS_LCTL(SS_LALT("3")));
                return false;
            }
            break;
            return true;

        case CLOSE_ALL_TAB_LEFT_ANDROID_STUDIO:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                SEND_STRING(SS_LCTL(SS_LALT("4")));
                return false;
            }
            break;
            return true;

        case CLOSE_ALL_TAB_RIGHT_ANDROID_STUDIO:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                SEND_STRING(SS_LCTL(SS_LALT("5")));
                return false;
            }
            break;
            return true;

        case CLIPBOARD_WINDOW:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                SEND_STRING(SS_LGUI("v"));
                return false;
            }
            break;
            return true;

        case LT_WIN_FUNCTION_SPC: {
            if (record->event.pressed) {
                if (mod_state & MOD_MASK_ALT) {
                    del_mods(MOD_MASK_ALT);
                    SEND_STRING(SS_LCTL(" "));
                    set_mods(mod_state);
                    unregister_code(KC_LALT);
                    return false;
                }
            }
            // Let QMK process the KC_BSPC keycode as usual outside of shift
            return true;
        }

        case KC_C: {
            if (record->event.pressed) {
                if (mod_state & MOD_MASK_CTRL) {
                    // SEND_STRING(SS_LCTL("c"));
                    tap_code(KC_C);
                    set_mods(mod_state);
                    return false;
                }
            }
            // Let QMK process the KC_BSPC keycode as usual outside of shift
            return true;
        }

        case MY_PASSWORD_1:
            if (record->event.pressed) {
                typeString(MY_PASSWORD_1);
            }
            return false;

        case MY_PASSWORD_1_UPPERCASE:
            if (record->event.pressed) {
                typeString(MY_PASSWORD_1_UPPERCASE);
            }
            return false;

        case MY_PASSWORD_1_UPPERCASE_SYMBOL:
            if (record->event.pressed) {
                typeString(MY_PASSWORD_1_UPPERCASE_SYMBOL);
            }
            return false;

        case MY_PASSWORD_2:
            if (record->event.pressed) {
                typeString(MY_PASSWORD_2);
            }
            return false;

        case MY_KC_PARENTHESE: {
            if (record->event.pressed) {
                SEND_STRING("()");
                tap_code(KC_LEFT);
            }
            return true;
        }

        case MY_KC_SQUARED_CURLY_BRACKETS: {
            if (record->event.pressed) {
                if (mod_state & MOD_MASK_SHIFT) {
                    del_mods(MOD_MASK_SHIFT);
                    SEND_STRING("{}");
                    tap_code(KC_LEFT);
                    set_mods(mod_state);
                    return false;
                } else {
                    SEND_STRING("[]");
                    tap_code(KC_LEFT);
                    return false;
                }
            }
            return true;
        }

        case MY_KC_APOSTROPHE_QUOTATION_MARKS: {
            if (record->event.pressed) {
                if (mod_state & MOD_MASK_SHIFT) {
                    del_mods(MOD_MASK_SHIFT);
                    SEND_STRING("\"\"");
                    tap_code(KC_LEFT);
                    set_mods(mod_state);
                    return false;
                } else {
                    SEND_STRING("''");
                    tap_code(KC_LEFT);
                    return false;
                }
            }
            return true;
        }
    }
    return true; // We didn't handle other keypresses, Process all other keycodes normally
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_WIN_QWERTY] = LAYOUT(KC_ESC, MY_Q, MY_W, MY_E, MY_R, MY_T, MY_Y, MY_U, MY_I, MY_O, MY_P, KC_BSPC, MT_CTRL_TAB, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_ENT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_LALT, KC_LGUI, KC_LCTL, LT_WIN_FUNCTION_SPC, MO_WIN_ARROW, MO_WIN_NUMBER, KC_APP, MO_WIN_NUM_FUNCTION),

    [_WIN_ARROW] = LAYOUT(KC_ESC, KC_NO, KC_W, KC_E, KC_R, KC_NO, KC_NO, KC_HOME, KC_UP, KC_END, KC_NO, CTRL_BSPC, MT_CTRL_TAB, KC_A, KC_BTN4, KC_D, KC_BTN5, KC_PGDOWN, KC_PGUP, KC_LEFT, KC_DOWN, KC_RGHT, KC_ENT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_NO, KC_DEL, KC_INS, KC_NO, KC_NO, KC_NO, KC_LALT, KC_LGUI, KC_LCTL, MO_WIN_MOUSE1, KC_NO, KC_NO, KC_NO, KC_NO),

    [_WIN_NUMBER] = LAYOUT(KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC, MT_CTRL_TAB, KC_LPRN, KC_RPRN, KC_MINS, KC_EQL, KC_QUOT, KC_COLN, KC_LBRC, KC_RBRC, KC_BSLS, KC_SCLN, KC_LSFT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_COMM, KC_DOT, KC_SLSH, KC_LALT, KC_LGUI, KC_LCTL, KC_SPC, KC_NO, KC_NO, KC_NO, KC_NO),

    [_WIN_FUNCTION] = LAYOUT(KC_ENT, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_CAPS, CLOSE_ONE_TAB_ANDROID_STUDIO, CLOSE_OTHER_TAB_ANDROID_STUDIO, CLOSE_ALL_TAB_ANDROID_STUDIO, CLOSE_ALL_TAB_LEFT_ANDROID_STUDIO, CLOSE_ALL_TAB_RIGHT_ANDROID_STUDIO, MEH(KC_R), LALT(KC_LEFT), KC_NO, LALT(KC_RIGHT), KC_F12, KC_LSFT, MEH(KC_1), MEH(KC_2), MEH(KC_3), MEH(KC_4), MEH(KC_5), MEH(KC_6), MEH(KC_7), MEH(KC_8), MEH(KC_9), LGUI(KC_LSFT), KC_LALT, KC_LGUI, KC_LCTL, KC_NO, MO_WIN_MOUSE2, KC_NO, MO_WIN_PASSWORD, MO_WIN_MEDIA),

    [_WIN_MOUSE1] = LAYOUT(KC_ESC, KC_NO, KC_BTN4, KC_MS_U, KC_BTN5, KC_NO, KC_NO, KC_BTN1, KC_WH_U, KC_BTN2, KC_NO, KC_NO, KC_TAB, KC_NO, KC_MS_L, KC_MS_D, KC_MS_R, KC_NO, KC_BTN3, KC_WH_L, KC_WH_D, KC_WH_R, KC_NO, KC_LSFT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_LALT, KC_LGUI, KC_LCTL, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO),

    [_WIN_MOUSE2] = LAYOUT(KC_ESC, KC_NO, KC_BTN4, KC_MS_U, KC_BTN5, KC_NO, KC_NO, KC_BTN1, KC_WH_U, KC_BTN2, KC_NO, KC_NO, KC_TAB, KC_NO, KC_MS_L, KC_MS_D, KC_MS_R, KC_NO, KC_BTN3, KC_WH_L, KC_WH_D, KC_WH_R, KC_NO, KC_LSFT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_LALT, KC_LGUI, KC_LCTL, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO),

    [_WIN_MEDIA] = LAYOUT(KC_ESC, RGB_VAD, RGB_VAI, RGB_SPD, RGB_SPI, RGB_RMOD, RGB_MOD, RGB_HUD, RGB_HUI, RGB_SAD, RGB_SAI, KC_BSPC, KC_TAB, KC_MPLY, KC_MPRV, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, RGB_MODE_REVERSE, RGB_MODE_FORWARD, RGB_TOG, KC_ENT, RGB_M_T, RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW, RGB_M_SN, RGB_M_K, RGB_M_X, RGB_M_G, RGB_M_TW, KC_NO, KC_LALT, KC_LGUI, KC_LCTL, KC_NO, KC_NO, CHANGE_LAYER_MAC_QWERTY, KC_NO, KC_NO),

    [_WIN_NUM_FUNCTION] = LAYOUT(KC_F12, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, MT_CTRL_TAB, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_LSFT, MY_KC_PARENTHESE, MY_KC_SQUARED_CURLY_BRACKETS, MY_KC_APOSTROPHE_QUOTATION_MARKS, KC_NO, KC_NO, KC_NO, KC_NO, KC_COMM, KC_DOT, LGUI(KC_LSFT), KC_LALT, KC_LGUI, KC_LCTL, KC_SPC, KC_NO, KC_NO, KC_NO, KC_NO),

    [_WIN_PASSWORD] = LAYOUT(KC_ESC, MY_PASSWORD_1, MY_PASSWORD_1_UPPERCASE, MY_PASSWORD_2, MY_PASSWORD_2_UPPERCASE_SYMBOL, MY_T, MY_Y, MY_U, MY_I, MY_O, MY_P, KC_BSPC, MT_CTRL_TAB, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_ENT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_LALT, KC_LGUI, KC_LCTL, KC_NO, KC_NO, KC_NO, KC_APP, KC_NO),

    [_MAC_QWERTY] = LAYOUT(KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, BSPC_FOR_MAC, MT(MOD_LALT, KC_TAB), KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_ENT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_LCTL, KC_LALT, KC_LGUI, LT(_MAC_FUNCTION, KC_SPC), MO(_MAC_ARROW), MO(_MAC_NUMBER), KC_APP, MO_MAC_NUM_FUNCTION),

    [_MAC_ARROW] = LAYOUT(KC_ESC, KC_NO, KC_W, KC_E, KC_R, KC_NO, KC_NO, KC_HOME, KC_UP, KC_END, KC_NO, OPTION_BSPC, MT_GUI_TAB, KC_A, KC_BTN4, KC_D, KC_BTN5, KC_PGDOWN, KC_PGUP, KC_LEFT, KC_DOWN, KC_RGHT, KC_ENT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_NO, KC_DEL, KC_INS, KC_NO, KC_NO, KC_NO, KC_LCTL, KC_LALT, KC_LGUI, MO(_MAC_MOUSE1), KC_NO, KC_NO, KC_NO, KC_NO),

    [_MAC_NUMBER] = LAYOUT(KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, BSPC_FOR_MAC, MT_GUI_TAB, KC_LPRN, KC_RPRN, KC_MINS, KC_EQL, KC_QUOT, KC_COLN, KC_LBRC, KC_RBRC, KC_BSLS, KC_SCLN, KC_LSFT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_COMM, KC_DOT, KC_SLSH, KC_LCTL, KC_LALT, KC_LGUI, KC_SPC, KC_NO, KC_NO, KC_NO, KC_NO),

    [_MAC_FUNCTION] = LAYOUT(KC_ENT, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_CAPS, KC_BRMD, KC_BRMU, KC_KB_POWER, KC_PWR, KC_SLEP, KC_BRIU, KC_BRID, KC_NO, KC_NO, KC_F12, KC_LSFT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, QK_BOOT, KC_LCTL, KC_LALT, KC_LGUI, KC_NO, MO_MAC_MOUSE2, KC_NO, MO_MAC_PASSWORD, MO_MAC_MEDIA),

    [_MAC_MOUSE1] = LAYOUT(KC_ESC, KC_NO, KC_BTN4, KC_MS_U, KC_BTN5, KC_NO, KC_NO, KC_BTN1, KC_WH_U, KC_BTN2, KC_NO, KC_NO, KC_TAB, KC_NO, KC_MS_L, KC_MS_D, KC_MS_R, KC_NO, KC_BTN3, KC_WH_L, KC_WH_D, KC_WH_R, KC_NO, KC_LSFT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_LCTL, KC_LALT, KC_LGUI, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO),

    [_MAC_MOUSE2] = LAYOUT(KC_ESC, KC_NO, KC_BTN4, KC_MS_U, KC_BTN5, KC_NO, KC_NO, KC_BTN1, KC_WH_U, KC_BTN2, KC_NO, KC_NO, KC_TAB, KC_NO, KC_MS_L, KC_MS_D, KC_MS_R, KC_NO, KC_BTN3, KC_WH_L, KC_WH_D, KC_WH_R, KC_NO, KC_LSFT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_LCTL, KC_LALT, KC_LGUI, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO),

    [_MAC_MEDIA] = LAYOUT(KC_ESC, RGB_VAD, RGB_VAI, RGB_SPD, RGB_SPI, RGB_RMOD, RGB_MOD, RGB_HUD, RGB_HUI, RGB_SAD, RGB_SAI, KC_BSPC, KC_TAB, KC_MPLY, KC_MPRV, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, RGB_MODE_REVERSE, RGB_MODE_FORWARD, RGB_TOG, KC_ENT, RGB_M_T, RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW, RGB_M_SN, RGB_M_K, RGB_M_X, RGB_M_G, RGB_M_TW, KC_NO, KC_LALT, KC_LGUI, KC_LCTL, KC_NO, KC_NO, CHANGE_LAYER_WIN_QWERTY, KC_NO, KC_NO),

    [_MAC_NUM_FUNCTION] = LAYOUT(KC_F12, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, MT_GUI_TAB, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_LSFT, MY_KC_PARENTHESE, MY_KC_SQUARED_CURLY_BRACKETS, MY_KC_APOSTROPHE_QUOTATION_MARKS, KC_NO, KC_NO, KC_NO, KC_NO, KC_COMM, KC_DOT, LGUI(KC_LSFT), KC_LCTL, KC_LALT, KC_LGUI, KC_SPC, KC_NO, KC_NO, KC_NO, KC_NO),

    [_MAC_PASSWORD] = LAYOUT(KC_ESC, MY_PASSWORD_1, MY_PASSWORD_1_UPPERCASE, MY_PASSWORD_2, MY_PASSWORD_2_UPPERCASE_SYMBOL, MY_T, MY_Y, MY_U, MY_I, MY_O, MY_P, KC_BSPC, MT_GUI_TAB, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_ENT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_LCTL, KC_LALT, KC_LGUI, KC_NO, KC_NO, KC_NO, KC_APP, KC_NO)};
    