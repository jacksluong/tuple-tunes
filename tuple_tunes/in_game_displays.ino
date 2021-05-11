/////////////
// In-game //
/////////////

uint8_t GRAY[] = {70, 70, 70};
uint8_t DARK_GRAY[] = {30, 30, 30};

void display_in_game() {
  tft.fillRect(0, 0, 106, 128, TFT_BLACK);
  tft.fillRect(106, 0, 80, 128, rgb_to_565(DARK_GRAY));

  // Left side
  display_grid(current_measure);

  // Right side
  strcpy(current_note, (is_flat_key ? NOTES_FLAT : NOTES_SHARP)[curr_note_index % 12]);
  // update starting index for selected symbol based on the note from this key
  if (current_note[1] == '#') selected_symbol = 0;
  else if (current_note[1] == 'b') selected_symbol = 1;
  else if (current_note[1] == ' ') selected_symbol = 2;

  tft.setCursor(112, 60, 1); // add a note
  tft.println("Add Note");
  tft.setCursor(118, 80, 1); // submit measure
  tft.println("Submit");

  // TODO: below
  
  tft.drawTriangle(115,21,110,23,115,25, TFT_WHITE); // arrows for note selection
  tft.drawTriangle(151,21,156,23,151,25, TFT_WHITE);
  tft.drawTriangle(115,41,110,43,115,45, TFT_WHITE); // arrows for duration selection
  tft.drawTriangle(151,41,156,43,151,45, TFT_WHITE);
  tft.drawTriangle(115,101,110,103,115,105, TFT_WHITE); // arrows for measure selection
  tft.drawTriangle(151,101,156,103,151,105, TFT_WHITE);

  for (int i = 0; i < 3; i++) { // ellipses
    tft.fillCircle(4 * i + 146, 119, 1, TFT_WHITE);
  }
}

void update_in_game() {
  // Print current selected note
  tft.setCursor(132, 20, 1); 
  tft.println(current_note);

  // Print current selected note duration
  if (selected_duration == 0) tft.setCursor(122, 40, 1); // 1/16
  else if (selected_duration == 4) tft.setCursor(133, 40, 1); // 1
  else tft.setCursor(127, 40, 1);
  tft.println(NOTE_DURATIONS[selected_duration]);

  // Print measure_num
  tft.setCursor(current_measure < 10 ? 122 : 119, 100, 1);
  tft.printf("m. %d", selected_measure);
  
  // Grid cursor
  if (note_state < 16) {
    set_cursor_pos(2 + 26.5 * (note_state % 4), 29 + 25*(int(note_state/4)));
    draw_cursor();
  }
  
  // right side input cursor
  tft.fillCircle(135, 30 + 20 * menu_state, 1, TFT_WHITE);

  // Update selected note
  if (is_locked && menu_state == 0) draw_note(note_state, curr_note_index + adjustment);

  // LED light
  // TODO: put it somewhere better so it's only called when in turn changes
  if (in_turn) set_led_color(255, 0, 0);
  else set_led_color(0, 255, 0);
}

void display_grid(int measure_i) {
  tft.fillRect(4, 5, 13, 75, TFT_BLACK);
  tft.setCursor(5, 6, 1);
  tft.printf("<measure %d/%d>", current_measure + 1, MEASURE_COUNT);
  for (int i = 0; i < (measure_i == current_measure ? note_state : 16); i++)
    draw_note(i, measures[measure_i][i]);
  if (measure_i == current_measure) draw_note(note_state, curr_note_index + adjustment);
}

void draw_note(int note_i, int note_num) {
  tft.fillRect(3 + 25 * (note_i % 4), 22 + 25 * (note_i / 4), 19, 19, TFT_BLACK);
  tft.setCursor(8 + 25 * (note_i % 4), 27 + 25 * (note_i / 4), 1);
  if (note_num == 37) tft.println("~");
  else if (note_num == 36) tft.println("R");
  else tft.println((is_flat_key ? NOTES_FLAT : NOTES_SHARP)[note_num % 12]);
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
  tft.setCursor(4, 20, 1);
  tft.println(" Resume\r\n");
  tft.setCursor(4, 40, 1);
  if (!playing_song) {
    tft.println(" Play Song\r\n");
  } else {
    tft.println(" Stop Song\r\n");
  }
  tft.setCursor(4, 60, 1);
  if (!playing_measure) {
    tft.println(" Play Measure\r\n");
  } else {
    tft.println(" Stop Measure\r\n");
  }
  tft.setCursor(4, 80, 1);
  if (sound_on) {
    tft.println(" Mute\r\n");
  } else {
    tft.println(" Unmute\r\n");
  }
  tft.setCursor(4, 100, 1);
  tft.println(" Leave Game\r\n");

  // right side static
  tft.setCursor(88, 6, 1);
  tft.println("Game Info\r\n");
  tft.setCursor(88, 20, 1);
  tft.printf(" Room#:%s\r\n", room_num);
  tft.setCursor(88, 40, 1);
  tft.printf(" Key:%s\r\n", NOTES_FLAT[selected_key]);
  tft.setCursor(88, 60, 1);
  tft.printf(" Tempo:%s\r\n", TEMPO_LABELS[selected_tempo]);
  tft.setCursor(88, 80, 1);
  tft.printf(" #Players:%d\r\n", num_players);
  tft.setCursor(88, 100, 1);
  if (sound_on) {
    tft.println(" Sound On");
  } else {
    tft.println(" Sound off");
  }

  if (in_turn) set_led_color(255, 0, 0);
  else set_led_color(0, 255, 0);
  update_game_menu();
}

void update_game_menu() {
  tft.fillRect(2, 20, 4, 100, TFT_BLACK);
  set_cursor_pos(2, 21 + 20 * (menu_state % 5));
  draw_cursor();

  tft.fillRect(6, 78, 50, 22, TFT_BLACK);
  tft.setCursor(4, 80, 1);
  if (sound_on) {
    tft.println(" Mute\r\n");
  } else {
    tft.println(" Unmute\r\n");
  }

  tft.fillRect(88, 98, 70, 22, TFT_BLACK);
  tft.setCursor(88, 100, 1);
  if (sound_on) {
    tft.println(" Sound On");
  } else {
    tft.println(" Sound Off");
  }
}
