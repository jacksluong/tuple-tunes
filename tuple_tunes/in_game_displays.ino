/////////////
// In-game //
/////////////

uint8_t old_menu_state = 0; // 0: not previously displayed, 1: was previously locked, 2: was previously unlocked

void display_in_game() {
  tft.fillRect(0, 0, 106, 128, TFT_BLACK);
  tft.fillRect(106, 0, 80, 128, rgb_to_565(DARK_GRAY));
  
  // Grid
  tft.fillRect(0, 0, 106, 128, TFT_BLACK);
  tft.fillRect(106, 0, 80, 128, rgb_to_565(DARK_GRAY));

  // Left side
  for (int i = 0; i < 4; i++) tft.drawRect(0, 25 * i + 19, 106, 25, TFT_WHITE);
  display_measure(selected_measure);

  // Right side
  menu_index = in_turn ? 0 : 4;
  
  if (in_turn) {
    draw_text("Add Note", 110, 60, CYAN, 1);
    draw_text("Submit", 115, 80, CYAN, 1);
  }

  for (int i = 0; i < 3; i++) { // ellipses
    tft.fillCircle(4 * i + 146, 119, 1, TFT_WHITE);
  }

  old_menu_state = 0;
  update_in_game(0, false);
}

void update_in_game(int js) {
  // Clearing
  if (is_locked) {
    if (menu_index == 0) tft.fillRect(125, 15, 25, 15, rgb_to_565(DARK_GRAY)); // selected note
    else if (menu_index == 1) tft.fillRect(122, 40, 25, 15, rgb_to_565(DARK_GRAY)); // duration
    else if (menu_index == 4) tft.fillRect(119, 100, 30, 15, rgb_to_565(DARK_GRAY)); // measure
    tft.drawTriangle(115, 21 + 20 * menu_index, 
                     110, 23 + 20 * menu_index,
                     115, 25 + 20 * menu_index, TFT_WHITE);
    tft.drawTriangle(151, 21 + 20 * menu_index, 
                     156, 23 + 20 * menu_index,
                     151, 25 + 20 * menu_index, TFT_WHITE); // arrows for note selection
  } else {
    tft.drawTriangle(115,21,110,23,115,25, rgb_to_565(DARK_GRAY)); // arrows for note selection
    tft.drawTriangle(151,21,156,23,151,25, rgb_to_565(DARK_GRAY));
    tft.drawTriangle(115,41,110,43,115,45, rgb_to_565(DARK_GRAY)); // arrows for duration selection
    tft.drawTriangle(151,41,156,43,151,45, rgb_to_565(DARK_GRAY));
    tft.drawTriangle(115,101,110,103,115,105, rgb_to_565(DARK_GRAY)); // arrows for measure selection
    tft.drawTriangle(151,101,156,103,151,105, rgb_to_565(DARK_GRAY));
    if (menu_index == 0 || menu_index == 1 || menu_index == 4) {
      tft.drawTriangle(115, 21 + 20 * menu_index, 
                       110, 23 + 20 * menu_index,
                       115, 25 + 20 * menu_index, rgb_to_565(GRAY));
      tft.drawTriangle(151, 21 + 20 * menu_index, 
                       156, 23 + 20 * menu_index,
                       151, 25 + 20 * menu_index, rgb_to_565(GRAY)); // arrows for current menu state
    }
  }
  if (!is_locked) // menu cursor
    for (int i = 0; i < 5; i++) tft.fillCircle(134, 30 + 20 * i, 1, rgb_to_565(DARK_GRAY)); 
//  if (note_added || selected_measure != current_measure) { // grid cursor
//    int current_x = 8 + 25 * (note_state % 4);
//    int current_y = 38 + 25 * (int(note_state / 4));
//    tft.drawLine(current_x, current_y, current_x + 10, current_y, TFT_BLACK);
//  }

  // Drawing
  uint8_t menu_state = (in_turn && selected_measure == current_measure) ? 2 : 1;
  if (menu_state != old_menu_state) {
    uint8_t* color = menu_state == 2 ? CYAN : GRAY;
    
    // scrollable menu options
    char text[10];
    sprintf(text, "m. %d", selected_measure + 1);
    draw_text(text, current_measure < 10 ? 122 : 117, 8, CYAN);
    sprintf(text, "%s", current_note);
    draw_text(text, 132, 24, color);
    sprintf(text, "%s", NOTE_DURATIONS[selected_duration]);
    draw_text(text, selected_duration == 0 ? 122 : (selected_duration == 4 ? 131 : 125), 40, color);

    // static menu options
    draw_text("Add Note", 110, 56, color);
    draw_text("Del Note", 110, 72, color);
    draw_text("Submit", 115, 88, color);
    draw_text("Menu", 120, 104, DARK_CYAN);
  }

  // update measure
//  if (menu_index == 1 && js) draw_note(note_state, measures[current_measure][note_state];
//  else display_measure(selected_measure, menu_index == 0 && (js == 2 || js == 4));
  display_measure(selected_measure);

  // grid cursor
  if (selected_measure == current_measure && note_state < 16) {
    set_cursor_pos(8 + 25 * (note_state % 4), 38 + 25 * (note_state / 4));
  } else set_cursor_pos(0, 0);
  draw_cursor();
  
  
  /*
  if (in_turn) {
    tft.setCursor(132, 20, 1); 
    tft.println(current_note); // print current selected note
    if (selected_duration == 0) { // 1/16
      tft.setCursor(122, 40, 1); 
    } else if (selected_duration == 4) { // 1
      tft.setCursor(131, 40, 1); 
    } else {
      tft.setCursor(125, 40, 1);
    }
    tft.println(NOTE_DURATIONS[selected_duration]); // print current selected note duration
  }
  if (current_measure < 10) { // scroll through measures
    tft.setCursor(122, 100, 1);
  } else {
    tft.setCursor(117, 100, 1); 
  }
  tft.printf("m. %d", selected_measure + 1);
  
  // Left cursor
  if (note_state < 16) {
    set_cursor_pos(8 + 25 * (note_state % 4), 38 + 25 * (note_state / 4));
    draw_cursor();
  }
  
  // Right cursor
  tft.fillCircle(134, 30 + 20 * menu_index, 1, TFT_WHITE);

  // Update selected note
  if (is_locked && menu_index == 0) draw_note(note_state, measures[current_measure][note_state]);
  if (note_added) display_measure(current_measure);
  */

  Serial.printf("Current/selected measure: %d/%d, note_state (c_n_i): %d (%d)\n", current_measure, selected_measure, note_state, note_num);
}

