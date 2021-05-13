void back_to_landing() {
  menu_state = 0;
  state = 0;
  display_landing();
  update_landing();
}

void reset_game() {
  // Playback variables
  note_index = 0;
  measure_index = 0;
  playing_measure = false;
  playing_song = false;

  // Game state variables
  player_count = 0;
  current_measure = 0;
  curr_note_index = selected_key;
  current_note[0] = '\0';
  strcpy(current_note, (is_flat_key ? NOTES_FLAT : NOTES_SHARP)[curr_note_index % 12]);
  // update starting index for selected symbol based on the note from this key
  if (current_note[1] == '#') selected_symbol = 0;
  else if (current_note[1] == 'b') selected_symbol = 1;
  else if (current_note[1] == ' ') selected_symbol = 2;
  
  selected_note = curr_note_index % 12;
  selected_duration = 0;
  selected_symbol = 0;
  selected_measure = 0;
  step_index = 0;
  note_state = 0;

  // Other variables
  if (is_host) {
    in_turn = true;
    set_led_color(0, 255, 0);
  } else {
    in_turn = false;
    set_led_color(255, 0, 0);
  }
  input_cursor = 0;
  state = 4;
  is_locked = !in_turn;
  tft.setTextSize(1);
}

void update_state(int bv, int js) {
  if (state == 0) {
    process_landing_state(bv, js);
  } else if (state == 1) {
    process_start_game(bv, js);
  } else if (state == 2) {
    process_join_game(bv, js);
  } else if (state == 3) {
    process_waiting_room(bv, js);
  } else if (state == 4) {
    process_in_game(bv, js);
  } else if (state == 5) {
    process_game_menu(bv, js);
  } else if (state == 6) {
    process_end_game(bv, js);
  }
}

////////////////////// landing //////////////////////

void process_landing_state(int bv, int js) {
  // Handle joystick input
  if (js == 1) { // up
    menu_state = (menu_state + 2) % 3;
    update_landing();
  } else if (js == 3) { // down
    menu_state = (menu_state + 1) % 3;
    update_landing();
  }

  // Handle button input
  if (bv == 1) {
    if (menu_state == 0) { // start
      state = 1;
      display_start_game();
    } else if (menu_state == 1) { // join
      game_code_input[0] = -10; // equivalent to 0 mod 10
      game_code_input[1] = -10;
      game_code_input[2] = -10;
      input_cursor = 0;
      is_locked = true;
      menu_state = 0;
      state = 2;
      display_join_game();
    } else { // gallery
      state = 7;
      display_gallery();
    }
    menu_state = 0;
  } else if (bv == 2) {
    sound_on = !sound_on;
    update_landing();
  }
}

////////////////////// start game //////////////////////

void process_start_game(int bv, int js) {

  // Handle joystick input
  if (!is_locked && js == 1) { // up
    menu_state = (menu_state + 3) % 4;
  } else if (!is_locked && js == 3) { // down
    menu_state = (menu_state + 1) % 4;
  } else if (is_locked && js == 2) { // right
    if (menu_state == 0) {
      selected_key = (selected_key + 1) % 12;
      play_note(selected_key);
    }
    else selected_tempo = (selected_tempo + 1) % 3;
  } else if (is_locked && js == 4) { // left
    if (menu_state == 0) {
      selected_key = (selected_key + 11) % 12;
      play_note(selected_key);
    }
    else selected_tempo = (selected_tempo + 2) % 3;
  }
  update_start_game(js);

  // Handle button input
  if (bv == 1) {
    if (menu_state == 0 || menu_state == 1) { // inputs
      is_locked = !is_locked;
      if (!is_locked) stop_sound();
      else if (menu_state == 0) play_note(selected_key);
      update_start_game(1);
    } else if (menu_state == 2) { // start
      create_game_http();
      is_host = true;
      is_locked = true;

      state = 3;
      num_players = 1;
      Serial.println("Host joining waiting room");
      Serial.println(player_list);
      wait_room_timer = millis();
      display_waiting_room();
    } else  { // back
      back_to_landing();
    }
  } else if (bv == 2) {
    sound_on = !sound_on;
    update_start_game(6); // TODO: toggle sound indicator
  }
}

////////////////////// join game //////////////////////

