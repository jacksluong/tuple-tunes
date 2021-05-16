///////////////////////
// Pre-game requests //
///////////////////////

/*
   Makes a POST request to ask the server for the code and ID of a new game.
*/
void create_game_http() {

  char body[100];
  sprintf(body, "type=create&username=%s&key=%d&tempo=%d", USERNAME, selected_key, selected_tempo); //instead of TEMPO_SPEEDS[selected_tempo]
  Serial.println(body);
  make_post_request(SERVER, PRE_GAME_ADDRESS, body, response, false);

  strcpy(game_code, strtok(response, "&"));
  room_num[0] = '\0';
  strcat(room_num, game_code);

  if (game_code == "-1") {
    Serial.printf("invalid post, please post username, selected key, and tempo");
  } else {
    game_id = atoi(strtok(NULL, "&"));
    Serial.printf("created game with game_id %d, game_code %s\n", game_id, game_code);
  }
}

/*
   Makes a POST request to join a game with the specified game code
   (from join-game page) and returns whether or not it was successful.
   Saves appropriate data (game_id, room_num) if successful.
*/
bool join_game_http() {
  int offset = 0;
  char body[100];
  sprintf(body, "type=join&username=%s&game_code=%d%d%d", USERNAME, game_code_input[0], game_code_input[1], game_code_input[2]);
  make_post_request(SERVER, PRE_GAME_ADDRESS, body, response, false);

  char code = strtok(response, "&")[0];
  if (code == '3') {
    game_id = atoi(strtok(NULL, "&"));
    selected_key = atoi(strtok(NULL, "&"));
    selected_tempo = atoi(strtok(NULL, "&"));

    if ((selected_key <= 5 && selected_key % 2 != 0) || (selected_key > 5 && selected_key % 2 == 0)) {
      is_flat_key = true;
    } else {
      is_flat_key = false;
    }
    
    room_num[0] = '\0';
    sprintf(room_num, "%d%d%d", game_code_input[0], game_code_input[1], game_code_input[2]);
    Serial.printf("joined game %s\n", room_num);
    Serial.printf("selected_key: %d, selected_tempo: %d, is_flat_key: %d \n", selected_key, selected_tempo, is_flat_key);
    
    return true;
  } else {
    // TODO: 0: no room found, 1: room limit reached, 2: game in progress or ended
    Serial.printf("failed to join room for error code %c\n", code);
    return false;
  }
}

/*
   Makes a POST request to start the current game. Only accessible
   by the host who created the game.
*/
bool start_game_http() {
  int offset = 0;
  char body[50];
  sprintf(body, "type=start&game_id=%d", game_id);
  make_post_request(SERVER, PRE_GAME_ADDRESS, body, response, false);

  char code = strtok(response, "&")[0];

  if (code == '1') {
    Serial.printf("Starting game id %d \n", game_id);
    game_state = 2;
    return true;
  } else if (code == '0' || code == '-') {
    Serial.printf("Game not found, invalid game_id: %d \n", game_id);
  } else if (code == '2') {
    Serial.println("Game has already started");
  } else if (code == '3') {
    Serial.println("Game has already ended");
  } else {
    Serial.println("something went wrong");
  }
  return false;
}

/*
 * Makes a GET request to get current game status.
 * Used to show all players in waiting room
 */
void get_game_status() {
  int offset = 0;
  char query[50];
  sprintf(query, "game_id=%d", game_id);
  make_get_request(SERVER, PRE_GAME_ADDRESS, query, response, false);

  char code = strtok(response, "&")[0];
  if (code == '1') { //game in waiting room, response ”1&{num_players}&{player_names}”
    num_players = atoi(strtok(NULL, "&"));
    strcpy(player_list, strtok(NULL, "&"));
    game_state = 1;
  } else if (code == '2') {
    Serial.printf("Game id %d, game code %c is in progress", game_id, code);
    game_state = 2;
  } else if (code == '3') {
    Serial.printf("Game id %d, game code %c has ended", game_id, code);
    game_state = 3;
  } else { //TODO: process how to parse other types of statements
    Serial.printf("Something went wrong; game_id: %d, response code: %c \n", game_id, code);
  }
}