void display_measure(int measure_i) {
  tft.fillRect(4, 5, 75, 13, TFT_BLACK);
  char text[20] = {0};
  sprintf(text, "<measure %d/%d>", selected_measure + 1, MEASURE_COUNT);
  draw_text(text, 5, 6, GRAY, 1);
  for (int i = 0; i < (measure_i == current_measure ? min(note_state + 1, 16) : 16); i++)
    draw_note(i, measures[measure_i][i]);
}

void draw_note(int note_i, int note_n) {
  tft.fillRect(3 + 25 * (note_i % 4), 22 + 25 * (note_i / 4), 19, 21, TFT_BLACK);
  uint8_t* color = (selected_measure == current_measure && note_i == note_state) ? CYAN : DARK_CYAN;
  draw_text(note_to_grid_string(note_n), 10 + 25 * (note_i % 4), 28 + 25 * (note_i / 4), color, 1);
}

///////////////
// Game Menu //
///////////////

void display_game_menu() {
  tft.fillScreen(TFT_BLACK);
  
  // dividing line between left and right
  tft.drawLine(84, 0, 84, 128, TFT_WHITE);
  tft.drawLine(85, 0, 85, 128, TFT_WHITE);

  // left side (menu options)
  draw_text("Menu", 2, 6, CYAN, 1);
  draw_text(" Resume", 4, 20, DARK_CYAN, 1);
  char text[15] = {0};
  strcpy(text, playing_song ? " Stop Song" : " Play Song");
  draw_text(text, 4, 40, DARK_CYAN, 1);
  strcpy(text, playing_measure ? " Stop Measure" : " Play Measure");
  draw_text(text, 4, 60, DARK_CYAN, 1);
  strcpy(text, sound_on ? " Mute" : " Unmute");
  draw_text(text, 4, 80, DARK_CYAN, 1);
  draw_text(" Leave Game", 4, 100, DARK_CYAN, 1);

  // right side (static info)
  draw_text("Game Info", 88, 6, CYAN, 1);
  sprintf(text, " Room: %s", room_num);
  draw_text(text, 88, 20, DARK_CYAN, 1);
  sprintf(text, " Key: %s", NOTES_FLAT[selected_key]);
  draw_text(text, 88, 40, DARK_CYAN, 1);
  sprintf(text, " Tempo: %s", TEMPO_LABELS[selected_tempo]);
  draw_text(text, 88, 60, DARK_CYAN, 1);
  sprintf(text, " Players: %d", num_players);
  draw_text(text, 88, 80, DARK_CYAN, 1);
  strcpy(text, sound_on ? " Sound On" : " Sound Off");
  draw_text(text, 88, 100, DARK_CYAN, 1);

  clear_old_cursor();
  update_game_menu();
}

void update_game_menu() {
  tft.fillRect(2, 20, 4, 100, TFT_BLACK);
  set_cursor_pos(2, 21 + 20 * (menu_index % 5));
  draw_cursor();

  char text[15] = {0};
  strcpy(text, sound_on ? " Mute" : " Unmute");
  draw_text(text, 4, 80, DARK_CYAN, 1);
  strcpy(text, sound_on ? " Sound On" : " Sound Off");
  draw_text(text, 88, 100, DARK_CYAN, 1);
}

//////////////
// End Game //
//////////////

void display_end_game() {
  tft.fillScreen(TFT_BLACK);
  draw_text("Game ended", 8, 13, GRAY, 1);
  draw_text("Return to landing page", 8, 28, GRAY, 1);
}
