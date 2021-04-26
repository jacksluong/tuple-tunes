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
int BUTTON_PINS[] = {5, 19, 3, 13};
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
Button button1(BUTTON_PINS[0]); // connected to pin 5
Button button2(BUTTON_PINS[1]);
Button button3(BUTTON_PINS[2]);
Button button4(BUTTON_PINS[3]);

//state machine
const uint8_t LANDING = 0;
const uint8_t START = 1;
const uint8_t JOIN = 2;
const uint8_t GALLERY = 3;
const uint8_t INTURN = 4;
const uint8_t OUTTURN = 5;
const uint8_t MENU = 6;

// Audio output (see lab05b)
double MULT = 1.059463094359; // 12th root of 2 (precalculated) for note generation
double A1 = 55; // A1 55 Hz for note generation
const uint8_t NOTE_COUNT = 97; // number of half-steps included in our range of notes

// State and inputs
char input[50] = {0};
TextInput text_input(input);
NumberInput number_input;
int state = 0; // determines overall game page 
int menu_state = 0; // determines cursor selection on menu page

// Below are project-specific variables
int room_num; // three-digit integer
char song_key[10]; // song key e.g. C Major
char tempo[10]; // song tempo e.g. Allegro
int player_count; // # of players for the current game
int curr_measure; // int number of measure currently on


////////////////////////////////
// Project-specific functions //
////////////////////////////////

void display_in_turn() {
  // dividing line between left and right
  tft.drawLine(106, 0, 106, 128, TFT_WHITE);
  tft.drawLine(107, 0, 107, 128, TFT_WHITE);
  
  // left side grid
  tft.setCursor(5, 6, 1);
  tft.println("<measure 4/4>");
  // horizontal grid lines
  tft.drawLine(0, 19, 106, 19, TFT_WHITE);
  tft.drawLine(0, 20, 106, 20, TFT_WHITE);
  tft.drawLine(0, 44, 106, 44, TFT_WHITE);
  tft.drawLine(0, 45, 106, 45, TFT_WHITE);
  tft.drawLine(0, 69, 106, 69, TFT_WHITE);
  tft.drawLine(0, 70, 106, 70, TFT_WHITE);
  tft.drawLine(0, 94, 106, 94, TFT_WHITE);
  tft.drawLine(0, 95, 106, 95, TFT_WHITE);
  tft.drawLine(0, 119, 106, 119, TFT_WHITE);
  tft.drawLine(0, 120, 106, 120, TFT_WHITE);
  // vertical grid lines
  tft.drawLine(26.5, 19, 26.5, 120, TFT_WHITE);
  tft.drawLine(53, 19, 53, 120, TFT_WHITE);
  tft.drawLine(79.5, 19, 79.5, 120, TFT_WHITE);

  // play notes
  tft.setCursor(8, 30, 1);
  tft.printf("C#");
  
  // right side info
  tft.setCursor(128, 20, 2);
  tft.println("C#"); // selected note
  tft.setCursor(126, 45, 1);
  tft.println("1/4"); // note duration
  tft.drawTriangle(117,45,112,47.5,117,50, TFT_WHITE);
  tft.drawTriangle(153,45,158,47,153,50, TFT_WHITE);
  tft.setCursor(146, 119, 1); // ellipses for menu
  tft.printf(".");
  tft.setCursor(150, 119, 1);
  tft.printf(".");
  tft.setCursor(154, 119, 1);
  tft.printf(".");
}

void display_menu() {
  // dividing line between left and right
  tft.drawLine(84, 0, 84, 128, TFT_WHITE);
  tft.drawLine(85, 0, 85, 128, TFT_WHITE);

  // left side actionables
  tft.setCursor(2, 6, 1);
  tft.println("Menu\r\n");
  tft.setCursor(4, 25, 1);
  tft.println(" Resume\r\n");
  tft.setCursor(4, 50, 1);
  tft.println(" Play Song\r\n");
  tft.setCursor(4, 75, 1);
  tft.println(" Play Measure\r\n");
  tft.setCursor(4, 100, 1);
  tft.println(" Leave Game\r\n");

  // right side static
  tft.setCursor(88, 6, 1);
  tft.println("Game Info\r\n");
  tft.setCursor(88, 25, 1);
  tft.printf(" Room#:%d\r\n", room_num);
  tft.setCursor(88, 50, 1);
  tft.printf(" Key:%s\r\n", song_key);
  tft.setCursor(88, 75, 1);
  tft.printf(" Tempo:%s\r\n", tempo);
  tft.setCursor(88, 100, 1);
  tft.printf(" #Players:%d", player_count);

  // draw triangle cursor for selection
  if (menu_state == 0) {
    tft.drawTriangle(2,25,3.5,27.5,2,30,TFT_WHITE);
  } else if (menu_state == 1) {
    tft.drawTriangle(2,50,3.5,52.5,2,55,TFT_WHITE);
  } else if (menu_state == 2) {
    tft.drawTriangle(2,75,3.5,77.5,2,80,TFT_WHITE);
  } else if (menu_state == 3) {
    tft.drawTriangle(2,100,3.5,102.5,2,105,TFT_WHITE);
  }
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
  tft.setRotation(1); // set display in landscape
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
  
  if (state == LANDING) { // landing page
    
  } else if (state == START) { // start game
    
  } else if (state == JOIN) { // join game
    
  } else if (state == GALLERY) { // gallery
    
  } else if (state == INTURN) { // in game and in turn
    display_in_turn();
  } else if (state == OUTTURN) { // in game and out of turn
    
  } else if (state == MENU) { // menu info page
    display_menu();
  }

}