void process_join_game(int bv, int js) {
  // Handle joystick input
  if (js == 1 || js == 3) {
    if (is_locked) {
      game_code_input[input_cursor] = (game_code_input[input_cursor] + (js == 1 ? 11 : 19)) % 10;
    } else {
      menu_state = (menu_state + (js == 1 ? 2 : 1)) % 3;
    }
  } else if (is_locked && js) {
    input_cursor = (input_cursor + (js == 2 ? 1 : 2)) % 3;
  }
  update_join_game(js);

  // Handle button input
  if (bv == 1) {
    if (menu_state == 0) { // input
      if (!is_locked) is_locked = true;
      else if (game_code_input[0] >= 0 && // three numbers specified
               game_code_input[1] >= 0 &&
               game_code_input[2] >= 0) is_locked = false;
      update_join_game(1);
    } else if (menu_state == 1) { // join
      if (join_game_http()) {
        is_host = false;
        is_locked = true;
        get_game_status();
        
        state = 3;
        Serial.println("Player joining waiting room");
        Serial.println(player_list);
        wait_room_timer = millis();
        display_waiting_room();
      }
    } else { // back
      back_to_landing();
    }
  } else if (bv == 2) {
    sound_on = !sound_on;
    update_join_game(6);
  }
}

////////////////////// waiting room //////////////////////

void process_waiting_room(int bv, int js) {
  if (game_state == 2) { // game started
    state = 4;
    reset_game();
    display_in_game();
  } else {
    if (bv == 1 && is_host) start_game_http();
    else if (millis() - wait_room_timer > PING_INTERVAL) {
      get_game_status();
      update_waiting_room();
      Serial.printf("Num players: %d \n", num_players);
      Serial.println(player_list);
      wait_room_timer = millis();
    }
  }
}

////////////////////// in-game //////////////////////

