#include QMK_KEYBOARD_H
#include <stdbool.h>

bool     is_alt_tab_active = false;
uint16_t alt_tab_timer     = 0;

uint16_t w_2_timer    = 0;
uint16_t q_1_timer    = 0;
uint16_t time_hold_kc = 0;

enum custom_keycodes { MY_HASH = SAFE_RANGE, ALT_TAB, MY_Q, MY_W, MY_E, MY_R, MY_T, MY_Y, MY_U, MY_I, MY_O, MY_P };

enum { TD_Q_TAB = 0, TD_RESET };

void safe_reset(qk_tap_dance_state_t *state, void *user_data) {
    if (state->count >= 3) {
        // Reset the keyboard if you tap the key more than three times
        reset_keyboard();
        reset_tap_dance(state);
    }
}

qk_tap_dance_action_t tap_dance_actions[] = {[TD_Q_TAB] = ACTION_TAP_DANCE_DOUBLE(KC_Q, KC_TAB), [TD_RESET] = ACTION_TAP_DANCE_FN(safe_reset)};

#define _LAYER0 0
#define _LAYER1 1
#define _LAYER2 2
#define _LAYER3 3
#define _LAYER4 4
#define _LAYER5 5
#define _LAYER6 6
#define _LAYER7 7
#define _LAYER8 8
#define _LAYER9 9

