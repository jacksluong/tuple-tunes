#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <BMP280_DEV.h>
#include <ArduinoJson.h>
#include <driver/dac.h>
#include "Button.h"
#include "Joystick.h"

/////////////
// Globals //
/////////////

// Pins for buttons, LCD, AUDIO CONTROL, joystick
int BUTTON_PINS[] = {32};
uint8_t AUDIO_TRANSDUCER = 26;
//dac_channel_t AUDIO_TRANSDUCER = 26;
const int JOYSTICK_LR = 35;
const int JOYSTICK_UD = 34;
uint8_t AUDIO_PWM = 1;

// HTTP
char NETWORK[] = "MIT";
char PASSWORD[] = "";
char USERNAME[] = "Irene";

uint16_t game_id;
char game_code[5];
char measure_response[200];
char player_in_turn[50];

char SERVER[] = "608dev-2.net";
char IN_GAME_ADDRESS[] = "/sandbox/sc/team59/server/logic.py";
char START_GAME_ADDRESS[] = "/sandbox/sc/team59/server/init.py";

const uint16_t OUT_BUFFER_SIZE = 2000;
char response[OUT_BUFFER_SIZE];
const uint16_t IN_BUFFER_SIZE = 2000;
char request[IN_BUFFER_SIZE];
uint32_t time_since_last_ping = millis();

// LED
const uint8_t red = 0;
const uint8_t green = 13;
const uint8_t blue = 15;

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

// State / Control
int state = 0;
int menu_state = 0;
int8_t game_code_input[] = {-10, -10, -10};
char input_cursor = 0;
bool is_locked = false;
uint16_t last_button_click = millis();

// Game constants
const char* TEMPO_LABELS[] = {"Slow", "Mid", "Fast"};
const uint8_t TEMPO_SPEEDS[] = {60, 96, 144};
const char* NOTE_DURATIONS[] = {"1/16", "1/8", "1/4", "1/2", "1"};
const char SYMBOLS[] = {'b', ' ', '#'};
const int MEASURE_COUNT = 16;
const uint8_t SCALE_STEPS[] = {0, 2, 2, 1, 2, 2, 2, 1}; // half steps that we need per scale
const char* NOTES_FLAT[] = {"C ", "Db", "D ", "Eb", "E ", "F ", "Gb", "G ", "Ab", "A ", "Bb", "B "}; // Db, Eb, F, Gb, Ab, Bb
const char* NOTES_SHARP[] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "}; // C, D, E, G, A, B

// Playback
int note_index = 0; // current note being played in measure
int measure_index = 0; // current measure being played in song
bool playing_measure = false;
bool playing_song = false;
bool sound_on = true;

// Game state options
bool is_flat_key = false;
int selected_key = 0; // iterates through NOTES_FLAT in start game
int selected_tempo = 0;
char room_num[4];

// Game state variables
int player_count = 0;
bool in_turn = false;
uint8_t measures[MEASURE_COUNT][16];
int current_measure = 0;
int selected_note = 0;
int curr_note_index = 0;
int adjustment = 0;
char current_note[5] = "\0";
int curr_notes_array[16]; //use note state to get index, temporary until we implement measures
int selected_duration = 0; // selected duration index for current note
int selected_symbol = 0; // selected symbol index for current note
int step_index = 0; // selected jump index for determining next note in key
int note_state = 0;

// Tests
int test1[16] = {7, 4, 4, 2, 4, 7, 7, 37, 9, 9, 12, 9, 9, 7, 7, 37};
int test2[16] = {7, 4, 4, 2, 4, 7, 7, 37, 9, 9, 7, 0, 4, 2, 0, 37};
int test_song[2][16] = {{7, 4, 4, 2, 4, 7, 7, 37, 9, 9, 12, 9, 9, 7, 7, 37}, {7, 4, 4, 2, 4, 7, 7, 37, 9, 9, 7, 0, 4, 2, 0, 37}};

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
  //dac_output_enable(DAC_CHANNEL_2);
  //dac_output_voltage(DAC_CHANNEL_2, 200);
  ledcSetup(AUDIO_PWM, 0, 12);//12 bits of PWM precision
  ledcWrite(AUDIO_PWM, 80); //0 is a 0% duty cycle for the NFET
  stop_sound();
  ledcAttachPin(AUDIO_TRANSDUCER, AUDIO_PWM);

  // Connect to WiFi
  WiFi.begin(NETWORK, PASSWORD);

  uint8_t count = 0;
  Serial.print("Attempting to connect to ");
  Serial.println(NETWORK);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  while (!WiFi.isConnected()) {
    delay(1000);
    Serial.printf("Connecting... (%s)\n", WiFi.status());
  }
  Serial.println("CONNECTED!");
  Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                WiFi.localIP()[1], WiFi.localIP()[0],
                WiFi.macAddress().c_str() , WiFi.SSID().c_str());
  delay(500);
  
  // Compute note frequencies
  note_freqs[0] = C3;
  for (int i = 1; i < NOTE_COUNT; i++) {
    note_freqs[i] = MULT*note_freqs[i-1];
  }

  //testing
  create_game_http();
  
  // Draw first screen
  back_to_landing();
}

void loop() {
  if (playing_measure && sound_on)  play_measure(curr_notes_array, note_state);
  if (playing_song && sound_on) play_song(test_song);
  int bv = button.read();
  int js = joystick.read();
    
  if (bv) last_button_click = millis();
  if (is_locked && note_state < 16) draw_cursor();
  
  update_state(bv, js);
  //stop_sound();
  // separating that huge chunk of nested ifs to a separate file so
  // when we handle continuous fetching, it doesn't get mixed in with it all here
}
