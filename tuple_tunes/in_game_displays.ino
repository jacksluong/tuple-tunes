/////////////
// In-game //
/////////////

uint8_t GRAY[] = {70, 70, 70};

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
  
  tft.drawLine(26.5, 19, 26.5, 118, rgb_to_565(GRAY));
  tft.drawLine(53, 19, 53, 118, rgb_to_565(GRAY));
  tft.drawLine(79.5, 19, 79.5, 118, rgb_to_565(GRAY));
  
  // Right side info
  current_note[0] = '\0'; // get first starting note in this key
  if (is_flat_key) {
    strcat(current_note, NOTES_FLAT[selected_note]); 
  } else {
    strcat(current_note, NOTES_SHARP[selected_note]); 
  }
  if (current_note[1] == '#') { // update starting index for selected symbol based on the note from this key
    selected_symbol = 0;
  } else if (current_note[1] == 'b') {
    selected_symbol = 1;
  } else if (current_note[1] == ' ') {
    selected_symbol = 2;
  }
  tft.setCursor(125, 40, 1);
  tft.println(NOTE_DURATIONS[selected_duration]); // note duration
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

//  for (int i = 0; i < strlen(measures[current_measure]); i++) {
//    tft.setCursor(8 + 26.5 * (i % 4), 28 + 25*(int(i/4)), 1);
//    tft.printf(measures[current_measure][i]);
//  }

  for (int i = 0; i < note_state; i++) {
      tft.setCursor(8 + 26.5 * (i % 4), 28 + 25*(int(i/4)), 1);
      tft.println(curr_notes_array[i]);
      
  }
  
  update_in_game();
}

void update_in_game() {
  tft.setCursor(132, 20, 1); 
  tft.println(current_note); // print current selected note
  tft.setCursor(125, 40, 1);
  tft.println(NOTE_DURATIONS[selected_duration]); // print current selected note duration

  // grid cursor
  set_cursor_pos(2 + 26.5 * (note_state % 4), 29 + 25*(int(note_state/4)));
  draw_cursor();

  // right side input cursor
  tft.fillCircle(135, 30 + 20 * menu_state, 1, TFT_WHITE);

  // include the next note on the grid
  if (is_locked && menu_state == 0) {
    tft.setCursor(8 + 26.5 * (note_state % 4), 28 + 25*(int(note_state/4)), 1);
    tft.printf(current_note);
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
  tft.setCursor(4, 20, 1);
  tft.println(" Resume\r\n");
  tft.setCursor(4, 40, 1);
  if (!playing_song) {
    tft.println(" Play Song\r\n");
  } else {
    tft.println(" Stop Song\r\n");
  }
  tft.setCursor(4, 60, 1);
  if (!playing_measure) {
    tft.println(" Play Measure\r\n");
  } else {
    tft.println(" Stop Measure\r\n");
  }
  tft.setCursor(4, 80, 1);
  if (sound_on) {
    tft.println(" Mute\r\n");
  } else {
    tft.println(" Unmute\r\n");
  }
  tft.setCursor(4, 100, 1);
  tft.println(" Leave Game\r\n");

  // right side static
  tft.setCursor(88, 6, 1);
  tft.println("Game Info\r\n");
  tft.setCursor(88, 20, 1);
  tft.printf(" Room#:%s\r\n", room_num);
  tft.setCursor(88, 40, 1);
  tft.printf(" Key:%s\r\n", NOTES_FLAT[selected_key]);
  tft.setCursor(88, 60, 1);
  tft.printf(" Tempo:%s\r\n", TEMPO_LABELS[selected_tempo]);
  tft.setCursor(88, 80, 1);
  tft.printf(" #Players:%d\r\n", player_count);
  tft.setCursor(88, 100, 1);
  if (sound_on) {
    tft.println(" Sound On");
  } else {
    tft.println(" Sound off");
  }

  update_game_menu();
}

void update_game_menu() {
  tft.fillRect(2, 20, 4, 100, TFT_BLACK);
  set_cursor_pos(2, 21 + 20 * (menu_state % 5));
  draw_cursor();

  tft.fillRect(6, 78, 50, 22, TFT_BLACK);
  tft.setCursor(4, 80, 1);
  if (sound_on) {
    tft.println(" Mute\r\n");
  } else {
    tft.println(" Unmute\r\n");
  }

  tft.fillRect(88, 98, 70, 22, TFT_BLACK);
  tft.setCursor(88, 100, 1);
  if (sound_on) {
    tft.println(" Sound On");
  } else {
    tft.println(" Sound Off");
  }
}
