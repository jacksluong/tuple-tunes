#include "Arduino.h"
#include "Joystick.h"
#include <TFT_eSPI.h>

Joystick::Joystick(int lr, int ud) {
  x_pin = lr;
  y_pin = ud;
  last_reading = 0;
  joystick_change_time = millis();
}

int Joystick::read() {
  // returns 0, 1, 2, 3, 4 for idle, up, right, down, left (cardinal directions) respectively
  x_reading = analogRead(x_pin);
  y_reading = analogRead(y_pin);
  int reading;
  
  if (y_reading > 1000 - THRESHOLD) reading = 1; // range goes up to 1023 but more fluctation at big vals
  else if (y_reading < THRESHOLD) reading = 3; 
  else if (x_reading > 1000 - THRESHOLD) reading = 2;
  else if (x_reading < THRESHOLD) reading = 4;
  else reading = 0;

  if (reading && millis() - joystick_change_time > COOLDOWN) {
    joystick_change_time = millis();
    last_reading = reading;
    return reading;
  } else return 0;
}
