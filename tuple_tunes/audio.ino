/////////////
//  Audio  //
/////////////

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

void play_measure(uint8_t* measure_input) {
  // Play note
  double note_period = 15000.0/TEMPO_SPEEDS[selected_tempo]; // change ig?
  int num_notes = selected_measure == current_measure ? note_state : 16;
  if (note_index < num_notes) {
    selected_note = measure_input[note_index];
    if (millis() - last_played > note_period) {
      Serial.printf("Note %d is %d\n", note_index, measure_input[note_index]);
      if (selected_note == NOTE_COUNT) {
        stop_sound();
      } else if (selected_note < NOTE_COUNT) {
        play_note(selected_note);
      }
      last_played = millis();
      note_index++;
    }
  }
  
  // Stop playing this measure after last note
  if (note_index == num_notes) {
    if (millis() - last_played > note_period) {
      stop_sound();
      playing_measure = false;
      note_index = 0;
      if (playing_song) {
        measure_index++;
      }
    }
  }
}

void play_song(int curr_num_measures){
  if (measure_index < current_measure + 1) {
    play_measure(measures[measure_index]);
  } else {
    measure_index = 0;
    stop_sound();
    playing_song = false;
  }
}

//void sine_wave(double freq) {
//  dacWrite(AUDIO_TRANSDUCER, 3.3);
//}
