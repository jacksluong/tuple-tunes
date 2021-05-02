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

void play_measure(int measure_input[16]) {
  double note_period = 15000.0/(tempo_speeds[selected_tempo]); // change ig?
  if (m_index < 16) {
    note_index = measure_input[m_index];
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
      if (play_song_bool) {
        s_index = s_index + 1;
      }
    }
  }
  
}

void play_song(int song_input[2][16]){
  if (s_index < MEASURE_COUNT) {
    if (song_input[s_index][0] > -1) play_measure(song_input[s_index]);
    else {
      play_song_bool = false;
      s_index = 0;
    }
  }
  if (s_index == MEASURE_COUNT) {
    s_index = 0;
    stop_sound();
    play_song_bool = false;
  }
}

//void sine_wave(double freq) {
//  dacWrite(AUDIO_TRANSDUCER, 3.3);
//}
