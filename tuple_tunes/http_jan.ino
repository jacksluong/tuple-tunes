void get_song_measures() {
  char body[200]; //for body
  sprintf(body, "username=%s&game_id=%d&measure=%d", USERNAME, game_id, current_measure);
  make_get_request(SERVER, IN_GAME_ADDRESS, body, response, false);
  int measure_index_http = 0;
  int num_measures_returned = 0;
  int note_index_http = 0;
  char* measure_strings[MEASURE_COUNT];
  char* current_player_returned;
  char* measure_response;
  char* temp_response = strchr(response, '&')+1;
  *(strchr(response, '&')) = '\0';
  if (response == "1" || response == "3") {
    if (response == "1") {
      measure_response = temp_response;
      num_measures_returned = MEASURE_COUNT;
    } else {
      char* temp = strtok(temp_response, "&");
      if (temp != NULL) {
        current_player_returned = temp;
        temp = strtok(NULL, "&");
        if (temp != NULL) {
          num_measures_returned = atoi(temp);
          temp = strtok(NULL, "&");
          if (temp != NULL) {
            measure_response = temp;
          }
        }
      }
    }
    
    char* measure_string = strtok(measure_response, " ");
    while (measure_string != NULL && measure_index_http < MEASURE_COUNT) {
      measure_strings[measure_index_http] = measure_string;
      measure_string = strtok(NULL, " ");
      measure_index_http = measure_index_http + 1;
    }
    measure_index_http = 0;
    
    while (measure_index_http < num_measures_returned) {
      char* note_string = strtok(measure_strings[measure_index_http], ",");
      while (note_string != NULL & note_index_http < 16) {
        measures[measure_index_http][note_index_http] = atoi(note_string);
        note_string = strtok(NULL, ",");
        note_index_http = note_index_http + 1;
      }
      measure_index_http = measure_index_http+1;
    }
  } 
  
  


//  bool still_parsing = true;

//  if (response[0] == "1") {
//    char* current_note = &(response[0]) + 2;
//    char* next_note = strchr(current_note, ',') + 1;
//    *(strchr(current_note, ',')) = '\0';
//    while (still_parsing) {
//      measures[measure_index_http][note_index_http] = atoi(current_note);
//      
//    }
//    
//    char* target_str = strchr(measures,',') + 2;
//    *(strchr(response_buffer, ',')) = '\0';
//    char* setting_str = strchr(target_str, ',') + 2;
//    *(strchr(target_str, ',')) = '\0';
//    *(strchr(setting_str, ')')) = '\0';
//  }
  
}

void submit_measure() {
  char string_of_notes[50];
  int notes_offset = 0;
  
  game_id = 6;
  for (int i = 0; i < note_state; i++) {
    notes_offset += sprintf(string_of_notes + notes_offset, "%d ", curr_notes_array[i]);
  }
  for (int i = note_state; i < 16; i++) {
    notes_offset += sprintf(string_of_notes + notes_offset, "36 ");
  }

  char body[200]; //for body
  sprintf(body, "type=new_measure&username=%s&game_id=%d&measure=%s", USERNAME, game_id, string_of_notes);
  make_post_request(SERVER, IN_GAME_ADDRESS, body, response, false);
}
