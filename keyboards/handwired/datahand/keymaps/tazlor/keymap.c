/* Copyright 2017-2019 Nikolaus Wittenstein <nikolaus.wittenstein@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include QMK_KEYBOARD_H

#include <stdbool.h>

/* Datahand features not supported:
 * * All online reprogramming (user settings using the reset button).
 *   * Program Selection features.
 *   * Macros.
 *   * Direct substitutions.
 * * L/R Modf.
 * * Mouse Click Lock (Function Direct Access + Mouse Button key).
 * * Different mouse movement speeds with the two pointer fingers, and using both pointer fingers to move even faster.
 *
 * As far as I know, everything else works.
 */

enum layer {
  NORMAL,
  GAME,
  FUNCTION_MOUSE,
  NAS,
  NAS_NUMLOCK,

  NUM_LAYERS
};

enum custom_keycodes {
  N = SAFE_RANGE,   /* Normal */
  NS,               /* NAS */
  NSL,              /* NAS Lock */
  NLK,              /* Numlock */
  FN,               /* Function mode - needs to be able to switch to mouse or arrow layer */
  TK0,              /* Ten-key off button */
  TK1,              /* Ten-key on button */
};

static bool numlock_enabled = false;
static bool nas_locked = false;

/* Declared weak so that it can easily be overridden. */
__attribute__((weak)) const uint16_t PROGMEM keymaps[NUM_LAYERS][MATRIX_ROWS][MATRIX_COLS] = {
// QWERTY
// [NORMAL] = LAYOUT(
//         KC_Q,                     KC_W,                 KC_E,                 KC_R,                       KC_U,                   KC_I,                     KC_O,                    KC_P,
// KC_DEL, KC_A, KC_LBRC,    KC_ESC, KC_S, KC_B,   KC_GRV, KC_D, KC_T,   KC_DQT, KC_F, KC_G,           KC_H, KC_J, KC_QUOT,   KC_Y,  KC_K,   KC_COLN,   KC_N,  KC_L, KC_ENT,   KC_RBRC, KC_SCLN, KC_BSLS,
//         KC_Z,                     KC_X,                 KC_C,                 KC_V,                       KC_M,                   KC_COMM,                  KC_DOT,                  KC_SLSH,
//                                                                           KC_ENT,    KC_TAB,   KC_BSPC,  KC_SPC,
//                                                                               KC_LSFT,              NS,
//                                                                               KC_CAPS,              NSL,
//                                                                           N,         KC_LCTL,  KC_LALT,  FN),
// DVORAK
[NORMAL] = LAYOUT(
        KC_QUOT,                KC_COMM,              KC_DOT,              KC_P,                       KC_G,                KC_C,                  KC_R,                    KC_L,
KC_ESC, KC_A, KC_LBRC,  KC_GRV, KC_O, KC_X,  KC_SLSH, KC_E, KC_Y,  KC_DEL, KC_U, KC_I,           KC_D, KC_H, KC_ENT,  KC_F, KC_T, KC_MINUS,  KC_B, KC_N, KC_PLUS,  KC_RBRC, KC_S, KC_BSLS,
        KC_SCLN,                KC_Q,                 KC_J,                KC_K,                       KC_M,                KC_W,                  KC_V,                    KC_Z,
                                                                          KC_BSPACE, KC_TAB,   KC_RCTL,  KC_SPC,
                                                                              KC_LSHIFT,            NS,
                                                                              KC_ENT,               NSL,
                                                                          N,         KC_LCMD,  KC_RALT,  FN),

[GAME] = LAYOUT(
        KC_TAB,                 KC_Q,              KC_W,              KC_E,                     _______,                    KC_UP,                       _______,                    _______,
KC_ESC, KC_LSHIFT, KC_1,  KC_2, KC_A, KC_3,  KC_4, KC_S, KC_R,  KC_V, KC_D, KC_F,      _______, KC_LEFT, _______,  _______, KC_DOWN, _______,  KC_RIGHT, _______, _______,  _______, _______, _______,
        KC_LCTRL,               KC_Z,              KC_X,              KC_C,                     _______,                    _______,                     _______,                    _______,
                                                                      _______, _______,   _______,  _______,
                                                                          KC_SPACE,            _______,
                                                                          XXXXXXX,             _______,
                                                                      _______,   XXXXXXX,  _______,  XXXXXXX),

[FUNCTION_MOUSE] = LAYOUT(
       KC_VOLU,                 KC_HOME,                KC_PGUP,                   KC_WH_U,                      KC_MS_U,                  KC_CAPS,                KC_SLCK,                KC_INSERT,
KC_F1, KC_MUTE, KC_F2,   KC_F3, KC_BTN3, KC_F4,  KC_F5, KC_BTN2, KC_F6,   _______, KC_BTN1, _______,    KC_MS_L, KC_LEFT, KC_MS_R,  KC_F7, KC_DOWN, KC_F8,  KC_F9, KC_UP, KC_F10,  KC_F11, KC_RIGHT, KC_F12,
       KC_VOLD,                 KC_END,                 KC_PGDN,                   KC_WH_D,                      KC_MS_D,                  KC_SLEP,                KC_NLCK,                KC_PSCREEN,
                                                                                             _______,   _______,   _______,   _______,
                                                                                                  _______,              _______,
                                                                                                  _______,              _______,
                                                                                             _______,   _______,   _______,   _______),
[NAS] = LAYOUT(
         KC_EXLM,                 KC_AT,                   KC_HASH,                 KC_DLR,                    KC_AMPR,                KC_ASTR,                 KC_LPRN,                 KC_RPRN,
_______, KC_1, _______,  _______, KC_2, KC_LCBR,  KC_LABK, KC_3, KC_RABK,  _______, KC_4, KC_5,          KC_6, KC_7, _______,  KC_EQL, KC_8, _______,  KC_RCBR, KC_9, KC_BSLS,  _______, KC_0, _______,
         _______,                 KC_TILDE,                KC_DOT,                  KC_PERC,                   KC_CIRC,                KC_SLSH,                 KC_QUES,                 TG(GAME),
                                                                                             _______,   _______,   _______,   _______,
                                                                                                  _______,              _______,
                                                                                                  _______,              _______,
                                                                                             _______,   _______,   _______,   _______),
[NAS_NUMLOCK] = LAYOUT(
         _______,                     _______,                     _______,                     _______,                            _______,                     KC_PAST,                     _______,                     _______,
_______, KC_KP_1, _______,   _______, KC_KP_2, _______,   _______, KC_KP_3, _______,   _______, KC_KP_4, KC_KP_5,          KC_KP_6, KC_KP_7, KC_PENT,   KC_PEQL, KC_KP_8, KC_PMNS,   _______, KC_KP_9, KC_PPLS,   _______, KC_KP_0, _______,
         KC_PEQL,                     _______,                     KC_PDOT,                     _______,                            _______,                     KC_PSLS,                     _______,                     _______,
                                                                                             _______,   _______,   _______,   _______,
                                                                                                  _______,              _______,
                                                                                                  _______,              _______,
                                                                                             _______,   _______,   _______,   _______),
};

