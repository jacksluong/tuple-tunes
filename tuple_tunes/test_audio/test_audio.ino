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
unsigned long last_played;
int SineValues[256];       // an array to store our values for sine

void setup() {
  //dac_output_enable(DAC_CHANNEL_1);
  //dac_cw_generator_enable();
  //dac_cw_generator_config(&DAC_BUZZER);
  Serial.begin(115200);
  pinMode(25, OUTPUT);
  time_period = 1000.0/freq;
  last_played = millis();
  float ConversionFactor=(2*PI)/256;        // convert my 0-255 bits in a circle to radians
                                            // there are 2 x PI radians in a circle hence the 2*PI
                                            // Then divide by 256 to get the value in radians
                                            // for one of my 0-255 bits.
  float RadAngle;                           // Angle in Radians
  // calculate sine values
  for(int MyAngle=0;MyAngle<256;MyAngle++) {
    RadAngle=MyAngle*ConversionFactor;               // 8 bit angle converted to radians
    SineValues[MyAngle]=(sin(RadAngle)*127)+128;  // get the sine of this angle and 'shift' up so
                                            // there are no negative values in the data
                                            // as the DAC does not understand them and would
                                            // convert to positive values.
  }
}


 
void loop()
{
  while (millis() - last_played < 2000)
    for(int i=0;i<256;i++)
      dacWrite(25,SineValues[i]/2);

  while (millis() - last_played > 2000 && millis() - last_played < 3000)
    for(int i=0;i<200;i++)
      dacWrite(25,SineValues[i]/2);
  
  while (millis() - last_played > 3000)
    for(int i=0;i<160;i++)
      dacWrite(25,SineValues[i]/2);
}
//void loop() {
//  for(int i=0;i<256;i++)
//    dacWrite(25,i);
//  Serial.println("Beans");
//  for(int i=254;i>0;i--)
//    dacWrite(25,i);
//}
