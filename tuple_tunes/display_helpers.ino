uint8_t cursor_pos[2] = {0};

/*
 * Convenience function to convert an int[3] array to uint16_t.
 */
uint16_t rgb_to_565(uint8_t* rgb) {
  return tft.color565(rgb[0], rgb[1], rgb[2]);
}

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
