/////////////////
// Convenience //
/////////////////

// Colors
uint8_t CYAN[] = {18, 243, 255};
uint8_t DARK_CYAN[] = {50, 162, 168};

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
  fade_in_text("Hold to mute", 3, 109, DARK_CYAN, 200, 1);

  update_landing();
}

void update_landing() {
  tft.fillRect(78, 75, 11, 45, TFT_BLACK);
  set_cursor_pos(80, 76 + 17 * menu_state);
  draw_cursor();
  tft.fillRect(0, 100, 80, 20, TFT_BLACK);
  tft.setCursor(3, 109, 1);
  if (sound_on) {
    tft.println("Hold to mute");
  } else {
    tft.println("Hold to unmute");
  }
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

  if (sound_on) {
    fade_in_text("Hold to mute", 3, 109, DARK_CYAN, 200, 1);
  } else {
    fade_in_text("Hold to unmute", 3, 109, DARK_CYAN, 200, 1);
  }
  
  update_start_game(1);
}

void update_start_game(int js) {
  if (js == 0) return;
  else if (!is_locked && (js == 1 || js == 3)) tft.fillRect(75, 45, 5, 120, TFT_BLACK); // clear cursor
  else if (is_locked && (js == 2 || js == 4)) tft.fillRect(80, 45, 80, 30, TFT_BLACK); // clear inputs
  else if (js > 4) {
    // Mute
    tft.fillRect(0, 100, 80, 20, TFT_BLACK);
    tft.setCursor(3, 109, 1);
    if (sound_on) {
      tft.println("Hold to mute");
    } else {
      tft.println("Hold to unmute");
    }
  }
  
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
  if (menu_state < 3) {
    set_cursor_pos(76, 48 + 17 * menu_state);
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
  fade_in_text(" Join\n Game", 0, 13, CYAN, 1200, 2);

  fade_in_text("Room:", 120, 47, DARK_CYAN, 200, 1);
  fade_in_text("___", 127, 64, DARK_CYAN, 200, 1);
  fade_in_text("      Join", 86, 81, DARK_CYAN, 200, 1);
  fade_in_text("      Back", 86, 109, DARK_CYAN, 200, 1);

  if (sound_on) {
    fade_in_text("Hold to mute", 3, 109, DARK_CYAN, 200, 1);
  } else {
    fade_in_text("Hold to unmute", 3, 109, DARK_CYAN, 200, 1);
  }

  update_join_game(1);
}

void update_join_game(int js) {
  if (js == 0) return;
  else if (!is_locked && (js == 1 || js == 3)) tft.fillRect(99, 45, 5, 120, TFT_BLACK); // clear cursor
  else if (is_locked && (js == 1 || js == 3)) tft.fillRect(118, 64, 40, 17, TFT_BLACK); // clear inputs
  else if (js > 4) { 
    // Mute
    tft.fillRect(0, 100, 80, 20, TFT_BLACK);
    tft.setCursor(3, 109, 1);
    if (sound_on) {
      tft.println("Hold to mute");
    } else {
      tft.println("Hold to unmute");
    }
  }
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
  if (menu_state < 2) {
    set_cursor_pos(100, 48 + 34 * menu_state);
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
  
  tft.setCursor(0, 13, 2);
  tft.println("Waiting Room");
  tft.setCursor(8, 40, 1);
  tft.printf("Username: %s", USERNAME);
  tft.setCursor(8, 53, 1);
  tft.printf("Room #: %s", room_num);
  if (is_host) {
    tft.setCursor(8, 66, 1);
    tft.println("You are the host!");
    //    tft.setCursor(8, 79, 1);
    //    tft.printf("Click to start game.");
    tft.setCursor(8, 100, 1);
    tft.printf("Number of Players: %d", num_players);
    tft.setCursor(8, 115, 1);
    tft.println("Start");
    set_cursor_pos(0, 116);
    draw_cursor();
  } else {
    tft.setCursor(8, 66, 1);
    tft.println("Waiting for host to start");
    tft.setCursor(8, 100, 1);
    tft.printf("Number of Players: %d", num_players);
  }
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
