/////////////
//  Audio  //
/////////////

uint32_t last_played = 0;

struct Measure {
  int notes[16]; //the notes (array of integers containing indices, later to find in note_freqs.)
  float note_period; //the timing of each note in milliseconds (take bpm, scale appropriately for note. This is 15000/bpm.
};

void play_note(int index) {
  ledcWriteTone(AUDIO_PWM, note_freqs[index]);
  last_played = millis();
}

void stop_sound() {
  ledcWriteTone(AUDIO_PWM, 0);
}

void play_measure(Measure measure) {
  int m_index = 0;
  while (m_index < 16) {
    m_index = m_index + 1;
  }
}
