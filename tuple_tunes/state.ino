void back_to_landing() {
  menu_state = 0;
  state = 0;
  display_landing();
  update_landing();
}

void update_state(int bv, int js) {
  if (state == 0) {             ////////////////////// landing //////////////////////
    if (js == 1) { // up
      menu_state = (menu_state + 2) % 3;
      update_landing();
    } else if (js == 3) { // down
      menu_state = (menu_state + 1) % 3;
      update_landing();
    }
      
    if (bv) {
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
    }
    
  } else if (state == 1) {      ////////////////////// start game //////////////////////
    if (millis() - last_played > 300) stop_sound();
    if (!is_locked && js == 1) { // up
      menu_state = (menu_state + 3) % 4;
    } else if (!is_locked && js == 3) { // down
      menu_state = (menu_state + 1) % 4;
    } else if (is_locked && js == 2) { // right
      if (menu_state == 0) {
        //selected key is index of the selected key, should not changed after locked
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
    
    if (bv) {
      if (menu_state == 0 || menu_state == 1) { // inputs
        is_locked = !is_locked;
        if (!is_locked) stop_sound();
        else if (menu_state == 0) play_note(selected_key);
        update_start_game(1);
      } else if (menu_state == 2) { // start
        for (int i = 0; i < 3; i++) room_num[i] = '0';
        menu_state = 0;
        curr_note_index = selected_key;
        state = 4;
        selected_note = selected_key;
        display_in_game();
      } else  { // back
        back_to_landing();
      }
    }
    
  } else if (state == 2) {      ////////////////////// join game //////////////////////
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

    if (bv) {
      if (menu_state == 0) { // input
        if (!is_locked) is_locked = true;
        else if (game_code_input[0] >= 0 && // three numbers specified
                 game_code_input[1] >= 0 &&
                 game_code_input[2] >= 0) is_locked = false;
        update_join_game(1);
      } else if (menu_state == 1) { // join
        room_num[0] = '\0';
        sprintf(room_num, "%d%d%d", game_code_input[0], game_code_input[1], game_code_input[2]);
        menu_state = 0;
        input_cursor = 0;
        state = 4;
        curr_note_index = selected_key;
        display_in_game();
      } else { // back
        back_to_landing();
      }
    }
  } else if (state == 3) {      ////////////////////// gallery //////////////////////
    
    } else if (state == 4) {      ////////////////////// in-game //////////////////////
    if (!is_locked && js) { // scrolling up and down the menu
      tft.fillCircle(135, 30 + 20 * menu_state, 1, TFT_BLACK);
      if (js == 1) { // up
        menu_state = (menu_state + 3) % 4;
      } else if (js == 3) { // down
        menu_state = (menu_state + 1) % 4;
      }
    } else if (is_locked && js) { // scrolling through note and duration selection
      if (menu_state == 0) { // note and sharp/flat/neutral selection
        tft.fillRect(133,20,20,15,TFT_BLACK); // clear note
        tft.fillRect(8 + 26.5 * (note_state % 4),28 + 25*(int(note_state/4)),15,15,TFT_BLACK); // clear grid cell
        if (js == 2) { // right
          selected_note = (selected_note + key_jumps[jump_index]) % 12;
          jump_index = (jump_index + 1) % 7;

          scale_index = (scale_index + 1) % 8;
          curr_note_index = curr_note_index + scale_steps[scale_index];

          if (curr_note_index > 35){
            curr_note_index = 35;
          }

          Serial.printf("Current note index updated %d \n", curr_note_index);
        
        } else if (js == 4) { // left
          jump_index = (jump_index + 6) % 7;
          selected_note = (selected_note + 12 - key_jumps[jump_index]) % 12;

          scale_index = (scale_index + 7) % 8; //same as subtracting 1
          curr_note_index = curr_note_index - scale_steps[scale_index];

          if (curr_note_index < 0){
            curr_note_index = 0;
          }

          Serial.printf("Current note index updated %d \n", curr_note_index);
        }

        //current note is what we will be displaying
        curr_note[0] = '\0';

        //this is our case for representing rests
        if(scale_index == 0){
          strcat(curr_note, "R");
          Serial.println("Current note is rest");
        } else if (is_flat_key) {
          strcat(curr_note, notes_flat[selected_note]);
          Serial.printf("Current note is %s", curr_note);
        } else {
          strcat(curr_note, notes_sharp[selected_note]); 
          Serial.printf("Current note is %s", curr_note);
        }

        
        if (js == 2 || js == 4) { // update current selected symbol if note has changed
          
          if (curr_note[1] == '#') {
            selected_sym = 2;
          } else if (curr_note[1] == 'b') {
            selected_sym = 0;
          } else if (curr_note[1] == ' ' || curr_note[1] == '\0') {
            selected_sym = 1;
          }
          
          adjustment = 0; //we need to reset adjustment each time
          
          
        }
        
        // changing sharp/flat/neutral
        if (js == 1) { // up
          //later on, we will add adjustment to note index
          if (selected_sym == 2){
            adjustment = adjustment - 2;
          } else {
            adjustment = adjustment + 1;
          }
          
          selected_sym = (selected_sym + 1) % 3;
          curr_note[1] = symbols[selected_sym];

          Serial.printf("Current note is %s, adjustment %d, curr_note_index %d \n", curr_note, adjustment, curr_note_index);

        } else if (js == 3) { // down

          if (selected_sym == 0){
            adjustment = adjustment + 2;
          } else {
            adjustment = adjustment - 1;
          }
          
          selected_sym = (selected_sym + 2) % 3;
          curr_note[1] = symbols[selected_sym]; 

          Serial.printf("Current note is %s, adjustment %d, curr_note_index %d \n", curr_note, adjustment, curr_note_index);
        }

        
      } else if (menu_state == 1) { // duration selection (joystick left and right)
        tft.fillRect(125,40,25,15,TFT_BLACK); // clear duration
        if (js == 2) { // right
          selected_dur = (selected_dur + 1) % 5;
        } else if (js == 4) { // left
          selected_dur = (selected_dur + 4) % 5;
        }
      }
    }
    update_in_game();
    
    if (bv) {
      if (bv == 1) {
        if (!is_locked) {
          is_locked = true;
        } else {
          is_locked = false;
        }
        // state changes
        if (menu_state == 2) { // add a note
          int curr_x = 2 + 26.5 * (note_state % 4);
          int curr_y = 29 + 25*(int(note_state/4));
          tft.drawTriangle(curr_x, curr_y, curr_x, curr_y + 4, curr_x + 3, curr_y + 2, TFT_BLACK); // clear grid cursor
          tft.fillCircle(135, 30 + 20 * menu_state, 1, TFT_BLACK); // clear right side input cursor

          int temp_note_state = note_state;
          
          if (note_state < 16) note_state += pow(2,selected_dur); // to update grid cursor position for next note
          is_locked = false;
          menu_state = 0;

          //adding the note to the notes array
          if (curr_note[0] == 'R'){
            curr_note_index = 36;
          } else {
            curr_note_index = curr_note_index + adjustment;
          }

          curr_notes_array[temp_note_state] = curr_note_index;
          temp_note_state = temp_note_state + 1;

          int i;

          for (i = 0; i < selected_dur; i = i + 1){
            curr_notes_array[temp_note_state] = 37;
            temp_note_state = temp_note_state + 1;
          }

          Serial.printf("Note index inserted %d", curr_note_index);
          //Serial.println(curr_notes_array);
        }
        update_in_game();
        
      } else { // go to game menu screen
        tft.fillScreen(TFT_BLACK);
        state = 5;
        menu_state = 0;
        is_locked = false;
        display_game_menu();
      }
    }
  } else if (state == 5) {      ////////////////////// game menu //////////////////////
    if (js == 1) { // up
      menu_state = (menu_state + 3) % 4;
      update_game_menu();
    } else if (js == 3) { // down
      menu_state = (menu_state + 1) % 4;
      update_game_menu();
    }
    if (bv) {
      if (menu_state == 0) { // resume game
        state = 4;
        display_in_game();
      } else if (menu_state == 1) {
        play_song_bool = true;
      } else if (menu_state == 2) {
        play_measure_bool = true;
      }
      else if (menu_state == 3) { // leave game
        back_to_landing();
      }
    }
  }

}
