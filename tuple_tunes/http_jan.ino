void get_song_measures() {
//  char body[200]; //for body
//  sprintf(body, "username=%s&game_id=%d&measure=%d", USERNAME, game_id, current_measure);
//  make_get_request(SERVER, IN_GAME_ADDRESS, body, response, false)
//  bool still_parsing = true;
//  int measure_index_http = 0;
//  int note_index_http = 0;
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
