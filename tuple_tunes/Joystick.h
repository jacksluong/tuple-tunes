#ifndef Joystick_h
#define Joystick_h
#include "Arduino.h"
#include <TFT_eSPI.h>

/*
 * Joystick class that can recognize directional input.
 */
class Joystick {
  static const uint8_t THRESHOLD = 45;
  static const uint8_t COOLDOWN = 750;
  
  uint8_t x_pin;
  uint8_t y_pin;
  uint32_t joystick_change_time;
  uint16_t x_reading; // declare variables in class so we don't have to reallocate
  uint16_t y_reading; // memory for two uint16's every time the loop is run
  uint8_t last_reading;
    
  public:
    Joystick(int lr, int ud);
    int read();
};
#endif
