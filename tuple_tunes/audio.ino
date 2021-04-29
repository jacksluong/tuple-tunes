/////////////
//  Audio  //
/////////////

uint32_t last_played = 0;
double note_freqs_mid[] = {261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88};

struct Measure {
  double notes[16]; //the notes (array of doubles containing frequencies in Hz. I used https://pages.mtu.edu/~suits/notefreqs.html
  boolean is_hold[16];
  float note_period; //the timing of each note in milliseconds (take bpm, scale appropriately for note (sixteenth note would be 4 since four per quarter note) then
};

void play_note(int index) {
  ledcWriteTone(AUDIO_PWM, note_freqs_mid[index]);
  last_played = millis();
  Serial.println("Playing sound");
}

void stop_sound() {
  ledcWriteTone(AUDIO_PWM, 0);
  Serial.println("Off sound");
}