//////////////////////
// In-game requests //
//////////////////////

bool fetch_game_state(int game_id) {
  char query[100]; //for body
  sprintf(query, "username=%s&game_id=%d&measure=%d", USERNAME, game_id, current_measure);
  Serial.println("making fetch");
  make_get_request(SERVER, IN_GAME_ADDRESS, query, response, false);

  Serial.println("response:");
  Serial.println(response);

  // format of response: "{in_turn}&{current_measure}&{song}"
  char* p = strtok(response, "&"); // name of current user's turn

  if (p[0] == '-') return false; // game ended
  
  Serial.printf("p is %s\n", p);
  Serial.printf("comparison: %d\n", strcmp(p, USERNAME));
  in_turn = !strcmp(p, USERNAME); // if matched, strcmp returns 0, which is equivalent to false

  Serial.print("in_turn: ");
  Serial.println(in_turn);

  // Turns on LED based on in turn: red for in turn, green for off turn.
  if (in_turn) {
    if (state == 4) {
      menu_state = 0;
      is_locked = false;
    }
    set_led_color(0, 255, 0);
  }

  p = strtok(NULL, "&"); // index of next measure to be submitted
  Serial.printf("current measure: {%s}\n", p);
  if (/*atoi(p) > current_measure*/true) { // we are now a measure behind
    p = strtok(NULL, "&"); // all measures string

    char* separator;
    char* measure;
    while ((separator = strchr(p, ' '))) {
      measure = p;
      p = separator + 1;
      separator[0] = '\0';
      Serial.printf("measure is %s\n", measure);

      note_state = 0;
      for (char* note = strtok(measure, ","); note; note = strtok(NULL, ",")) {
        uint8_t n = atoi(note);
        measures[current_measure][note_state] = n;
        Serial.printf("note %s turns into num %d (saved as %d)\n", note, atoi(note), measures[current_measure][note_state]);
        note_state++;
      }
      
      if (note_state == 16) {
        if (selected_measure == current_measure) selected_measure++;
        current_measure++;
        note_state = 0;
      }
    }

    Serial.println("finished parsing fetched data, measures is now:");
    for (int i = 0; i < MEASURE_COUNT; i++) {
      for (int j = 0; j < 16; j++)
        Serial.printf("%d ", measures[i][j]);
      Serial.println();
    }

    if (state == 4) display_in_game();
  } else {
    Serial.println("no updates needed");
  }
  time_since_last_ping = millis();

  return true;
}

/*
   Submits the current measure to the current game on the server via a
   POST request and fills any empty inputs with rests.
*/
void submit_measure() {
  char string_of_notes[50];
  uint8_t offset = 0;

  for (int i = 0; i < note_state; i++) {
    offset += sprintf(string_of_notes + offset, "%d ", measures[current_measure][i]);
  }
  for (int i = note_state; i < 16; i++) {
    offset += sprintf(string_of_notes + offset, "36 ");
  }

  char query[100]; //for body
  sprintf(query, "type=new_measure&username=%s&game_id=%d&measure=%s", USERNAME, game_id, string_of_notes);
  make_post_request(SERVER, IN_GAME_ADDRESS, query, response, false);
}

/*
   Tells the server we're still connected via a POST request.
*/
void ping() {
  char query[100]; //for body
  sprintf(query, "type=ping&username=%s&game_id=%d", USERNAME, game_id);
  Serial.println("Ping:");
  make_post_request(SERVER, IN_GAME_ADDRESS, query, response, false);

  time_since_last_ping = millis();
}

void leave_game() {
  char query[100]; //for body
  sprintf(query, "type=leave&username=%s&game_id=%d", USERNAME, game_id);
  Serial.println("Leaving game");
  make_post_request(SERVER, IN_GAME_ADDRESS, query, response, false);
}

/*
 * gallery request for random song
 */

void gallery_request() {
  char query[50] = "gallery=True";
  make_get_request(SERVER, IN_GAME_ADDRESS, query, response, false);
  
}
