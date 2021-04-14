#include <Arduino.h>
#include "TextInput.h"

TextInput::TextInput(char* charset, char* output) {
  this -> charset = charset;
  this -> output = output;
  char_index = 0;
  scrolling_timer = millis();
}

/*
 * Updates a string using tilt-based text input. 
 * Returns the selected character at cursor, or a null character if appending.
 */
char TextInput::update(float angle, bool append) {
  // angle: -imu.readAccelData(imu.accelCount)[1] * imu.aRes
  if (append) {
    int len_current_string = strlen(output);
    output[len_current_string] = charset[char_index];
    output[len_current_string + 1] = '\0';
    char_index = 0;
    return '\0';
  } else {
    if (millis() - scrolling_timer > scrolling_threshold) {
      int len_charset = strlen(charset);
      if (angle > angle_threshold) {
        char_index = (char_index + 1) % len_charset;
        scrolling_threshold = 350 - 1000 * ((angle > 0.6 ? 0.6 : angle) - 0.3);
      } else if (-angle > angle_threshold) {
        char_index = (char_index + len_charset - 1) % len_charset;
        scrolling_threshold = 350 - 1000 * ((-angle > 0.6 ? 0.6 : -angle) - 0.3);
      } else {
        scrolling_threshold = 150;
      }
      scrolling_timer = millis();
      return charset[char_index];
    }
  }
}

void TextInput::set_output(char* new_output) {
  output = new_output;
}

void TextInput::set_charset(char* new_charset) {
  charset = new_charset;
  char_index = 0;
}
