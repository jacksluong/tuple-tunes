#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <BMP280_DEV.h>
#include <ArduinoJson.h>
#include "Button.h"
#include "TextInput.h"
#include "NumberInput.h"

/////////////
// Globals //
/////////////

// Pins for buttons, LCD, AUDIO CONTROL
int BUTTON_PINS[] = {19, 3, 5, 13};
const int AUDIO_IN = A0;
uint8_t LCD_CONTROL = 25;
uint8_t AUDIO_TRANSDUCER = 26;

// HTTP
char NETWORK[] = "MIT";
char PASSWORD[] = "";
char USERNAME[] = "jkluong";

const uint16_t OUT_BUFFER_SIZE = 1000;
char response[OUT_BUFFER_SIZE];

// LED
const uint32_t red = 0;
const uint32_t green = 13;
const uint32_t blue = 15;

// Parts
MPU6050 imu;
BMP280_DEV bmp280(21, 22);
TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;

// Buttons
Button button1(BUTTON_PINS[0]);
Button button2(BUTTON_PINS[1]);
Button button3(BUTTON_PINS[2]);
Button button4(BUTTON_PINS[3]);

// Audio output (see lab05b)
double MULT = 1.059463094359; // 12th root of 2 (precalculated) for note generation
double A1 = 55; // A1 55 Hz for note generation
const uint8_t NOTE_COUNT = 97; // number of half-steps included in our range of notes

// State and inputs
char input[50] = {0};
TextInput text_input(input);
NumberInput number_input;
int state = 0;

// Below are project-specific variables



////////////////////////////////
// Project-specific functions //
////////////////////////////////

void example() {
  
}

/////////////////////////////////
// Convenient helper functions //
/////////////////////////////////

/*
 * Sets the color of the LED given RGB input values.
 */
void set_led_color(int r, int g, int b) {
  ledcWrite(red, r);
  ledcWrite(green, g);
  ledcWrite(blue, b);
}

////////////////////
// Setup and loop //
////////////////////

void setup() {
  Serial.begin(115200);
  Serial.println("*********\nGreetings and salutations!\n*********\n");

  // Set up screen
  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Set up IMU
  if (imu.setupIMU(1)) {
    Serial.println("IMU connected!");
  } else {
    Serial.println("IMU not connected, restarting...");
    ESP.restart(); // restart the ESP (proper way)
  }

  // Set up temp/pressure sensor
  bmp280.begin(0x77);
  bmp280.setTimeStandby(TIME_STANDBY_2000MS);
  bmp280.startNormalConversion();

  // Set up buttons
  delay(100);
  for (int pin : BUTTON_PINS) pinMode(pin, INPUT_PULLUP);

  // Set up LED
  ledcSetup(red, 50, 8);
  ledcSetup(green, 80, 8);
  ledcSetup(blue, 80, 8);
  
  ledcAttachPin(14, red); 
  ledcAttachPin(27, green);
  ledcAttachPin(12, blue);

  set_led_color(0, 0, 0);

  // Connect to WiFi
  WiFi.begin(NETWORK, PASSWORD);

  uint8_t count = 0; // count used for WiFi check times
  Serial.print("Attempting to connect to ");
  Serial.println(NETWORK);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { // if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());
    delay(500);
  } else { // try again if failed to connect
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  
  // Draw first screen
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0, 0);
}

void loop() {
  int bv1 = button1.update();
  int bv2 = button2.update();
  int bv3 = button3.update();
  int bv4 = button4.update();

  imu.readAccelData(imu.accelCount);
  float x = -imu.accelCount[0] * imu.aRes;
  float y = -imu.accelCount[1] * imu.aRes;
  float z = -imu.accelCount[2] * imu.aRes;
  
  if (state == 0) {
    
  } else if (state == 1) {
    
  } else if (state == 2) {
    
  }
}
