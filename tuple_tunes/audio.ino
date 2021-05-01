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
  
  double note_period = 15000.0/(tempo_speeds[selected_tempo]); // change ig?
  if (m_index < 16) {
    note_index = measure[m_index];
    if (millis() - last_played > note_period){
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
  if (m_index == 16){
    if (millis() - last_played > note_period) {
      stop_sound();
      play_measure_bool = false;
      m_index = 0;
    }
  }
  
}

void play_song(){
  
}

//void sine_wave(double freq) {
//  dacWrite(AUDIO_TRANSDUCER, 3.3);
//}
