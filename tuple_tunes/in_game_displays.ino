/////////////
// In-game //
/////////////

uint8_t old_menu_state = 0; // 0: not previously displayed, 1: was previously locked, 2: was previously unlocked

void display_in_game() {
  tft.fillRect(0, 0, 106, 128, TFT_BLACK);
  tft.fillRect(106, 0, 80, 128, rgb_to_565(DARK_GRAY));

  // Left side
  for (int i = 0; i < 4; i++) tft.drawRect(0, 25 * i + 19, 106, 25, TFT_WHITE);

  // Right side
  menu_index = 0;
  old_menu_state = 0;

  update_in_game(1, 0, false);
}

void update_in_game(int bv, int js, bool note_num_changed) {
  // Clearing
  if (is_locked) {
    tft.fillRect(118, 9 + 16 * menu_index, 31, 8, rgb_to_565(DARK_GRAY));
    tft.drawTriangle(115, 11 + 16 * menu_index,
                     110, 13 + 16 * menu_index,
                     115, 15 + 16 * menu_index, TFT_WHITE);
    tft.drawTriangle(151, 11 + 16 * menu_index,
                     156, 13 + 16 * menu_index,
                     151, 15 + 16 * menu_index, TFT_WHITE); // arrows for note selection
  } else {
    for (int i = 0; i < 3; i++) {
      tft.drawTriangle(115, 11 + 16 * i,
                       110, 13 + 16 * i,
                       115, 15 + 16 * i, rgb_to_565(i == menu_index ? GRAY : DARK_GRAY));
      tft.drawTriangle(151, 11 + 16 * i,
                       156, 13 + 16 * i,
                       151, 15 + 16 * i, rgb_to_565(i == menu_index ? GRAY : DARK_GRAY)); // arrows for note selection
    }
  }
  if (!is_locked) { // menu cursor
    for (int i = 0; i < 7; i++) tft.fillCircle(134, 20 + 16 * i, 1, rgb_to_565(DARK_GRAY));
    tft.fillCircle(134, 20 + 16 * menu_index, 1, TFT_WHITE);
  }
  
  // Drawing
  uint8_t menu_state = (in_turn && selected_measure == current_measure) ? 2 : 1;
  uint8_t* color = menu_state == 2 ? CYAN : DARK_CYAN;
  uint16_t dark_gray = rgb_to_565(DARK_GRAY);
  char text[10];
  if (menu_state != old_menu_state) {
    // scrollable menu options
    sprintf(text, "m. %d", selected_measure + 1);
    draw_text(text, current_measure < 10 ? 122 : 117, 10, CYAN, 1, dark_gray);
    sprintf(text, "%s", current_note);
    tft.fillRect(128, 25, 14, 10, rgb_to_565(DARK_GRAY));
    draw_text(text, 129 + (text[1] == ' ' ? 3 : 0), 26, color, 1, dark_gray);
    sprintf(text, "%s", NOTE_DURATIONS[selected_duration]);
    draw_text(text, selected_duration == 0 ? 122 : (selected_duration == 4 ? 132 : 125), 42, color, 1, dark_gray);
  
    // static menu options
    draw_text("Add Note", 110, 58, color, 1, dark_gray);
    draw_text("Del Note", 110, 74, color, 1, dark_gray);
    draw_text("Submit", 115, 90, color, 1, dark_gray);
    draw_text("Menu", 121, 106, CYAN, 1, dark_gray);

    old_menu_state = menu_state;
  } else {
    if (menu_index == 0) {
      sprintf(text, "m. %d", selected_measure + 1);
      draw_text(text, current_measure < 10 ? 122 : 117, 10, CYAN, 1, dark_gray);
    } else if (menu_index == 1) {
      sprintf(text, "%s", current_note);
      tft.fillRect(128, 25, 14, 10, rgb_to_565(DARK_GRAY));
      draw_text(text, 129 + (text[1] == ' ' ? 3 : 0), 26, color, 1, dark_gray);
    } else if (menu_index == 2) {
      sprintf(text, "%s", NOTE_DURATIONS[selected_duration]);
      draw_text(text, selected_duration == 0 ? 122 : (selected_duration == 4 ? 131 : 125), 42, color, 1, dark_gray);
    }
  }

  // update measure
  if (note_num_changed) {
    draw_note(note_state, measures[current_measure][note_state]);
  }
  else if (bv) display_measure(selected_measure);

  Serial.println("DEBUG: (in-game received input)");
  Serial.printf("Current/selected measure: %d/%d, note_state (c_n_i): %d (%d)\n", current_measure, selected_measure, note_state, note_num);
  Serial.printf("menu_index: %d (state %d js %d), current and current_note: %d | %s\n", menu_index, state, js, measures[selected_measure][note_state], current_note);
}

void display_measure(int measure_i) {
  tft.fillRect(4, 5, 75, 13, TFT_BLACK);
  tft.setTextColor(rgb_to_565(GRAY));
  tft.setCursor(5, 6, 1);
  tft.printf("<measure %d/%d>", selected_measure + 1, MEASURE_COUNT);
  for (int i = 0; i < (measure_i == current_measure ? min(note_state + 1, 16) : 16); i++)
    draw_note(i, measures[measure_i][i]);
  if (measure_i == current_measure) for (int i = note_state + 1; i < 16; i++)
    tft.fillRect(9 + 25 * (i % 4), 28 + 25 * (i / 4), 14, 10, TFT_BLACK);

  // grid cursor
  for (int i = 0; i < 3; i++) tft.fillRect(9, 37 + 25 * i, 86, 3, TFT_BLACK);
  if (selected_measure == current_measure && note_state < 16) {
    set_cursor_pos(10 + 25 * (note_state % 4), 38 + 25 * (note_state / 4));
    draw_cursor();
  }
}

