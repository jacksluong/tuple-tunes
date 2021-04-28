/////////////////
// Convenience //
/////////////////

// Colors
uint8_t CYAN[] = {18, 243, 255};
uint8_t DARK_CYAN[] = {50, 162, 168};

// For functions
uint16_t fade_threshold = millis();

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

/*
 * A non-blocking function that animates a flashing cursor.
 */
void animate_cursor() {
  
}

/////////////
// Landing //
/////////////

void display_landing() {
  tft.fillScreen(TFT_BLACK);
  fade_in_text(" Tuple\n Tunes", 0, 13, CYAN, 1200, 3);
  
  // Menu options
  fade_in_text("Start Game", 90, 75, DARK_CYAN, 700, 1);
  fade_in_text(" Join Game", 90, 92, DARK_CYAN, 700, 1);
  fade_in_text("   Gallery", 90, 109, DARK_CYAN, 700, 1);

  update_landing();
}

void update_landing() {
  tft.fillRect(78, 75, 11, 45, TFT_BLACK);
  tft.drawTriangle(80, 76 + 17 * menu_state, 
                   80, 80 + 17 * menu_state, 
                   83, 78 + 17 * menu_state, TFT_WHITE);
}

////////////////
// Start Game //
////////////////

void display_start_game() {
  tft.fillScreen(TFT_BLACK);
  selected_tempo = 1;
  fade_in_text(" Start\n Game", 0, 13, CYAN, 1200, 2);

//  char key_text[20];
//  strcpy (key_text, "  Key: ");
//  strcat(key_text, key_labels[selected_key]);
//  char tempo_text[20];
//  strcpy(tempo_text, "Tempo: ");
//  strcat(tempo_text, tempo_labels[selected_tempo]);

  // Menu options
  fade_in_text("  Key: C", 86, 47, DARK_CYAN, 200, 1);
  fade_in_text("Tempo: Mid", 86, 64, DARK_CYAN, 200, 1);
  fade_in_text("   Start", 86, 81, DARK_CYAN, 200, 1);
  fade_in_text("    Back", 86, 109, DARK_CYAN, 200, 1);

  update_start_game();
}

void update_start_game() {
  tft.fillRect(75, 45, 5, 120, TFT_BLACK); // clear arrow
  tft.fillRect(80, 45, 80, 30, TFT_BLACK); // clear inputs
  
  char key_text[20] = "\0";
  sprintf(key_text, "  Key: %s", key_labels[selected_key]);
//  strcpy(key_text, "  Key: ");
//  strcat(key_text, key_labels[key_state]);
//  strcat(key_text, "\0");
  char tempo_text[20] = "\0";
  sprintf(tempo_text, "Tempo: %s", tempo_labels[selected_tempo]);
//  strcpy(tempo_text, "Tempo: ");
//  strcat(tempo_text, tempo_labels[tempo_state]);
//  strcat(key_text, "\0");

  // Menu options
  draw_text(key_text, 86, 47, DARK_CYAN, 1);
  draw_text(tempo_text, 86, 64, DARK_CYAN, 1);
  draw_text("   Start", 86, 81, DARK_CYAN, 1);
  draw_text("    Back", 86, 109, DARK_CYAN, 1);
  
  if (menu_state < 3) {
    tft.drawTriangle(76, 48 + 17 * menu_state, 
                     76, 52 + 17 * menu_state, 
                     79, 50 + 17 * menu_state, TFT_WHITE);
  } else {
    tft.drawTriangle(76, 110, 
                     76, 114, 
                     79, 112, TFT_WHITE); 
  }
}

///////////////
// Join Game //
///////////////

void display_join_game() {
  tft.fillScreen(TFT_BLACK);
  fade_in_text(" Join\n Game", 0, 13, CYAN, 1200, 2);

  fade_in_text("Room:", 110, 47, DARK_CYAN, 200, 1);
  fade_in_text("___", 113, 64, DARK_CYAN, 200, 1);
  fade_in_text("     Join", 86, 81, DARK_CYAN, 200, 1);
  fade_in_text("     Back", 86, 107, DARK_CYAN, 200, 1);

  update_join_game();
}

void update_join_game() {
  tft.fillRect(85, 45, 5, 120, TFT_BLACK); // clear arrow
  tft.fillRect(80, 64, 80, 17, TFT_BLACK); // clear inputs
  
  char input[4] = "\0";
  for (int i = 0; i < 3; i++) {
    int8_t digit = game_code_input[i];
    sprintf(input + i, "%c", digit >= 0 ? (char) (digit + 48) : '_');
  }

  // Input
  draw_text(input, 113, 64, DARK_CYAN, 1);

  if (menu_state < 2) {
    tft.drawTriangle(86, 48 + 34 * menu_state, 
                     86, 52 + 34 * menu_state, 
                     89, 50 + 34 * menu_state, TFT_WHITE);
  } else {
    tft.drawTriangle(86, 110, 
                     86, 114, 
                     89, 112, TFT_WHITE); 
  } 
}

/////////////
// Gallery //
/////////////
void display_gallery() {
  tft.fillScreen(TFT_BLACK);
  fade_in_text(" Gallery", 0, 13, CYAN, 1200, 2);

  update_gallery();
}

void update_gallery() {
  
}
