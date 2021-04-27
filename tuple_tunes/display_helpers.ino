/////////////////
// Convenience //
/////////////////

uint8_t CYAN[] = {18, 243, 255};
uint8_t DARK_CYAN[] = {50, 162, 168};

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
  fade_in_text(" Tuple\n Tunes", 0, 13, CYAN, 1200, 3);
  
  // Menu options
  fade_in_text("Start Game", 90, 75, DARK_CYAN, 700, 1);
  fade_in_text(" Join Game", 90, 92, DARK_CYAN, 700, 1);
  fade_in_text("   Gallery", 90, 109, DARK_CYAN, 700, 1);
}

void update_landing(int selection) {
  tft.fillRect(78, 75, 11, 45, TFT_BLACK);
  tft.drawTriangle(80, 77 + 17 * selection, 
                   80, 81 + 17 * selection, 
                   83, 79 + 17 * selection, TFT_WHITE);
}

////////////////
// Start Game //
////////////////

void display_start_game() {
  tft.fillScreen(TFT_BLACK);
  fade_in_text(" Start\n Game", 0, 13, CYAN, 1200, 2);

  // Menu options
  fade_in_text("  Key: C", 86, 25, DARK_CYAN, 200, 1);
  fade_in_text("Tempo: Fast", 86, 42, DARK_CYAN, 200, 1);
  fade_in_text("    Start", 86, 59, DARK_CYAN, 200, 1);
  fade_in_text("     Back", 86, 105, DARK_CYAN, 200, 1);
}

void update_start_game() {
  
}

///////////////
// Join Game //
///////////////

void display_join_game() {
  tft.fillScreen(TFT_BLACK);
  fade_in_text(" Join\n Game", 0, 13, CYAN, 1200, 2);

  draw_text("Room:", 100, 25, DARK_CYAN, 1);
  fade_in_text("___", 103, 42, DARK_CYAN, 200, 1);
  fade_in_text("   Change", 86, 59, DARK_CYAN, 200, 1);
  fade_in_text("     Join", 86, 76, DARK_CYAN, 200, 1);
  fade_in_text("     Back", 86, 105, DARK_CYAN, 200, 1);
}

void update_join_game() {
  
}

/////////////
// Gallery //
/////////////
void display_gallery() {
  tft.fillScreen(TFT_BLACK);
  fade_in_text(" Gallery", 0, 13, CYAN, 1200, 2);
   
}

void update_gallery() {
  
}

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

  for (int i = 0; i < 5; i++) {
    tft.drawRect(0, 25 * i + 19, 106, 25 * i + 20, TFT_WHITE);
  }
  
  tft.drawLine(26.5, 19, 26.5, 120, TFT_WHITE);
  tft.drawLine(53, 19, 53, 120, TFT_WHITE);
  tft.drawLine(79.5, 19, 79.5, 120, TFT_WHITE);

  // Play notes
  tft.setCursor(8, 30, 1);
  tft.printf("C#");
  
  // Right side info
  tft.setCursor(128, 20, 2);
  tft.println("C#"); // selected note
  tft.setCursor(126, 45, 1);
  tft.println("1/4"); // note duration
  tft.drawTriangle(117,45,112,47.5,117,50, TFT_WHITE);
  tft.drawTriangle(153,45,158,47,153,50, TFT_WHITE);

  for (int i = 0; i < 3; i++) { // ellipses
    tft.fillCircle(4 * i + 146, 119, 1, TFT_WHITE);
  }
}

void update_in_game() {
  
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
  tft.printf(" Room#:%d\r\n", room_num);
  tft.setCursor(88, 50, 1);
  tft.printf(" Key:%s\r\n", song_key);
  tft.setCursor(88, 75, 1);
  tft.printf(" Tempo:%s\r\n", tempo);
  tft.setCursor(88, 100, 1);
  tft.printf(" #Players:%d", player_count);

  // draw triangle cursor for selection
  if (menu_state == 0) {
    tft.drawTriangle(2,25,3.5,27.5,2,30,TFT_WHITE);
  } else if (menu_state == 1) {
    tft.drawTriangle(2,50,3.5,52.5,2,55,TFT_WHITE);
  } else if (menu_state == 2) {
    tft.drawTriangle(2,75,3.5,77.5,2,80,TFT_WHITE);
  } else if (menu_state == 3) {
    tft.drawTriangle(2,100,3.5,102.5,2,105,TFT_WHITE);
  }
}

void update_game_menu() {
  
}