void draw_note(int note_i, int note_n) {
  tft.fillRect(3 + 25 * (note_i % 4), 22 + 25 * (note_i / 4), 19, 21, TFT_BLACK); // clear any grid cursor
  tft.fillRect(9 + 25 * (note_i % 4), 28 + 25 * (note_i / 4), 14, 10, TFT_BLACK); // clear any note here
  char text[3] = {0};
  if (note_n == 37) strcpy(text, "~ ");
  else if (note_n == 36) strcpy(text, "R ");
  else strcpy(text, (is_flat_key ? NOTES_FLAT : NOTES_SHARP)[note_n % 12]);
  draw_text(text, 10 + 25 * (note_i % 4) + (text[1] == ' ' ? 3 : 0), 28 + 25 * (note_i / 4),
            (selected_measure == current_measure && note_i == note_state) ? CYAN : DARK_CYAN);
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
   draw_text(" Resume", 4, 20, CYAN, 1);
   char text[15] = {0};
   strcpy(text, playing_song ? " Stop Song" : " Play Song");
   draw_text(text, 4, 40, CYAN, 1);
   strcpy(text, playing_measure ? " Stop Measure" : " Play Measure");
   draw_text(text, 4, 60, CYAN, 1);
   strcpy(text, sound_on ? " Mute" : " Unmute");
   draw_text(text, 4, 80, CYAN, 1);
   draw_text(" Leave Game", 4, 100, CYAN, 1);

   // right side (static info)
   draw_text("Game Info", 88, 6, CYAN, 1);
   sprintf(text, " Room: %s", room_num);
   draw_text(text, 88, 20, DARK_CYAN, 1);
   sprintf(text, " Key: %s", NOTES_FLAT[selected_key]);
   draw_text(text, 88, 40, DARK_CYAN, 1);
   sprintf(text, " Tempo: %s", TEMPO_LABELS[selected_tempo]);
   text[11] = '\0'; // overflow
   draw_text(text, 88, 60, DARK_CYAN, 1);
   sprintf(text, " Players: %d", num_players);
   draw_text(text, 88, 80, DARK_CYAN, 1);
   strcpy(text, sound_on ? " Sound On" : " Sound Off");
   draw_text(text, 88, 100, DARK_CYAN, 1);

   update_game_menu();
}

void update_game_menu() {
  tft.fillRect(2, 20, 4, 100, TFT_BLACK);
  set_cursor_pos(2, 21 + 20 * (menu_index % 5));
  draw_cursor();

  char text[10];
  strcpy(text, sound_on ? " Mute" : " Unmute");
  draw_text(text, 4, 80, CYAN, 1);
  strcpy(text, sound_on ? " Sound On" : " Sound Off");
  draw_text(text, 88, 100, DARK_CYAN, 1);
}

//////////////
// End Game //
//////////////

void display_end_game() {
  tft.fillScreen(TFT_BLACK);

  display_in_game();
  tft.fillRect(106, 0, 80, 128, rgb_to_565(DARK_GRAY));
  draw_text("Play", 123, 10, CYAN, 1);
  draw_text("Song", 123, 20, CYAN, 1);
  draw_text("Play", 123, 40, CYAN, 1);
  draw_text("Measure", 113, 50, CYAN, 1);
  draw_text("New", 127, 70, CYAN, 1);
  draw_text("Game", 123, 80, CYAN, 1);

  if (current_measure < 10) { // scroll through measures
    tft.setCursor(122, 100, 1);
  } else {
    tft.setCursor(117, 100, 1); 
  }
  tft.printf("m. %d", selected_measure + 1);

  for (int i = 0; i < 3; i++) { // ellipses
    tft.fillCircle(4 * i + 146, 119, 1, TFT_WHITE);
  }
  
  update_end_game();
}

void update_end_game() {
  // Clearing
  if (is_locked) {
    if (menu_index == 3) tft.fillRect(119, 110, 20, 10, rgb_to_565(DARK_GRAY)); // measure
    if (menu_index == 3) {
      tft.drawTriangle(115, 11 + 30 * menu_index, 
                       110, 13 + 30 * menu_index,
                       115, 15 + 30 * menu_index, TFT_WHITE);
      tft.drawTriangle(151, 11 + 30 * menu_index, 
                       156, 13 + 30 * menu_index,
                       151, 15 + 30 * menu_index, TFT_WHITE); // arrows for measure selection
    }
  } else {
    tft.drawTriangle(115,121,110,103,115,105, rgb_to_565(DARK_GRAY)); // arrows for measure selection
    tft.drawTriangle(151,121,156,103,151,105, rgb_to_565(DARK_GRAY));
//    if (menu_index == 3) {
    tft.drawTriangle(115, 11 + 30 * menu_index, 
                     110, 13 + 30 * menu_index,
                     115, 15 + 30 * menu_index, rgb_to_565(menu_index == 3 ? GRAY : DARK_GRAY));
    tft.drawTriangle(151, 11 + 30 * menu_index, 
                     156, 13 + 30 * menu_index,
                     151, 15 + 30 * menu_index, rgb_to_565(menu_index == 3 ? GRAY : DARK_GRAY)); // arrows for current menu state
//    }
  }


  // Menu cursor
  if (menu_index == 3) {
    tft.fillCircle(134, 50 + 30 * (menu_index - 1), 1, TFT_WHITE);
  } else {
    tft.fillCircle(134, 30 + 30 * menu_index, 1, TFT_WHITE);
  }
}
