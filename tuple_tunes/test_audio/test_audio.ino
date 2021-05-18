#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include <WiFiClient.h>
#include <TFT_eSPI.h>
#include "soc/rtc_wdt.h"

const int NOTE_COUNT = 36;
double global_freqs[NOTE_COUNT];
double MULT = 1.059463094359; //12th root of 2 (precalculated) for note generation
double C2 = 130.81; //C3 130.81 Hz  for note generation
double val;

int BUTTON_PINS[] = {32};

const int NUM_NOTES = 8;
double music_freqs[NUM_NOTES];
//double music_loop

const int NUM_BITS = 128;
double loop_time[NOTE_COUNT];
double global_freqs_new[NUM_NOTES];
unsigned long last_played;
unsigned long last_note;
int SineValues[NUM_BITS];       // an array to store our values for sine
uint16_t sine_index = 0;
uint8_t note_index_global = 0;
bool play_song = true;
double current_freq;
unsigned long first_played;

TaskHandle_t Task1;

void setup() {
  Serial.begin(115200);
  Serial.println("\n*********\nGreetings and salutations!\n*********\n");
  pinMode(25, OUTPUT);
  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());
  //disableCore1WDT();
  disableCore0WDT();
  //time_period = 1000.0/freq;
  last_played = micros();

  for (int pin : BUTTON_PINS) pinMode(pin, INPUT_PULLUP);

  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    30000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  //esp_task_wdt_delete(Task1);
  
  global_freqs[0] = C2;
  for (int i = 1; i < NOTE_COUNT; i++) {
    global_freqs[i] = MULT*global_freqs[i - 1];
  }

  music_freqs[0] = global_freqs[15];
  music_freqs[1] = global_freqs[13];
  music_freqs[2] = global_freqs[11];
  music_freqs[3] = global_freqs[13];
  music_freqs[4] = global_freqs[15];
  music_freqs[5] = global_freqs[18];
  music_freqs[6] = global_freqs[20];
  music_freqs[7] = global_freqs[23];
  
  float ConversionFactor=(2*PI)/NUM_BITS;        // convert my 0-255 bits in a circle to radians
                                            // there are 2 x PI radians in a circle hence the 2*PI
                                            // Then divide by 256 to get the value in radians
                                            // for one of my 0-255 bits.
//  float RadAngle;                           // Angle in Radians
//  // calculate sine values
//  for(int MyAngle=0;MyAngle<NUM_BITS;MyAngle++) {
//    RadAngle=MyAngle*ConversionFactor;               // 8 bit angle converted to radians
//    //SineValues[MyAngle]=(sin(RadAngle)*64)+32;  // get the sine of this angle and 'shift' up so
//                                            // there are no negative values in the data
//                                            // as the DAC does not understand them and would
//                                            // convert to positive values.
//
//    float first_sine = (sin(RadAngle)*34);
//    float second_sine = (sin(RadAngle*2)*15);
//    float third_sine = (sin(RadAngle*3)*10);
//    float fourth_sine = (sin(RadAngle*4)*5);
////    float first_sine = (sin(RadAngle)*10);
////    float second_sine = (sin(RadAngle*2)*6);
////    float third_sine = (sin(RadAngle*3)*4);
////    float fourth_sine = (sin(RadAngle*4)*2);
//    SineValues[MyAngle] = first_sine + second_sine + third_sine + fourth_sine + 128;
//    Serial.printf(", %d", SineValues[MyAngle]);
//  }
  double time_period = 0;
  for (int i = 0; i < NUM_NOTES; i++) {
    //time_period = 1000000.0/(global_freqs[i]);
    global_freqs_new[i] = 2*PI*music_freqs[i]/1000000;
    //loop_time[i] = time_period / NUM_BITS;
  }
  current_freq = global_freqs_new[0];
  last_note = millis();
  first_played = millis();
}

