#ifndef TextInput_h
#define TextInput_h
#include <Arduino.h>

/*
 * Tilt-based text input of multiple characters and customizable character set/output string.
 */
class TextInput {
  char* charset; // set of characters to scroll over
  char* output; // some string variable outside of this class to write to
  int char_index;
  uint32_t scrolling_timer;
  int scrolling_threshold = 150; // idle wait-time, but actually dependent on tilt magnitude
  const float angle_threshold = 0.3;
  
  public:
    TextInput(char* output) : TextInput(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", output) {}
    TextInput(char* charset, char* output);
    char update(float angle, bool append);
    void set_output(char* new_output);
    void set_charset(char* new_charset);
};

#endif
