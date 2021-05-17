uint8_t cursor_pos[2] = {0};
uint8_t old_cursor_pos[2] = {0};

/*
 * Convenience function to convert an int[3] array to uint16_t.
 */
uint16_t rgb_to_565(uint8_t* rgb) {
  return tft.color565(rgb[0], rgb[1], rgb[2]);
}

void loading_page() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED);
  tft.setTextSize(2);
  tft.setCursor(15, 50);
  tft.println("Loading...");
}

/*
 * Convenience function to set the cursor pos.
 */
void set_cursor_pos(uint8_t x, uint8_t y) {
  cursor_pos[0] = x;
  cursor_pos[1] = y;
}

/*
 * Convenience function to clear the old cursor pos.
 */
void clear_old_cursor() {
  old_cursor_pos[0] = 0;
  old_cursor_pos[1] = 0;
}

/*
 * A non-blocking function that draws a static or flashing cursor.
 */
void draw_cursor() {
  // Clear old cursor if necessary
  uint8_t x = cursor_pos[0], y = cursor_pos[1], old_x = old_cursor_pos[0], old_y = old_cursor_pos[1];
  if (!(old_x == 0 && old_y == 0) && (x != old_x || y != old_y)) {
    if (state == 4) tft.drawLine(old_x, old_y, old_x + 9, old_y, TFT_BLACK);
    else tft.drawTriangle(old_x, old_y, old_x, old_y + 4, old_x + 3, old_y + 2, TFT_BLACK);
  }

  if (x == 0 && y == 0) return; // signal for no cursor
  
  // Choose color
  uint16_t color; 
  if (is_locked) {
    uint8_t val = (int) (255 * abs(cos((millis() - last_button_click) * PI / 600.0)));
    color = tft.color565(val, val, val);
  }
  else color = TFT_WHITE;

  // Draw new cursor/underline
  if (state == 4 && selected_measure == current_measure && note_state < 16) tft.drawLine(x, y, x + 9, y, color);
  else tft.drawTriangle(x, y, x, y + 4, x + 3, y + 2, color);

  // Save position of new cursor
  old_cursor_pos[0] = x;
  old_cursor_pos[1] = y;
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
  // clear area
  tft.fillRect(x, y, strlen(text) * 6 * font_size, 8 * font_size, tft.readPixel(x, y - 1));
  
  tft.setCursor(x, y);
  tft.setTextSize(font_size);
  tft.setTextColor(tft.color565(rgb[0], rgb[1], rgb[2]));
  tft.println(text);
}

/*
 * Converts selected note (int) to its appropriate string representation in the grid.
 */
char* note_to_grid_string(int note_n) {
  char s[3];
  if (note_n == 37) strcpy(s, "~");
  else if (note_n == 36) strcpy(s, "R");
  else strcpy(s, (is_flat_key ? NOTES_FLAT : NOTES_SHARP)[note_n % 12]);
  return s;
}
