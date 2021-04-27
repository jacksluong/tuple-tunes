#ifndef Button_h
#define Button_h
#include "Arduino.h"
#include <TFT_eSPI.h>

/*
 * Button class that can recognize short presses and long presses.
 */
class Button {
  static const uint32_t DEBOUNCE_DURATION = 10;
  static const uint32_t LONG_PRESS_DURATION = 1000;
  
  uint8_t pin;
  uint32_t state_2_start_time;
  uint32_t button_change_time;
  uint8_t flag;
  bool button_pressed;
  uint8_t state;
    
  int update();
  
  public:
    Button(int p);
    int read();
};
#endif
