/////////////////
// Convenience //
/////////////////

// Colors
uint8_t CYAN[] = {18, 243, 255};
uint8_t DARK_CYAN[] = {50, 162, 168};


uint8_t cursor_pos[2] = {0};

/*
 * Convenience function to set the cursor pos.
 */
void set_cursor_pos(uint8_t x, uint8_t y) {
  cursor_pos[0] = x;
  cursor_pos[1] = y;
}

/*
 * A non-blocking function that draws a static or flashing cursor.
 */
void draw_cursor() {
  uint16_t color; 
  if (is_locked) {
    uint8_t val = (int) (255 * abs(cos((millis() - last_button_click) * PI / 600.0)));
    color = tft.color565(val, val, val);
  }
  else color = TFT_WHITE;
  uint8_t x = cursor_pos[0], y = cursor_pos[1];
  tft.drawTriangle(x, y, x, y + 4, x + 3, y + 2, color);
}

/*
 * A blocking function that will animate text with a fade-in
 * with the given specs (color, location, duration, size).
 */
void fade_in_text(char* text, uint8_t x, uint8_t y, uint8_t* rgb, int duration, uint8_t font_size = 1) {
  // draw: function that takes in an alpha value; duration: millis
  uint32_t last = millis();
  uint16_t interval = (int) (duration / 20);
  for (float alpha = 0.0; alpha < 1; alpha += 0.05) {
    tft.setCursor(x, y);
    tft.setTextSize(font_size);
    tft.setTextColor(tft.color565((int) (rgb[0] * alpha),
                                  (int) (rgb[1] * alpha),
                                  (int) (rgb[2] * alpha)));
    tft.println(text);
    while (millis() - last < interval);
    last = millis();
  }
}

/*
 * Draws text with the given specs.
 */
void draw_text(char* text, uint8_t x, uint8_t y, uint8_t* rgb, uint8_t font_size = 1) {
  tft.setCursor(x, y);
  tft.setTextSize(font_size);
  tft.setTextColor(tft.color565(rgb[0], rgb[1], rgb[2]));
  tft.println(text);
}

/////////////
// Landing //
/////////////

void display_landing() {
  tft.fillScreen(TFT_BLACK);
  fade_in_text(" Tuple\n Tunes", 0, 13, CYAN, 1000, 3);
  
  // Menu options
  fade_in_text("Start Game", 90, 75, DARK_CYAN, 500, 1);
  fade_in_text(" Join Game", 90, 92, DARK_CYAN, 500, 1);
  fade_in_text("   Gallery", 90, 109, DARK_CYAN, 500, 1);

  update_landing();
}

void update_landing() {
  tft.fillRect(78, 75, 11, 45, TFT_BLACK);
  set_cursor_pos(80, 76 + 17 * menu_state);
  draw_cursor();
}

////////////////
// Start Game //
////////////////

void display_start_game() {
  tft.fillScreen(TFT_BLACK);
  selected_tempo = 1;
  fade_in_text(" Start\n Game", 0, 13, CYAN, 1000, 2);

//  char key_text[20];
//  strcpy (key_text, "  Key: ");
//  strcat(key_text, notes_flat[selected_key]);
//  char tempo_text[20];
//  strcpy(tempo_text, "Tempo: ");
//  strcat(tempo_text, tempo_labels[selected_tempo]);

  // Menu options
  fade_in_text("  Key: C", 86, 47, DARK_CYAN, 200, 1);
  fade_in_text("Tempo: Mid", 86, 64, DARK_CYAN, 200, 1);
  fade_in_text("     Start", 86, 81, DARK_CYAN, 200, 1);
  fade_in_text("      Back", 86, 109, DARK_CYAN, 200, 1);

  update_start_game(1);
}

void update_start_game(int js) {
  if (js == 0) return;
  else if (!is_locked && (js == 1 || js == 3)) tft.fillRect(75, 45, 5, 120, TFT_BLACK); // clear cursor
  else if (is_locked && (js == 2 || js == 4)) tft.fillRect(80, 45, 80, 30, TFT_BLACK); // clear inputs
  
  char key_text[20] = "\0";
  sprintf(key_text, "  Key: %s", notes_flat[selected_key]);
//  strcpy(key_text, "  Key: ");
//  strcat(key_text, notes_flat[key_state]);
//  strcat(key_text, "\0");
  char tempo_text[20] = "\0";
  sprintf(tempo_text, "Tempo: %s", tempo_labels[selected_tempo]);
//  strcpy(tempo_text, "Tempo: ");
//  strcat(tempo_text, tempo_labels[tempo_state]);
//  strcat(key_text, "\0");

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

  update_join_game(1);
}

void update_join_game(int js) {
  if (js == 0) return;
  else if (!is_locked && (js == 1 || js == 3)) tft.fillRect(99, 45, 5, 120, TFT_BLACK); // clear cursor
  else if (is_locked && (js == 1 || js == 3)) tft.fillRect(118, 64, 40, 17, TFT_BLACK); // clear inputs
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

/////////////
// Gallery //
/////////////
void display_gallery() {
  tft.fillScreen(TFT_BLACK);
  fade_in_text(" Gallery", 0, 13, CYAN, 1000, 2);

  update_gallery();
}

void update_gallery() {
  
}
