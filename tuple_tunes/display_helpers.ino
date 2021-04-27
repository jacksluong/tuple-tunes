void display_in_turn() {
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

void display_menu() {
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
