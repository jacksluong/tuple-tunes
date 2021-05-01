/////////////
// In-game //
/////////////

void display_in_game() {
  tft.fillScreen(TFT_BLACK);
  
  // Dividing line between left and right
  tft.drawRect(106, 0, 107, 128, TFT_WHITE);
  
  // Left side grid
  tft.setCursor(5, 6, 1);
  tft.println("<measure 4/4>");

  for (int i = 0; i < 4; i++) {
    tft.drawRect(0, 25 * i + 19, 107, 25, TFT_WHITE);
  }
  
  tft.drawLine(26.5, 19, 26.5, 118, TFT_WHITE);
  tft.drawLine(53, 19, 53, 118, TFT_WHITE);
  tft.drawLine(79.5, 19, 79.5, 118, TFT_WHITE);
  
  // Right side info
  curr_note[0] = '\0';
  tft.setCursor(133, 20, 1); // print current selected note
  if (is_flat_key) {
    strcat(curr_note, notes_flat[selected_note]); 
  } else {
    strcat(curr_note, notes_sharp[selected_note]); 
  }
  if (curr_note[1] == '#') {
    selected_sym = 0;
  } else if (curr_note[1] == 'b') {
    selected_sym = 1;
  } else if (curr_note[1] == ' ') {
    selected_sym = 2;
  }
  tft.println(curr_note);
  tft.setCursor(125, 40, 1);
  tft.println(notes_dur[selected_dur]); // note duration
  tft.drawTriangle(117,41,112,43,117,45, TFT_WHITE);
  tft.drawTriangle(151,41,156,43,151,45, TFT_WHITE);
  tft.setCursor(112, 60, 1); // add a note
  tft.println("Add Note");
  tft.setCursor(118, 80, 1); // submit measure
  tft.println("Submit");

  for (int i = 0; i < 3; i++) { // ellipses
    tft.fillCircle(4 * i + 146, 119, 1, TFT_WHITE);
  }

  tft.println(room_num); // not to keep there, it's just for purposes of seeing the game code
  
  
  update_in_game();
}

void update_in_game() {
  
  tft.setCursor(133, 20, 1); // print current selected note
  tft.println(curr_note);
  tft.setCursor(125, 40, 1);
  tft.println(notes_dur[selected_dur]); // note duration

  // grid cursor
  set_cursor_pos(2 + 26.5 * (note_state % 4), 29 + 25*(int(note_state/4)));
  draw_cursor();

  // input cursor
  tft.fillCircle(135, 30 + 20 * menu_state, 1, TFT_WHITE);

  // include the next note on the grid
  if (is_locked && menu_state == 0) {
    tft.setCursor(8 + 26.5 * (note_state % 4), 28 + 25*(int(note_state/4)), 1);
    tft.printf(curr_note);
  }
  
}

///////////////
// Game Menu //
///////////////

void display_game_menu() {
  tft.fillScreen(TFT_BLACK);
  
  // dividing line between left and right
  tft.drawLine(84, 0, 84, 128, TFT_WHITE);
  tft.drawLine(85, 0, 85, 128, TFT_WHITE);

  // left side actionables
  tft.setCursor(2, 6, 1);
  tft.println("Menu\r\n");
  tft.setCursor(4, 25, 1);
  tft.println(" Resume\r\n");
  tft.setCursor(4, 50, 1);
  tft.println(" Play Song\r\n");
  tft.setCursor(4, 75, 1);
  tft.println(" Play Measure\r\n");
  tft.setCursor(4, 100, 1);
  tft.println(" Leave Game\r\n");

  // right side static
  tft.setCursor(88, 6, 1);
  tft.println("Game Info\r\n");
  tft.setCursor(88, 25, 1);
  tft.printf(" Room#:%s\r\n", room_num);
  tft.setCursor(88, 50, 1);
  tft.printf(" Key:%s\r\n", notes_flat[selected_key]);
  tft.setCursor(88, 75, 1);
  tft.printf(" Tempo:%s\r\n", tempo_labels[selected_tempo]);
  tft.setCursor(88, 100, 1);
  tft.printf(" #Players:%d", player_count);

  update_game_menu();
}

void update_game_menu() {
  tft.fillRect(2, 25, 4, 100, TFT_BLACK);
  set_cursor_pos(2, 26 + 25 * (menu_state % 4));
  draw_cursor();
}
