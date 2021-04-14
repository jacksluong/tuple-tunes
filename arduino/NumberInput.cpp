#include <Arduino.h>
#include "NumberInput.h"

NumberInput::NumberInput(int minimum, int maximum) {
  this -> minimum = minimum;
  this -> maximum = maximum;
  current_value = minimum;
  scrolling_timer = millis();
}

/*
 * Returns the currently selected number, controllable using tilt.
 */
int NumberInput::update(float angle) {
  // angle: -imu.readAccelData(imu.accelCount)[1] * imu.aRes
  if (millis() - scrolling_timer > scrolling_threshold) {
    int range = maximum - minimum + 1;
    if (angle > angle_threshold) {
      current_value = (current_value - minimum + 1) % range + minimum;
      scrolling_threshold = 350 - 1000 * ((angle > 0.6 ? 0.6 : angle) - 0.3);
    } else if (-angle > angle_threshold) {
      current_value = (current_value - minimum + range - 1) % range + minimum;
      scrolling_threshold = 350 - 1000 * ((-angle > 0.6 ? 0.6 : -angle) - 0.3);
    } else {
      scrolling_threshold = 150;
    }
    scrolling_timer = millis();
    return current_value;
  } else {
    return INT_MIN;
  }
}

/*
 * Functions to reset the range of numbers to scroll through.
 */
void NumberInput::reset(int minimum, int maximum) {
  this -> minimum = minimum;
  this -> maximum = maximum;
  current_value = minimum > 0 ? minimum : 0;
}
void NumberInput::reset(int maximum) {
  this -> maximum = maximum;
  current_value = minimum > 0 ? minimum : 0;
}
void NumberInput::reset() {
  current_value = minimum > 0 ? minimum : 0;
}

/*
 * Sets the cursor at a certain number.
 */
void NumberInput::set_cursor(int value) {
  current_value = value;
}
