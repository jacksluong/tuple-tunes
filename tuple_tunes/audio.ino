/////////////
//  Audio  //
/////////////

uint32_t last_played = 0;

void play_note(int index) {
  ledcWriteTone(AUDIO_PWM, note_freqs[index]);
//  dac_output_voltage(DAC_CHANNEL_2, 200);
//  int num = 0;
//  while (num < 500){
//    dacWrite(AUDIO_TRANSDUCER, 3.3);
//    num = num + 1
//  }
  last_played = millis();
}

void stop_sound() {
  ledcWriteTone(AUDIO_PWM, 0);
}

void play_measure(int measure[16]) {
  int m_index = 0;
  int note_index;
  bool still_playing = true;
  double note_period = 15000.0/(tempo_speeds[selected_tempo]); // change ig?
  if (m_index < 16) {
    note_index = measure[m_index];
    if (millis() - last_played > note_period){
      still_playing = true;
      if (note_index == NOTE_COUNT) {
        stop_sound();
      }
      else if (note_index < NOTE_COUNT) {
        play_note(note_index);
      }
      last_played = millis();
      m_index = m_index + 1;
    }
  }
  if (still_playing && m_index == 16){
    if (millis() - last_played > note_period) {
      stop_sound();
      still_playing = false;
      m_index = 0;
    }
  }
  
}

//void sine_wave(double freq) {
//  dacWrite(AUDIO_TRANSDUCER, 3.3);
//}