void process_in_game(int bv, int js) {
  if (current_measure == MEASURE_COUNT) { // if reach measure count limit, need to end game
    display_end_game();
    state = 6;
    return;
  }
  if (!is_locked && js) { // scrolling up and down the menu
    if (js == 1) { // up
      menu_state = (menu_state + 4) % 5;
    } else if (js == 3) { // down
      menu_state = (menu_state + 1) % 5;
    }
  } else if (is_locked && js) { // scrolling through an option
    if (menu_state == 0) { // note selection
      if (js == 2 || js == 4) { // left/right
        if (js == 2) { // right
          if (curr_note_index + SCALE_STEPS[(step_index + 1) % 8] <= 35) {
            step_index = (step_index + 1) % 8;
            curr_note_index = curr_note_index + SCALE_STEPS[step_index];
            selected_note = curr_note_index % 12;
          }
        } else if (js == 4) { // left
          if (curr_note_index - SCALE_STEPS[step_index] >= 0) {
            curr_note_index = curr_note_index - SCALE_STEPS[step_index];
            step_index = (step_index + 7) % 8; //same as subtracting 1
            selected_note = curr_note_index % 12;
          }
        }
        Serial.printf("Current note index updated %d \n", curr_note_index);
  
        // Display selected note
        current_note[0] = '\0';
        if (step_index == 7) {
          stop_sound();
          sprintf(current_note, "R ");
          Serial.println("Current note is rest");
        } else {
          sprintf(current_note, (is_flat_key ? NOTES_FLAT : NOTES_SHARP)[selected_note]);
          Serial.printf("Current note is %s \n", current_note);
        }
        
        if (current_note[1] == '#') {
          selected_symbol = 2;
        } else if (current_note[1] == 'b') {
          selected_symbol = 0;
        } else if (current_note[1] == ' ') {
          selected_symbol = 1;
        }
        adjustment = 0;
      } else { // Changing symbol
        if (current_note[0] == 'R') return;
        if (js == 1) { // up
          if (selected_symbol == 2) {
            if (curr_note_index + adjustment - 2 >= 0) {
              adjustment -= 2;
              selected_symbol = (selected_symbol + 1) % 3;
            }
          } else {
            if (curr_note_index + adjustment + 1 <= 35) {
              adjustment += 1;
              selected_symbol = (selected_symbol + 1) % 3;
            }
          }
          current_note[1] = SYMBOLS[selected_symbol];
        } else if (js == 3) { // down
          if (selected_symbol == 0) {
            if (curr_note_index + adjustment + 2 <= 35) {
              adjustment += 2;
              selected_symbol = (selected_symbol + 2) % 3;
            }
          } else {
            if (curr_note_index + adjustment - 1 >= 0) {
              adjustment -= 1;
              selected_symbol = (selected_symbol + 2) % 3;
            }
          }
          current_note[1] = SYMBOLS[selected_symbol];
        }
      }
      
      Serial.printf("Current note is %s, adjustment %d, current_note_index %d \n", current_note, adjustment, curr_note_index);
      int temp_note_num = current_note[0] == 'R' ? 36 : curr_note_index + adjustment;
      if (note_state < 16) measures[current_measure][note_state] = temp_note_num;
      if (temp_note_num != 36) play_note(temp_note_num);
    } else if (menu_state == 1) { // duration selection
      if (js == 2) { // right
        selected_duration = (selected_duration + 1) % 5;
      } else if (js == 4) { // left
        selected_duration = (selected_duration + 4) % 5;
      }
    } else if (menu_state == 4) {
      uint8_t old_selected_measure = selected_measure;
      if (js == 2) { // right
        selected_measure = (selected_measure + 1) % (current_measure + 1);
      } else if (js == 4) { // left
        selected_measure = (selected_measure + current_measure) % (current_measure + 1);
      }
      if (old_selected_measure != selected_measure) display_in_game();
      Serial.printf("Selected measure: %d", selected_measure);
    }
  }

  bool note_added = false;
  if (bv == 1) {
    if (in_turn) {
      if (!is_locked && (menu_state < 2 || menu_state == 4)) is_locked = true;
      else is_locked = false;
    }
    // state changes
    if (menu_state == 0) {
      play_note(curr_note_index);
    }
    if (menu_state == 2) { // add a note
      int num_notes_added = min(16 - note_state, pow(2, selected_duration));

      if (num_notes_added) { // if nonzero
        measures[current_measure][note_state] = current_note[0] == 'R' ? 36 : (curr_note_index + adjustment);
        for (int i = 1; i < num_notes_added; i++) measures[current_measure][note_state + i] = 37;
        note_state += num_notes_added;
        menu_state = 0;
        if (note_state < 16) // display currently selected note in next note index, not C
          measures[current_measure][note_state] = current_note[0] == 'R' ? 36 : (curr_note_index + adjustment);
      }
      note_added = true;
    } else if (menu_state == 3) {
      while (note_state < 16) {
        measures[current_measure][note_state] = 36;
        note_state += 1;
      }
      submit_measure();

      // Update internal
      note_state = 0;
      current_measure += 1;
      selected_measure = current_measure;

      // Update controls
      menu_state = 4;
      in_turn = false;
      is_locked = true;

      // Update visuals
      display_in_game();
      set_led_color(255, 0, 0);
    }
  } else if (bv == 2) { // go to game menu screen
    state = 5;
    menu_state = 0;
    is_locked = false;
    display_game_menu();
  }
  
  if ((bv || js) && state == 4) update_in_game(js, note_added);
}

////////////////////// game menu //////////////////////

void process_game_menu(int bv, int js) {
  // Handle joystick input
  if (js == 1) { // up
    menu_state = (menu_state + 4) % 5;
    update_game_menu();
  } else if (js == 3) { // down
    menu_state = (menu_state + 1) % 5;
    update_game_menu();
  }

  // Handle button input
  if (bv) {
    if (playing_song || playing_measure) {
      stop_sound();
      playing_song = false;
      playing_measure = false;
      return;
    }
    
    update_game_menu();
    if (menu_state == 0) { // resume game
      state = 4;
      is_locked = true;
      display_in_game();
    } else if (menu_state == 1) {
      if (playing_song) stop_sound();
      playing_song = !playing_song;
    } else if (menu_state == 2) {
      if (playing_measure) stop_sound();
      playing_measure = !playing_measure;
    } else if (menu_state == 3) {
      sound_on = !sound_on;
      playing_song = false;
      playing_measure = false;
      stop_sound();
      update_game_menu();
    } else if (menu_state == 4) { // leave game
      leave_game();
      is_locked = false;
      game_state = 0;
      back_to_landing();
    }
  }
}

////////////////////// end game //////////////////////

void process_end_game(int bv, int js) { // TODO: END GAME SERVER LOGIC and clearing all measures
  if (bv == 1) {
    is_locked = false;
    back_to_landing();
  }
}
