#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <BMP280_DEV.h>
#include <ArduinoJson.h>
#include "Button.h"
#include "Joystick.h"

/////////////
// Globals //
/////////////

// Pins for buttons, LCD, AUDIO CONTROL, joystick
int BUTTON_PINS[] = {32};
uint8_t AUDIO_TRANSDUCER = 26;
const int JOYSTICK_LR = 35;
const int JOYSTICK_UD = 34;
uint8_t AUDIO_PWM = 1;

// HTTP
char NETWORK[] = "MIT";
char PASSWORD[] = "";
char USERNAME[] = "jkluong";

const uint16_t OUT_BUFFER_SIZE = 2000;
char response[OUT_BUFFER_SIZE];

// LED
const uint32_t red = 0;
const uint32_t green = 13;
const uint32_t blue = 15;

// Display
TFT_eSPI tft = TFT_eSPI();
const int SCREEN_WIDTH = 160;
const int SCREEN_HEIGHT = 128;

// Input
Button button(BUTTON_PINS[0]);
Joystick joystick(JOYSTICK_LR, JOYSTICK_UD);

// Audio output (see lab05b)
const int NOTE_COUNT = 36;
double note_freqs[NOTE_COUNT];
double MULT = 1.059463094359; //12th root of 2 (precalculated) for note generation
double C3 = 130.81; //C3 130.81 Hz  for note generation

// State
int state = 0;
int menu_state = 0;
int8_t game_code_input[] = {-10, -10, -10};
char input_cursor = 0;
bool is_locked = false;
uint16_t last_button_click = millis();
int note_state = 0;

// Game variable options
char* notes_flat[] = {"C ", "Db", "D ", "Eb", "E ", "F ", "Gb", "G ", "Ab", "A ", "Bb", "B "};
char* notes_sharp[] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};      // C major
char* tempo_labels[] = {"Slow", "Mid", "Fast"};
int tempo_speeds[] = {60, 96, 144};
int selected_key = 0;
int selected_tempo = 0;

// Game variables
char room_num[4];
int song_key;
int tempo;
int player_count;
char* measures[100] = {"\0"};
/* can delete this comment later but for future reference,
 *  each element in measures will contain a single string formatted
 *  to contain all information in that measure:
 *  e.g. "E E E ~ E E E ~ E G C D E ~ R ~ "
 *  (16 tokens, separated by spaces, with an additional one at the end
 *  to make life easier when parsing)
 */
int current_measure;

struct Measure {
  int notes[16]; //the notes (array of integers containing indices, later to find in note_freqs.)
  int bpm; //the timing of each note in milliseconds (take bpm, scale appropriately for note. This is 15000/bpm.
};

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
  Serial.println("\n*********\nGreetings and salutations!\n*********\n");

  // Set up screen
  tft.init();
  tft.setRotation(1); // set display in landscape
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED);
  tft.setTextSize(2);
  tft.setCursor(15, 50);
  tft.println("Loading...");

  // Set up button(s)
  delay(100);
  for (int pin : BUTTON_PINS) pinMode(pin, INPUT_PULLUP);

  // Set up joystick
  pinMode(JOYSTICK_LR, INPUT);
  pinMode(JOYSTICK_UD, INPUT);
  analogReadResolution(10);

  // Set up LED
  ledcSetup(red, 50, 8);
  ledcSetup(green, 80, 8);
  ledcSetup(blue, 80, 8);
  
  ledcAttachPin(14, red); 
  ledcAttachPin(27, green);
  ledcAttachPin(12, blue);

  set_led_color(0, 0, 255);

  //set up AUDIO_PWM which we will control in this lab for music:
  pinMode(AUDIO_TRANSDUCER, OUTPUT);
  ledcSetup(AUDIO_PWM, 0, 12);//12 bits of PWM precision
  ledcWrite(AUDIO_PWM, 80); //0 is a 0% duty cycle for the NFET
  stop_sound();
  ledcAttachPin(AUDIO_TRANSDUCER, AUDIO_PWM);

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

  note_freqs[0] = C3;
  //fill in note_freq with appropriate frequencies from 55 Hz to 55*(MULT)^{NOTE_COUNT-1} Hz
  for (int i = 1; i < NOTE_COUNT; i++) {
    note_freqs[i] = MULT*note_freqs[i-1];
  }

  Measure test = {{7, 4, 4, 2, 4, 7, 7, 37, 9, 9, 12, 9, 9, 7, 7, 37}, 40};
  play_measure(test);
  
  // Draw first screen
  back_to_landing();
}

void loop() {
  int bv = button.read();
  int js = joystick.read();
  
  if (bv) last_button_click = millis();
  if (is_locked) draw_cursor();
  
  update_state(bv, js);

  // separating that huge chunk of nested ifs to a separate file so
  // when we handle continuous fetching, it doesn't get mixed in with it all here
}
