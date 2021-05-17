void draw_sound_toggle() {
  draw_text("Hold to", 5, 101, DARK_CYAN);
  char text[10];
  strcpy(text, sound_on ? "mute  " : "unmute");
  draw_text(text, 5, 111, DARK_CYAN);
  tft.setCursor(3, 109, 1);
}

/////////////
// Landing //
/////////////

void display_landing() {
  set_led_color(0, 0, 255);
  tft.fillScreen(TFT_BLACK);
  fade_in_text(" Tuple\n Tunes", 0, 13, CYAN, 1000, 3);
  
  // Menu options
  fade_in_text("Start Game", 90, 75, DARK_CYAN, 500, 1);
  fade_in_text(" Join Game", 90, 92, DARK_CYAN, 500, 1);
  fade_in_text("   Gallery", 90, 109, DARK_CYAN, 500, 1);

  sound_on = true;

  update_landing();
}

void update_landing() {
  tft.fillRect(78, 75, 11, 45, TFT_BLACK);
  set_cursor_pos(80, 76 + 17 * menu_index);
  draw_cursor();
  draw_sound_toggle();
}

////////////////
// Start Game //
////////////////

void display_start_game() {
  tft.fillScreen(TFT_BLACK);
  selected_tempo = 1;
  fade_in_text(" Start\n Game", 0, 13, CYAN, 1000, 2);

  // Menu options
  fade_in_text("  Key: C", 86, 47, DARK_CYAN, 200, 1);
  fade_in_text("Tempo: Mid", 86, 64, DARK_CYAN, 200, 1);
  fade_in_text("     Start", 86, 81, DARK_CYAN, 200, 1);
  fade_in_text("      Back", 86, 109, DARK_CYAN, 200, 1);

  draw_sound_toggle();
  update_start_game(1);
}

void update_start_game(int js) {
  if (js == 0) return;
  else if (!is_locked && (js == 1 || js == 3)) tft.fillRect(75, 45, 5, 120, TFT_BLACK); // clear cursor
  else if (is_locked && (js == 2 || js == 4)) tft.fillRect(80, 45, 80, 30, TFT_BLACK); // clear inputs
  else if (js > 4) draw_sound_toggle();
  
  char key_text[20] = "\0";
  sprintf(key_text, "  Key: %s", NOTES_FLAT[selected_key]);
  if ((selected_key <= 5 && selected_key % 2 != 0) || (selected_key > 5 && selected_key % 2 == 0)) {
    is_flat_key = true;
  } else {
    is_flat_key = false;
  }
  char tempo_text[20] = "\0";
  sprintf(tempo_text, "Tempo: %s", TEMPO_LABELS[selected_tempo]);

  // Menu options
  draw_text(key_text, 86, 47, DARK_CYAN, 1);
  draw_text(tempo_text, 86, 64, DARK_CYAN, 1);
  draw_text("     Start", 86, 81, DARK_CYAN, 1);
  draw_text("      Back", 86, 109, DARK_CYAN, 1);

  // Cursor
  if (menu_index < 3) {
    set_cursor_pos(76, 48 + 17 * menu_index);
  } else {
    set_cursor_pos(76, 110);
  }
  draw_cursor();
}

///////////////
// Join Game //
///////////////

void display_join_game() {
  tft.fillScreen(TFT_BLACK);
  fade_in_text(" Join\n Game", 0, 13, CYAN, 1000, 2);

  fade_in_text("Room:", 120, 47, DARK_CYAN, 200, 1);
  fade_in_text("___", 127, 64, DARK_CYAN, 200, 1);
  fade_in_text("      Join", 86, 81, DARK_CYAN, 200, 1);
  fade_in_text("      Back", 86, 109, DARK_CYAN, 200, 1);

  draw_sound_toggle();
  update_join_game(1);
}

void update_join_game(int js) {
  if (js == 0) return;
  else if (!is_locked && (js == 1 || js == 3)) tft.fillRect(99, 45, 5, 120, TFT_BLACK); // clear cursor
  else if (is_locked && (js == 1 || js == 3)) tft.fillRect(118, 64, 40, 17, TFT_BLACK); // clear inputs
  else if (js > 4) draw_sound_toggle();
  tft.fillRect(127, 73, 18, 4, TFT_BLACK); // clear dot
  
  char input[4] = "\0";
  for (int i = 0; i < 3; i++) {
    int8_t digit = game_code_input[i];
    sprintf(input + i, "%c", digit >= 0 ? (char) (digit + 48) : '_');
  }

  // Input
  draw_text(input, 127, 64, DARK_CYAN, 1);

  // Input cursor
  if (is_locked) tft.fillCircle(130 + 6 * input_cursor, 75, 1, TFT_WHITE);

  // Cursor
  if (menu_index < 2) {
    set_cursor_pos(100, 48 + 34 * menu_index);
  } else {
    set_cursor_pos(100, 110);
  }
  draw_cursor();
}

//////////////////
// Waiting Room //
//////////////////

void display_waiting_room() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(rgb_to_565(CYAN));

  fade_in_text("Waiting Room", 9, 13, CYAN, 600, 2);

  char temp[50];
  sprintf(temp, "Username: %s", USERNAME);
  fade_in_text(temp, 9, 39, DARK_CYAN, 200, 1);
  temp[0] = '\0';
  sprintf(temp, "Room #:   %s", room_num);
  fade_in_text(temp, 9, 51, DARK_CYAN, 200, 1);
  temp[0] = '\0';
  sprintf(temp, "Players:  %d", num_players);
  fade_in_text(temp, 9, 63, DARK_CYAN, 200, 1);
  strcpy(temp, is_host ? "You are the host!" : "Waiting for host");
  fade_in_text(temp, 9, 75, DARK_CYAN, 200, 1);
  
  if (is_host) {
    fade_in_text("Start", 15, 100, DARK_CYAN, 100, 1);
  }
  set_cursor_pos(6, 101);
}

void update_waiting_room() {
  char temp[20];
  tft.fillRect(66, 62, 30, 12, TFT_BLACK);
  temp[0] = '\0';
  sprintf(temp, "%d", num_players);
  draw_text(temp, 70, 63, DARK_CYAN, 1);
}

/////////////
// Gallery //
/////////////

void display_gallery() {
  tft.fillScreen(TFT_BLACK);
  fade_in_text(" Gallery", 0, 13, CYAN, 1000, 2);

  update_gallery(0);
}

void update_gallery(int js) {
  
}
