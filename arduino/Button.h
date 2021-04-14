#ifndef Button_h
#define Button_h
#include "Arduino.h"
#include <TFT_eSPI.h>

/*
 * Button class that can recognize short presses and long presses.
 */
class Button {
  uint32_t state_2_start_time;
  uint32_t button_change_time;
  uint32_t debounce_duration;
  uint32_t long_press_duration;
  uint8_t pin;
  uint8_t flag;
  bool button_pressed;
  uint8_t state; // this is public for the sake of convenience
    
  public:
    Button(int p);
    void read();
    int update();
};
#endif
