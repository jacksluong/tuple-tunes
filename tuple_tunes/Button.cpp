#include "Arduino.h"
#include "Button.h"
#include <TFT_eSPI.h>

Button::Button(int p) {
  flag = 0;
  state = 0;
  pin = p;
  state_2_start_time = millis();
  button_change_time = millis();
  button_pressed = 0;
}

int Button::read() {
  uint8_t button_state = digitalRead(pin);
  button_pressed = !button_state;

  return update();
}

int Button::update() {
  // returns 0, 1, 2 for off, short press, long press, respectively
  flag = 0;
  switch (state) {
    case 0:
      if (button_pressed) {
        state = 1;
        button_change_time = millis();
      }
      break;
    case 1:
      if (!button_pressed) {
        state = 0;
        button_change_time = millis();
      } else if (millis() - button_change_time >= DEBOUNCE_DURATION) {
        state = 2;
        state_2_start_time = millis();
      }
      break;
    case 2:
      if (!button_pressed) {
        state = 4;
        button_change_time = millis();
      } else if (millis() - state_2_start_time >= LONG_PRESS_DURATION) {
        state = 3;
      }
      break;
    case 3:
      if (!button_pressed) {
        state = 4;
        button_change_time = millis();
      }
      break;
    case 4:
      if (!button_pressed && millis() - button_change_time >= DEBOUNCE_DURATION) {
        state = 0;
        flag = millis() - state_2_start_time < LONG_PRESS_DURATION ? 1 : 2;
      } else if (button_pressed) {
        button_change_time = millis();
        if (millis() - state_2_start_time < LONG_PRESS_DURATION) {
          state = 2;
        } else {
          state = 3;
        }
      }
      break;
  }
  return flag;
}
