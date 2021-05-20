/////////////
//  Audio  //
/////////////

void play_measure(uint8_t measure_i) {
  // Play note
  uint8_t* this_measure = measures[measure_i];
  double note_period = 15000.0/TEMPO_SPEEDS[selected_tempo]; // change ig?
  int num_notes = measure_i == current_measure ? note_state : 16;
  if (note_index < num_notes) {
    selected_note = this_measure[note_index];
    if (millis() - last_played > note_period) {
      Serial.printf("Playing note index %d (%d) in measure\n", note_index, this_measure[note_index]);
      if (selected_note == NOTE_COUNT) {
        stop_sound();
      } else if (selected_note < NOTE_COUNT) {
        play_note(selected_note);
        prev_freq = selected_note;
      } else {
        stop_sound();
        play_note(prev_freq);
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

void play_song() {
  if (measure_index < current_measure + 1) {
    play_measure(measure_index);
  } else {
    measure_index = 0;
    stop_sound();
    playing_song = false;
  }
}

void play_note(int index) {
  if (sound_on){
    current_freq = note_freqs[index];
    last_played = millis();
  }
}

void stop_sound() {
  current_freq = 0;
}
