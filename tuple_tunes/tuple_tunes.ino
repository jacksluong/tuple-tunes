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

char measure_response[200];
char player_in_turn[50];
char USERNAME[] = "jkluong";

char SERVER[] = "608dev-2.net";
char IN_GAME_ADDRESS[] = "/sandbox/sc/team59/server/logic.py";
char PRE_GAME_ADDRESS[] = "/sandbox/sc/team59/server/init.py";

const uint16_t OUT_BUFFER_SIZE = 2000;
char response[OUT_BUFFER_SIZE];
const uint16_t IN_BUFFER_SIZE = 2000;
char request[IN_BUFFER_SIZE];
uint32_t time_since_last_ping = millis();
const int PING_INTERVAL = 6000;

// Waiting room
uint32_t wait_room_timer = millis();
char player_list[200];
bool is_host = false; //determining if you are the host / made the create_game request
uint8_t num_players = 1;
uint8_t game_state = 0; // consistent with return numbers of get game status

// LED
const uint8_t red = 14;
const uint8_t green = 13;
const uint8_t blue = 15;

// Display
TFT_eSPI tft = TFT_eSPI();
const uint8_t SCREEN_WIDTH = 160;
const uint8_t SCREEN_HEIGHT = 128;

// Colors
uint8_t CYAN[] = {18, 243, 255};
uint8_t DARK_CYAN[] = {54, 131, 135};
uint8_t GRAY[] = {100, 100, 100};
uint8_t DARK_GRAY[] = {50, 50, 50};

// Input
Button button(BUTTON_PINS[0]);
Joystick joystick(JOYSTICK_LR, JOYSTICK_UD);

// State / Control
uint8_t state = 0;
uint8_t menu_index = 0;
int8_t game_code_input[] = {-10, -10, -10};
char input_cursor = 0;
bool is_locked = false;
uint32_t last_button_click = millis();

// Audio output (see lab05b)
const int NOTE_COUNT = 36;
double note_freqs[NOTE_COUNT];
double MULT = 1.059463094359; //12th root of 2 (precalculated) for note generation
double C3 = 130.81; //C3 130.81 Hz  for note generation
uint32_t last_played = 0;

// Game constants
const char* TEMPO_LABELS[] = {"Slow", "Mid", "Fast"};
const uint8_t TEMPO_SPEEDS[] = {60, 96, 144};
const char* NOTE_DURATIONS[] = {"1/16", "1/8", "1/4", "1/2", "1"};
const char SYMBOLS[] = {'b', ' ', '#'};
const int MEASURE_COUNT = 6;
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
uint16_t game_id;
char game_code[5];
uint8_t player_count = 0;

bool in_turn = false;
uint8_t measures[MEASURE_COUNT][16];
int current_measure = 0;
int selected_measure = 0; // the measure the player is viewing
int selected_note = 0;
int note_num = 0;
int adjustment = 0;
char current_note[5] = "\0";
int selected_duration = 0; // selected duration index for current note
int selected_symbol = 0; // selected symbol index for current note
int step_index = 0; // selected jump index for determining next note in key
int note_state = 0;

// PLEASE DON'T CHANGE ANYTHING BELOW:
TaskHandle_t Task1;
const int SPEAKER_PIN = 25;
double current_freq = 0;
bool play_sound_bool = false; 


/////////////////
// Convenience //
/////////////////

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
  Serial.println("\n\n\n\n\n\n*********\nGreetings and salutations!\n*********\n");

  // Set up screen
  tft.init();
  tft.setRotation(1); // set display in landscape
  loading_page();

  // Set up button(s)
  delay(100);
  for (int pin : BUTTON_PINS) pinMode(pin, INPUT_PULLUP);

  // Set up joystick
  pinMode(JOYSTICK_LR, INPUT);
  pinMode(JOYSTICK_UD, INPUT);
  analogReadResolution(10);

  // Set up LED
  ledcSetup(red, 80, 8);
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
  pinMode(SPEAKER_PIN, OUTPUT);
  disableCore0WDT();

  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

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
  note_freqs[0] = C3*2*PI/1000000;
  for (int i = 1; i < NOTE_COUNT; i++) {
    note_freqs[i] = MULT*note_freqs[i - 1];
  }
  
  // Draw first screen
  back_to_landing();
}

void loop() {
  int bv = button.read();
  int js = joystick.read();
    
  if (bv) last_button_click = millis();
  
  update_state(bv, js);

  processes();
}

void processes() {
  if (is_locked && note_state < 16 && 
      !(state == 3 && !is_host) && 
      !(state == 0 && !in_turn)) draw_cursor();
  
  // Fetch/ping
  if ((state == 4 || state == 5) && millis() - time_since_last_ping > PING_INTERVAL) {
    if (in_turn) ping();
    else if (!fetch_game_state(game_id)) { // condition true only when game ended (prematurely)
      display_end_game();
      state = 6;
    }
  }

  // Sounds
  if (playing_measure && sound_on) play_measure(selected_measure);
  if (playing_song && sound_on) play_song();
  if ((state == 1 || state == 4) && millis() - last_played > 300) stop_sound();
}

// WITH SPEAKER:
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  while(1){
    //Serial.printf("Play sound bool: %d, Frequency: %f\n", play_sound_bool, current_freq); 
    //if (play_sound_bool){
      //Serial.println("playing");
      dacWrite(SPEAKER_PIN, 128+19*sin(current_freq*micros())-60*sin(2*current_freq*micros())+25*sin(3*current_freq*micros()));
    //}

  } 
}
