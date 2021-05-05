void fetch_game_state() {
  char query[100]; //for body
  game_id = 6;
  sprintf(query, "username=%s&game_id=%d&measure=%d", USERNAME, game_id, current_measure);
  Serial.println("making fetch");
  make_get_request(SERVER, IN_GAME_ADDRESS, query, response, false);

  Serial.println("response:");
  Serial.println(response);

  // format of response: "{in_turn}&{current_measure}&{song}"
  char* p = strtok(response, "&"); // name of current user's turn

  Serial.printf("p is %s\n", p);
  Serial.printf("comparison: %d\n", strcmp(p, USERNAME));
  in_turn = !strcmp(p, USERNAME); // if matched, strcmp returns 0, which is equivalent to false

  Serial.print("in_turn: ");
  Serial.println(in_turn);
  
  p = strtok(NULL, "&"); // index of next measure to be submitted
  Serial.printf("current measure: {%s}\n", p);
  if (atoi(p) > current_measure) { // we are now a measure behind
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

      if (note_state == 16) current_measure++;
    }

    
    Serial.println("finished parsing fetched data, measures is now:");
    for (int i = 0; i <= current_measure; i++) {
      for (int j = 0; j < 16; j++)
        Serial.printf("%d ", measures[i][j]);
      Serial.println();
    }
  } else {
    Serial.println("no updates needed");
  }
  time_since_last_ping = millis();
}

void submit_measure() {
  char string_of_notes[50];
  uint8_t notes_offset = 0;
  
//  game_id = 6;
  for (int i = 0; i < note_state; i++) {
    notes_offset += sprintf(string_of_notes + notes_offset, "%d ", curr_notes_array[i]);
  }
  for (int i = note_state; i < 16; i++) {
    notes_offset += sprintf(string_of_notes + notes_offset, "36 ");
  }

  char query[200]; //for body
  sprintf(query, "type=new_measure&username=%s&game_id=%d&measure=%s", USERNAME, game_id, string_of_notes);
  make_post_request(SERVER, IN_GAME_ADDRESS, query, response, false);
}