//void play_sound(double local_loop) {
//  dacWrite(25, SineValues[sine_index]);
//  if (micros() - last_played >= local_loop) {
//    last_played = micros();
//    sine_index = (sine_index+1)%NUM_BITS;
//  }
//    
//}

//void play_sound(double local_freq_new, double local_loop) {
void play_sound(double local_freq_new) {
  
    //val = 2*PI*local_freq*micros()/1000000;
    //dacWrite(25, 128+67*sin(val)+30*sin(2*val)+21*sin(3*val));
  dacWrite(25, 128+19*sin(local_freq_new*micros())+60*sin(2*local_freq_new*micros())+25*sin(3*local_freq_new*micros()));
  
}

void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    if (play_song) {
      //dacWrite(25, 128+60*sin(global_freqs_new[note_index_global]*micros())-19*sin(2*global_freqs_new[note_index_global]*micros())+25*sin(3*global_freqs_new[note_index_global]*micros()));
      dacWrite(25, 128+19*sin(current_freq*micros())-60*sin(2*current_freq*micros())+25*sin(3*current_freq*micros()));
    
    } 
  } 
}

void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  while(1){
//    dacWrite(25, 128+60*sin(global_freqs_new[note_index_global]*micros())+19*sin(2*global_freqs_new[note_index_global]*micros())+25*sin(3*global_freqs_new[note_index_global]*micros()));
//  //  } 
//    if (millis() - last_note > 250) {
//      last_note = millis();
//      //dacWrite(25, 128+60*sin(global_freqs_new[note_index_global]*micros())+19*sin(2*global_freqs_new[note_index_global]*micros())+25*sin(3*global_freqs_new[note_index_global]*micros()));
//      note_index_global = (note_index_global+1)%NUM_NOTES;
//     // dacWrite(25, 128+60*sin(global_freqs_new[note_index_global]*micros())+19*sin(2*global_freqs_new[note_index_global]*micros())+25*sin(3*global_freqs_new[note_index_global]*micros()));
//    }
  } 
}

void loop()
{
  if (millis() - first_played > 6000) {
    play_song = false;
  }
  if (millis() - last_note > 250) {
      last_note = millis();
      //dacWrite(25, 128+60*sin(global_freqs_new[note_index_global]*micros())+19*sin(2*global_freqs_new[note_index_global]*micros())+25*sin(3*global_freqs_new[note_index_global]*micros()));
      note_index_global = (note_index_global+1)%NUM_NOTES;
      current_freq = global_freqs_new[note_index_global];
     // dacWrite(25, 128+60*sin(global_freqs_new[note_index_global]*micros())+19*sin(2*global_freqs_new[note_index_global]*micros())+25*sin(3*global_freqs_new[note_index_global]*micros()));
      //rtc_wdt_reset();
    }
//  dacWrite(25, 128+60*sin(global_freqs_new[note_index_global]*micros())+19*sin(2*global_freqs_new[note_index_global]*micros())+25*sin(3*global_freqs_new[note_index_global]*micros()));
//  //  } 
//    if (millis() - last_note > 250) {
//      last_note = millis();
//      //dacWrite(25, 128+60*sin(global_freqs_new[note_index_global]*micros())+19*sin(2*global_freqs_new[note_index_global]*micros())+25*sin(3*global_freqs_new[note_index_global]*micros()));
//      note_index_global = (note_index_global+1)%NUM_NOTES;
//     // dacWrite(25, 128+60*sin(global_freqs_new[note_index_global]*micros())+19*sin(2*global_freqs_new[note_index_global]*micros())+25*sin(3*global_freqs_new[note_index_global]*micros()));
//    }
//  Serial.print("setup() running on core ");
//  Serial.println(xPortGetCoreID());
  //dacWrite(26, 255);
  //play_sound();
//  if (millis() - last_note < 250) {
    //play_sound(loop_time[note_index_global]);
    //play_sound(global_freqs_new[note_index_global]);
    
}
