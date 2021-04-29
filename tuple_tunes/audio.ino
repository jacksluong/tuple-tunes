/////////////
//  Audio  //
/////////////

uint32_t last_played = 0;

struct Measure {
  double notes[16]; //the notes (array of doubles containing frequencies in Hz. I used https://pages.mtu.edu/~suits/notefreqs.html
  float note_period; //the timing of each note in milliseconds (take bpm, scale appropriately for note. This is 15000/bpm.
};

void play_note(int index) {
  ledcWriteTone(AUDIO_PWM, note_freqs[index]);
  last_played = millis();
}

void stop_sound() {
  ledcWriteTone(AUDIO_PWM, 0);
}
