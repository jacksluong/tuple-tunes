//#include <ESP32AnalogRead.h>

//#include <analogWrite.h>

#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include <WiFiClient.h>
#include <TFT_eSPI.h>
//#include <driver/dac.h>
//#include <driver/dac_common.h>

//dac_cw_config_t DAC_BUZZER = {DAC_CHANNEL_1, 0x0, 0x2, 261.63, 0}
double freq = 440;
double time_period;

void setup() {
  //dac_output_enable(DAC_CHANNEL_1);
  //dac_cw_generator_enable();
  //dac_cw_generator_config(&DAC_BUZZER);
  Serial.begin(115200);
  pinMode(25, OUTPUT);
  time_period = 1000.0/freq;
}

void loop() {
  
}
