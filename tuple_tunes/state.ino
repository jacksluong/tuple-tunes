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
    
    if (bv) {
      if (menu_state == 0 || menu_state == 1) { // inputs
        is_locked = !is_locked;
        if (!is_locked) stop_sound();
        else if (menu_state == 0) play_note(selected_key);
        update_start_game(1);
      } else if (menu_state == 2) { // start
        for (int i = 0; i < 3; i++) room_num[i] = '0';
        menu_state = 0;
        state = 4;
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
        state = 4;
        display_in_game();
      } else { // back
        back_to_landing();
      }
    }
  } else if (state == 3) {      ////////////////////// gallery //////////////////////
    
  } else if (state == 4) {      ////////////////////// in-game //////////////////////
    if (bv == 2) back_to_landing(); // for debugging/demo only
  } else if (state == 5) {      ////////////////////// game menu //////////////////////
    display_game_menu();
  }

}
