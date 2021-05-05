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
  selected_key = 0;
  selected_note = curr_note_index % 12;
  selected_duration = 0;
  selected_symbol = 0;
  step_index = 0;
  note_state = 0;

  // Other variables
  menu_state = 0;
  input_cursor = 0;
  state = 4;
  is_locked = true;
}

void update_state(int bv, int js) {
  if (state == 0) {             
    process_landing_state(bv, js);
  } else if (state == 1) {      
    process_start_game(bv, js);
  } else if (state == 2) { 
    process_join_game(bv, js);
  } else if (state == 3) {      

  } else if (state == 4) {      ////////////////////// in-game //////////////////////
    process_in_game(bv, js);
  } else if (state == 5) {      ////////////////////// game menu //////////////////////
    process_game_menu(bv, js);
  }
}

////////////////////// landing //////////////////////

void process_landing_state(int bv, int js) {
  if (js == 1) { // up
    menu_state = (menu_state + 2) % 3;
    update_landing();
  } else if (js == 3) { // down
    menu_state = (menu_state + 1) % 3;
    update_landing();
  }

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
      state = 3;
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
  if (millis() - last_played > 300) stop_sound();
  if (!is_locked && js == 1) { // up
    menu_state = (menu_state + 3) % 4;
  } else if (!is_locked && js == 3) { // down
    menu_state = (menu_state + 1) % 4;
  } else if (is_locked && js == 2) { // right
    if (menu_state == 0) {
      // selected key is index of the selected key, should not changed after locked
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

  if (bv == 1) {
    if (menu_state == 0 || menu_state == 1) { // inputs
      is_locked = !is_locked;
      if (!is_locked) stop_sound();
      else if (menu_state == 0) play_note(selected_key);
      update_start_game(1);
    } else if (menu_state == 2) { // start
      for (int i = 0; i < 3; i++) room_num[i] = '0'; // TODO: hardcoded room number
      reset_game();
      in_turn = true;
      display_in_game();
    } else  { // back
      back_to_landing();
    }
  } else if (bv == 2) {
    sound_on = !sound_on;
    update_start_game(6);
  }
}

////////////////////// join game //////////////////////

void process_join_game(int bv, int js) {
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

  if (bv == 1) {
    if (menu_state == 0) { // input
      if (!is_locked) is_locked = true;
      else if (game_code_input[0] >= 0 && // three numbers specified
               game_code_input[1] >= 0 &&
               game_code_input[2] >= 0) is_locked = false;
      update_join_game(1);
    } else if (menu_state == 1) { // join
      room_num[0] = '\0';
      sprintf(room_num, "%d%d%d", game_code_input[0], game_code_input[1], game_code_input[2]);
      reset_game();
      in_turn = false;
      display_in_game();
    } else { // back
      back_to_landing();
    }
  } else if (bv == 2) {
    sound_on = !sound_on;
    update_join_game(6);
  }
}

////////////////////// in-game //////////////////////

void process_in_game(int bv, int js) {
  if (millis() - last_played > 300) stop_sound();
  if (!is_locked && js) { // scrolling up and down the menu
    tft.fillCircle(135, 30 + 20 * menu_state, 1, TFT_BLACK);
    if (js == 1) { // up
      menu_state = (menu_state + 4) % 5;
    } else if (js == 3) { // down
      menu_state = (menu_state + 1) % 5;
    }
  } else if (is_locked && js) { // scrolling through note and duration selection
    if (menu_state == 0) { // note and sharp/flat/neutral selection
      tft.fillRect(133, 20, 15, 15, TFT_BLACK); // clear note
      tft.fillRect(8 + 26.5 * (note_state % 4), 28 + 25 * (int(note_state / 4)), 15, 15, TFT_BLACK); // clear grid cell
      if (js == 2) { // right
        if (curr_note_index + SCALE_STEPS[(step_index + 1) % 8] <= 35) {
          step_index = (step_index + 1) % 8;
          curr_note_index = curr_note_index + SCALE_STEPS[step_index];
          selected_note = curr_note_index % 12;
        }
        
        play_note(curr_note_index);
        Serial.printf("Current note index updated %d \n", curr_note_index);

      } else if (js == 4) { // left

        if (curr_note_index - SCALE_STEPS[step_index] >= 0) { // TODO
          curr_note_index = curr_note_index - SCALE_STEPS[step_index];
          step_index = (step_index + 7) % 8; //same as subtracting 1
          selected_note = curr_note_index % 12;
        }
        
        play_note(curr_note_index);
        Serial.printf("Current note index updated %d \n", curr_note_index);
      }

      //current note is what we will be displaying
      current_note[0] = '\0';

      //this is our case for representing rests
      if (step_index == 7) {
        stop_sound();
        strcat(current_note, "R");
        Serial.println("Current note is rest");
      } else if (is_flat_key) {
        strcat(current_note, NOTES_FLAT[selected_note]);
        Serial.printf("Current note is %s", current_note);
      } else {
        strcat(current_note, NOTES_SHARP[selected_note]);
        Serial.printf("Current note is %s", current_note);
      }

      if (js == 2 || js == 4) { // update current selected symbol if note has changed
        if (current_note[1] == '#') {
          selected_symbol = 2;
        } else if (current_note[1] == 'b') {
          selected_symbol = 0;
        } else if (current_note[1] == ' ' || current_note[1] == '\0') {
          selected_symbol = 1;
        }
        adjustment = 0; //we need to reset adjustment each time
      }

      // changing sharp/flat/neutral
      if (js == 1) { // up

        /////////////////////////////
        // edge cases //////////////
        ////////////////////////////
        if (curr_note_index == 0){ //edge case for Cb
          if (selected_symbol == 2){
            adjustment = adjustment - 1;
            selected_symbol = 1;
          } else if (selected_symbol == 1) {
            adjustment = adjustment + 1;
            selected_symbol == 2;
          } else {
            Serial.println("we should not be rendering Cb");
          }
          current_note[1] = SYMBOLS[selected_symbol];            
        }

        else if (curr_note_index == 35){ //B
          if (selected_symbol == 0){
            adjustment = adjustment + 1;
            selected_symbol = 1;
          } else if (selected_symbol == 1){
            adjustment = adjustment - 1;
            selected_symbol = 0;
          } else {
            Serial.println("we should not be rendering B#");
          }
          current_note[1] = SYMBOLS[selected_symbol];
        }

        ///////////////////////////////////

        else { //this is for all other notes
           //later on, we will add adjustment to note index
          if (selected_symbol == 2) {
            adjustment = adjustment - 2;
          } else {
            adjustment = adjustment + 1;
          }
          play_note(curr_note_index + adjustment);
          selected_symbol = (selected_symbol + 1) % 3;
          current_note[1] = SYMBOLS[selected_symbol];

          Serial.printf("Current note is %s, adjustment %d, current_note_index %d \n", current_note, adjustment, curr_note_index);
          
        }

      } else if (js == 3) { // down

        /////////////////////////////
        // edge cases //////////////
        ////////////////////////////
        if (curr_note_index == 0){ //edge case for Cb
          if (selected_symbol == 2){
            adjustment = adjustment - 1;
            selected_symbol = 1;
          } else if (selected_symbol == 1) {
            adjustment = adjustment + 1;
            selected_symbol == 2;
          } else {
            Serial.println("we should not be rendering Cb");
          }
          current_note[1] = SYMBOLS[selected_symbol];            
        }

        else if (curr_note_index == 35){
          if (selected_symbol == 0){
            adjustment = adjustment + 1;
            selected_symbol = 1;
          } else if (selected_symbol == 1){
            adjustment = adjustment - 1;
            selected_symbol = 0;
          } else {
            Serial.println("we should not be rendering B#");
          }
          current_note[1] = SYMBOLS[selected_symbol];
        }

        ///////////////////////////////////


        else {
            if (selected_symbol == 0) {
              adjustment = adjustment + 2;
            } else {
              adjustment = adjustment - 1;
            }
            play_note(curr_note_index + adjustment);
            selected_symbol = (selected_symbol + 2) % 3;
            current_note[1] = SYMBOLS[selected_symbol];
            Serial.printf("Current note is %s, adjustment %d, current_note_index %d \n", current_note, adjustment, curr_note_index);
          }
        }

    } else if (menu_state == 1) { // duration selection (joystick left and right)
      tft.fillRect(122, 40, 25, 15, TFT_BLACK); // clear duration
      if (js == 2) { // right
        selected_duration = (selected_duration + 1) % 5;
      } else if (js == 4) { // left
        selected_duration = (selected_duration + 4) % 5;
      }
    } else if (menu_state == 4) {
      tft.fillRect(119, 100, 27, 15, TFT_BLACK); // clear note
      if (js == 2) { // right
        current_measure = (current_measure + 1) % MEASURE_COUNT;
      } else if (js == 4) { // left
        current_measure = (current_measure + MEASURE_COUNT - 1) % MEASURE_COUNT;
      }
    }
  }
  update_in_game();
  if (in_turn) {
    if (bv == 1) {
      if (!is_locked && menu_state != 2) {
        is_locked = true;
      } else {
        is_locked = false;
      }
      // state changes
      if (menu_state == 0) {
        play_note(curr_note_index);
      }
      if (menu_state == 2) { // add a note
        int curr_x = 2 + 26.5 * (note_state % 4);
        int curr_y = 29 + 25 * (int(note_state / 4));
        tft.drawTriangle(curr_x, curr_y, curr_x, curr_y + 4, curr_x + 3, curr_y + 2, TFT_BLACK); // clear grid cursor
        tft.fillCircle(135, 30 + 20 * menu_state, 1, TFT_BLACK); // clear right side input cursor
  
        int temp_note_state = note_state;
        Serial.printf("Added is %d", note_state + pow(2, selected_duration));
        if ((note_state + pow(2, selected_duration) >= 16) || (note_state >= 16)) {
          note_state = 16;  // to update grid cursor position for next note
          menu_state = 3;
        } else {
          note_state += pow(2, selected_duration);
          is_locked = false;
          menu_state = 0;
    
          //adding the note to the notes array
          if (current_note[0] == 'R') {
            curr_note_index = 36;
          } else {
            //curr_note_index = curr_note_index + adjustment;
          }
    
          //curr_notes_array[temp_note_state] = curr_note_index;
          curr_notes_array[temp_note_state] = curr_note_index + adjustment; // Needs to be like this for playback
          temp_note_state = temp_note_state + 1;
  
          int i;
          for (i = 0; i < pow(2, selected_duration); i = i + 1) {
            curr_notes_array[temp_note_state] = 37;
            temp_note_state = temp_note_state + 1;
          }
        }
      } else if (menu_state == 3) {
        submit_measure();
        tft.fillCircle(135, 30 + 20 * menu_state, 1, TFT_BLACK); // clear right side input cursor
        note_state = 0;
        current_measure += 1;
        menu_state = 0;
        display_in_game();
      }
      update_in_game();
    } else if (bv == 2) { // go to game menu screen
      tft.fillScreen(TFT_BLACK);
      state = 5;
      menu_state = 0;
      is_locked = false;
      display_game_menu();
    }
  } else {
    if (millis() - time_since_last_ping > PING_INTERVAL) {
      fetch_game_state(6); // TODO: hardcoded 6
      Serial.println("time since last ping updated");
    }
  }
}

////////////////////// game-menu //////////////////////

void process_game_menu(int bv, int js) {
  if (js == 1) { // up
    menu_state = (menu_state + 4) % 5;
    update_game_menu();
  } else if (js == 3) { // down
    menu_state = (menu_state + 1) % 5;
    update_game_menu();
  }
  if (bv) {
    update_game_menu();
    if (menu_state == 0) { // resume game
      state = 4;
      display_in_game();
    } else if (menu_state == 1) {
      if (playing_song) stop_sound();
      playing_song = ! playing_song;
    } else if (menu_state == 2) {
      if (playing_measure) stop_sound();
      playing_measure = ! playing_measure;
    } else if (menu_state == 3) {
      sound_on = !sound_on;
      playing_song = false;
      playing_measure = false;
      stop_sound();
      update_game_menu();
    } else if (menu_state == 4) { // leave game
      reset_game();
      back_to_landing();
    }
  }
}
