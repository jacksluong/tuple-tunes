/////////////
//  Audio  //
/////////////

uint32_t last_played = 0;



void play_note(int index) {
  ledcWriteTone(AUDIO_PWM, note_freqs[index]);
  last_played = millis();
}

void stop_sound() {
  ledcWriteTone(AUDIO_PWM, 0);
}

void play_measure(Measure measure) {
  int m_index = 0;
  int note_index;
  double current_note = 0.0;
  while (m_index < 16) {
    note_index = measure.notes[m_index];
    
    current_note = note_freqs[note_index];
    m_index = m_index + 1;
  }
}