static void lock_led_set(bool on, uint8_t led) {
  if (on) {
    LED_LOCK_PORT &= ~led;
  } else {
    LED_LOCK_PORT |= led;
  }
}

static void mode_led_set(uint8_t led) {
  static const uint8_t ALL_MODE_LEDS = LED_FN | LED_NORMAL | LED_NAS | LED_TENKEY;
  LED_MODE_PORT |= ALL_MODE_LEDS;
  LED_MODE_PORT &= ~led;
}

// Runs whenever there is a layer state change.
uint32_t layer_state_set_user(uint32_t state) {
  switch (biton32(state)) { // highest active layer
    case NORMAL:
      mode_led_set(LED_NORMAL);
      break;
    case GAME:
      mode_led_set(LED_TENKEY);
      break;
    case FUNCTION_MOUSE:
      mode_led_set(LED_FN);
      break;
    case NAS:
    case NAS_NUMLOCK:
      mode_led_set(LED_NAS);
      break;
  }
  return state;
}

static void layer_set(bool on, uint8_t layer) {
  if (on) {
    layer_on(layer);
  } else {
    layer_off(layer);
  }
}

static void set_nas(bool on) {
  /* Turn off nas_lock if nas is turned off. */
  if (!on) {
    nas_locked = false;
  }
  /* Always turn on the base NAS layer so other layers can fall through. */
  layer_set(on, NAS);

  layer_set(on && numlock_enabled, NAS_NUMLOCK);
}

static void set_normal(void) {
  layer_off(FUNCTION_MOUSE);
  set_nas(false);
}

static void toggle_numlock(void) {
  numlock_enabled = !numlock_enabled;
  lock_led_set(numlock_enabled, LED_NUM_LOCK);

  if (layer_state_is(NAS)) {
    /* If we're already in NAS, re-set it so that we activate the numlock layer. */
    set_nas(true);
  }
}

static void set_function(void) {
  /* Make sure to turn off NAS if we're entering function */
  set_nas(false);

  layer_on(FUNCTION_MOUSE);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  bool pressed = record->event.pressed;

  switch(keycode) {
    case N:
    if (pressed) {
      set_normal();
    }
    break;

    case NS:
    if (!nas_locked) {
      set_nas(pressed);
    }
    break;

    case NSL:
    if (pressed) {
      nas_locked = !nas_locked;
      set_nas(true);
    }
    break;

    case NLK:
    if (pressed) {
      toggle_numlock();
      SEND_STRING(SS_DOWN(X_NUMLOCK));
    } else {
      SEND_STRING(SS_UP(X_NUMLOCK));
    }
    break;

    case FN:
    if (pressed) {
      set_function();
    }
    break;

  }

  return true;
};

void matrix_init_user(void) {
}

void matrix_scan_user(void) {

}

void led_set_user(uint8_t usb_led) {
  lock_led_set(usb_led & (1<<USB_LED_NUM_LOCK), LED_NUM_LOCK);
  lock_led_set(usb_led & (1<<USB_LED_CAPS_LOCK), LED_CAPS_LOCK);
  lock_led_set(usb_led & (1<<USB_LED_SCROLL_LOCK), LED_SCROLL_LOCK);
}
