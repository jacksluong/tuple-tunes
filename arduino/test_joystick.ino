#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include <WiFiClient.h>
#include <TFT_eSPI.h>

const uint8_t PIN1 = 34;
const uint8_t PIN2 = 35;
int reading1;
int reading2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PIN1, INPUT);
  pinMode(PIN2, INPUT);
  reading1 = 0;
  reading2 = 0;
  analogReadResolution(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  reading1 = analogRead(PIN1);
  reading2 = analogRead(PIN2);
  Serial.println(reading1);
  Serial.println(reading2);
  delay(1000);
}
