/////////////
//  Audio  //
/////////////

uint32_t last_played = 0;

void play_note(int index) {
  if (sound_on) {
    ledcWriteTone(AUDIO_PWM, note_freqs[index]);
    last_played = millis();
  }
//  dac_output_voltage(DAC_CHANNEL_2, 200);
//  int num = 0;
//  while (num < 500){
//    dacWrite(AUDIO_TRANSDUCER, 3.3);
//    num = num + 1
//  }
}

void stop_sound() {
  ledcWriteTone(AUDIO_PWM, 0);
}

void play_measure(int measure_input[16], int num_notes) {
  double note_period = 15000.0/(TEMPO_SPEEDS[selected_tempo]); // change ig?
  if (note_index < num_notes) {
    selected_note = measure_input[note_index];
    if (millis() - last_played > note_period){
      if (selected_note == NOTE_COUNT) {
        stop_sound();
      }
      else if (selected_note < NOTE_COUNT) {
        play_note(selected_note);
      }
      last_played = millis();
      note_index = note_index + 1;
    }
  }
  if (note_index == num_notes){
    if (millis() - last_played > note_period) {
      stop_sound();
      playing_measure = false;
      note_index = 0;
      if (playing_song) {
        measure_index = measure_index + 1;
      }
    }
  }
  
}

void play_song(int song_input[2][16]){
  if (measure_index < MEASURE_COUNT) {
    if (song_input[measure_index][0] > -1) play_measure(song_input[measure_index], 16);
    else {
      playing_song = false;
      measure_index = 0;
    }
  }
  if (measure_index == MEASURE_COUNT) {
    measure_index = 0;
    stop_sound();
    playing_song = false;
  }
}

//void sine_wave(double freq) {
//  dacWrite(AUDIO_TRANSDUCER, 3.3);
//}