uint8_t mod_state;

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

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint16_t my_hash_timer;

    // Store the current modifier state in the variable for later reference
    mod_state = get_mods();

    switch (keycode) {
        case MY_HASH:
            if (record->event.pressed) {
                my_hash_timer = timer_read();
                register_code(KC_SPC);  // Change the key to be held here
            } else {
                unregister_code(KC_SPC);  // Change the key that was held here, too!
                if (timer_elapsed(my_hash_timer) < TAPPING_TERM) {
                    SEND_STRING(SS_TAP(X_BSPC) "vinhan26298");  // Change the character(s) to be sent on tap here
                }
            }
            return false;

        case KC_GESC: {
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
            } else if ((get_mods() & MOD_BIT(KC_LCTL)) == MOD_BIT(KC_LCTL)) {
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
            } else {  // on release of KC_BSPC
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

        case ALT_TAB:
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

            return true;  // We handled this keypress

        case MY_Q:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                SEND_STRING(SS_TAP(X_Q));
                return false;
            } else {
                uint16_t kcNumber = mapToNumber(keycode);
                if (timer_elapsed(time_hold_kc) > TAPPING_TERM_LONG) {
                    tap_code(KC_BSPC);
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
            break;
            return true;

        case MY_W:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                SEND_STRING(SS_TAP(X_W));
                return false;
            } else {
                uint16_t kcNumber = mapToNumber(keycode);
                if (timer_elapsed(time_hold_kc) > TAPPING_TERM_LONG) {
                    tap_code(KC_BSPC);
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
            break;
            return true;

        case MY_E:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                SEND_STRING(SS_TAP(X_E));
                return false;
            } else {
                uint16_t kcNumber = mapToNumber(keycode);
                if (timer_elapsed(time_hold_kc) > TAPPING_TERM_LONG) {
                    tap_code(KC_BSPC);
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
            break;
            return true;

        case MY_R:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                tap_code(KC_R);
                return false;
            } else {
                uint16_t kcNumber = mapToNumber(keycode);
                if (timer_elapsed(time_hold_kc) > TAPPING_TERM_LONG) {
                    tap_code(KC_BSPC);
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
            break;
            return true;

        case MY_T:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                tap_code(KC_T);
                return false;
            } else {
                uint16_t kcNumber = mapToNumber(keycode);
                if (timer_elapsed(time_hold_kc) > TAPPING_TERM_LONG) {
                    tap_code(KC_BSPC);
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
            break;
            return true;

        case MY_Y:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                tap_code(KC_Y);
                return false;
            } else {
                uint16_t kcNumber = mapToNumber(keycode);
                if (timer_elapsed(time_hold_kc) > TAPPING_TERM_LONG) {
                    tap_code(KC_BSPC);
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
            break;
            return true;

        case MY_U:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                tap_code(KC_U);
                return false;
            } else {
                uint16_t kcNumber = mapToNumber(keycode);
                if (timer_elapsed(time_hold_kc) > TAPPING_TERM_LONG) {
                    tap_code(KC_BSPC);
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
            break;
            return true;

        case MY_I:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                tap_code(KC_I);
                return false;
            } else {
                uint16_t kcNumber = mapToNumber(keycode);
                if (timer_elapsed(time_hold_kc) > TAPPING_TERM_LONG) {
                    tap_code(KC_BSPC);
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
            break;
            return true;

        case MY_O:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                tap_code(KC_O);
                return false;
            } else {
                uint16_t kcNumber = mapToNumber(keycode);
                if (timer_elapsed(time_hold_kc) > TAPPING_TERM_LONG) {
                    tap_code(KC_BSPC);
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
            break;
            return true;

        case MY_P:
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                tap_code(KC_P);
                return false;
            } else {
                uint16_t kcNumber = mapToNumber(keycode);
                if (timer_elapsed(time_hold_kc) > TAPPING_TERM_LONG) {
                    tap_code(KC_BSPC);
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
            break;
            return true;

        case KC_ENT: {
            // static bool ctrl_sft_ent_registered;
            if (record->event.pressed) {
                time_hold_kc = timer_read();
                // if (((mod_state & MOD_BIT(KC_LSFT)) == MOD_BIT(KC_LSFT)) && ((mod_state & MOD_BIT(KC_LCTL)) == MOD_BIT(KC_LCTL))) {
                //     del_mods(MOD_MASK_SHIFT);
                //     del_mods(MOD_MASK_CTRL);
                //     tap_code(KC_END);
                //     tap_code(KC_SCLN);

                //     ctrl_sft_ent_registered = true;

                //     set_mods(mod_state);
                //     return false;
                // }

                return false;
            } else {
                // if (ctrl_sft_ent_registered) {
                //     ctrl_sft_ent_registered = false;
                //     return false;
                // }
                if (timer_elapsed(time_hold_kc) < TAPPING_TERM) {
                    register_code(KC_ENT);
                    unregister_code(KC_ENT);
                }
                if (timer_elapsed(time_hold_kc) > TAPPING_TERM_LONG) {
                    register_code(KC_LSFT);
                    tap_code(KC_SCLN);
                    unregister_code(KC_LSFT);
                    return false;
                }
                if (timer_elapsed(time_hold_kc) > TAPPING_TERM) {
                    tap_code(KC_SCLN);
                    return false;
                }
            }
            break;
            return true;
        }
    }
    return true;  // We didn't handle other keypresses
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_LAYER0] = LAYOUT_60_b_ansi(KC_LEAD, TD(TD_RESET), TD(TD_Q_TAB), LCTL(LALT(KC_DEL)), MT(MOD_LCTL | MOD_LSFT, KC_D), ALT_TAB, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_GESC, MY_Q, MY_W, MY_E, MY_R, MY_T, MY_Y, MY_U, MY_I, MY_O, MY_P, KC_BSPC, _______, _______, MT(MOD_LCTL, KC_TAB), KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_ENT, LT(_LAYER6, KC_ENT), KC_SFTENT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, _______, _______, KC_LCTL, KC_LGUI, KC_LALT, LT(_LAYER3, KC_SPC), MY_HASH, MO(_LAYER1), MO(_LAYER2), KC_APP, KC_LEAD, KC_B, KC_MENU),

    [_LAYER1] = LAYOUT_60_b_ansi(_______, _______, _______, _______, MT(MOD_LCTL | MOD_LSFT, KC_D), _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_ESC, _______, KC_W, KC_E, KC_R, _______, _______, KC_HOME, KC_UP, KC_END, _______, LCTL(KC_BSPC), _______, LCTL(KC_BSPC), MT(MOD_LCTL, KC_TAB), KC_A, KC_BTN4, KC_D, KC_BTN5, KC_PGDOWN, KC_PGUP, KC_LEFT, KC_DOWN, KC_RGHT, KC_ENT, _______, _______, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, _______, KC_DEL, KC_INS, _______, _______, _______, _______, _______, KC_LCTL, KC_LGUI, KC_LALT, MO(_LAYER4), _______, _______, _______, _______, _______, _______, _______),

    [_LAYER2] = LAYOUT_60_b_ansi(_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, LCTL(KC_BSPC), KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC, _______, _______, MT(MOD_LCTL, KC_TAB), KC_LPRN, KC_RPRN, KC_MINS, KC_EQL, KC_QUOT, KC_COLN, KC_LBRC, KC_RBRC, KC_BSLS, KC_SCLN, _______, _______, KC_LSFT, _______, _______, _______, _______, _______, _______, _______, KC_COMM, KC_DOT, KC_SLSH, _______, _______, KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, MO(_LAYER6), _______, _______, _______, _______, _______, _______),

    [_LAYER3] = LAYOUT_60_b_ansi(_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_ESC, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, _______, _______, KC_CAPS, KC_F12, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_LSFT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_LCTL, KC_LGUI, KC_LALT, _______, _______, MO(_LAYER5), _______, _______, _______, _______, _______),

    [_LAYER4] = LAYOUT_60_b_ansi(_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_ESC, _______, KC_BTN4, KC_MS_U, KC_BTN5, _______, _______, KC_BTN1, KC_WH_U, KC_BTN2, _______, _______, _______, _______, KC_TAB, _______, KC_MS_L, KC_MS_D, KC_MS_R, _______, KC_BTN3, KC_WH_L, KC_WH_D, KC_WH_R, _______, _______, _______, KC_LSFT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_LCTL, KC_LGUI, KC_LALT, _______, _______, _______, _______, _______, _______, _______, _______),

    [_LAYER5] = LAYOUT_60_b_ansi(_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_ESC, _______, KC_BTN4, KC_MS_U, KC_BTN5, _______, _______, KC_BTN1, KC_WH_U, KC_BTN2, _______, _______, _______, _______, KC_TAB, _______, KC_MS_L, KC_MS_D, KC_MS_R, _______, KC_BTN3, KC_WH_L, KC_WH_D, KC_WH_R, _______, _______, _______, KC_LSFT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_LCTL, KC_LGUI, KC_LALT, _______, _______, _______, _______, _______, _______, _______, _______),

    [_LAYER6] = LAYOUT_60_b_ansi(_______, TG(_LAYER7), _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_ESC, RGB_VAD, RGB_VAI, RGB_SPD, RGB_SPI, RGB_RMOD, RGB_MOD, RGB_HUD, RGB_HUI, KC_MINS, KC_EQL, KC_BSPC, _______, _______, KC_TAB, KC_MPLY, KC_MPRV, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, _______, _______, _______, KC_ENT, _______, _______, KC_LSFT, RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW, RGB_M_SN, RGB_M_K, RGB_M_X, RGB_M_G, RGB_M_T, RGB_TOG, _______, _______, KC_LCTL, KC_LGUI, KC_LALT, _______, _______, _______, _______, _______, RESET, _______, _______),

    [_LAYER7] = LAYOUT_60_b_ansi(KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_BSPC, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_UP, KC_DEL, KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, MO(_LAYER9), MO(_LAYER8), MO(_LAYER2), KC_APP, KC_LEFT, KC_DOWN, KC_RGHT),

    [_LAYER8] = LAYOUT_60_b_ansi(KC_ESC, TG(_LAYER7), _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, LCTL(KC_BSPC), KC_TAB, _______, KC_PGDN, KC_WH_U, KC_PGUP, _______, _______, KC_HOME, KC_UP, KC_END, _______, KC_BSPC, KC_BSPC, _______, _______, _______, KC_BTN4, KC_WH_D, KC_BTN5, _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______, KC_ENT, _______, _______, KC_BTN4, KC_WH_D, KC_BTN5, _______, KC_NO, KC_LEFT, KC_DOWN, KC_RGHT, KC_NO, KC_NO, KC_ENT, KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, _______, _______, _______, _______, _______, _______, _______),

    [_LAYER9] = LAYOUT_60_b_ansi(KC_GRAVE, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, RGB_TOG, RGB_TOG, KC_TAB, RGB_VAD, RGB_VAI, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_CAPS, KC_MPLY, KC_MPRV, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, _______, _______, _______, _______, _______, _______, KC_LSFT, RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW, RGB_M_SN, RGB_M_K, RGB_M_X, RGB_M_G, RGB_M_T, _______, _______, _______, KC_LCTL, KC_LGUI, KC_LALT, _______, _______, _______, _______, _______, _______, _______, _______)};
