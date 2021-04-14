#ifndef NumberInput_h
#define NumberInput_h
#include <Arduino.h>

/*
 * Tilt-based number input that returns a single selected value within a maximum expressible in int8_t.
 */
class NumberInput {
  int minimum;
  int maximum;
  int current_value;
  uint32_t scrolling_timer;
  int scrolling_threshold = 150; // idle wait-time, but actually dependent on tilt magnitude
  const float angle_threshold = 0.3;
  
  public:
    NumberInput() : NumberInput(0, 1000) {}
    NumberInput(int maximum) : NumberInput(0, maximum) {}
    NumberInput(int minimum, int maximum);
    int update(float angle);
    void reset(int minimum, int maximum);
    void reset(int maximum);
    void reset();
    void set_cursor(int value);
};

#endif
