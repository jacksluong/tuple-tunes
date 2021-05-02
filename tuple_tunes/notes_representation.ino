void get_next_note(){
    // js returns 0, 1U, 2R, 3D, 4L for idle, up, right, down, left (cardinal directions) respectively

}

 /*
//at this point, selected note = selected key
//note_index = selected key

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
          strcat(curr_note, 'R');
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
          original_symbol = 
          
          
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
          Serial.printf("Current note is %s, adjustment %d, curr_note_index %d \n", curr_note, adjustment, curr_note_index);

          
          
          int curr_x = 2 + 26.5 * (note_state % 4);
          int curr_y = 29 + 25*(int(note_state/4));
          tft.drawTriangle(curr_x, curr_y, curr_x, curr_y + 4, curr_x + 3, curr_y + 2, TFT_BLACK); // clear grid cursor
          tft.fillCircle(135, 30 + 20 * menu_state, 1, TFT_BLACK); // clear right side input cursor
          
          if (note_state < 16) note_state += pow(2,selected_dur); // to update grid cursor position for next note
          is_locked = false;
          menu_state = 0;
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

  */
