void create_game_http() {
  char body[100];
  sprintf(body, "type=create&username=%s&key=%d&tempo=%d", USERNAME, selected_key, TEMPO_SPEEDS[selected_tempo]);
  Serial.println(body);
  make_post_request(SERVER, START_GAME_ADDRESS, body, response, false);  

  strcpy(game_code, strtok(response, "&"));
  game_id = atoi(strtok(NULL, "&"));

  Serial.printf("created game with game_id %d, game_code %s\n", game_id, game_code);
}

bool join_game_http() {
  int offset = 0;
  char body[100];
  sprintf(body, "type=join&username=%s&game_code=%d%d%d", USERNAME, game_code_input[0], game_code_input[1], game_code_input[2]);
  make_post_request(SERVER, START_GAME_ADDRESS, body, response, false);

  char code = strtok(response, "&")[0];
  if (code == '3') {
    game_id = atoi(strtok(NULL, "&"));
    room_num[0] = '\0';
    sprintf(room_num, "%d%d%d", game_code_input[0], game_code_input[1], game_code_input[2]);
    Serial.printf("joined game %s\n", room_num);
    return true;
  } else {
    // TODO: 0: no room found, 1: room limit reached, 2: game in progress or ended
    Serial.printf("failed to join room for error code %c\n", code);
    return false;
  }
}

void start_game_http() {
  int offset = 0;
  char body[50];
  sprintf(body, "type=start&game_id=%d", game_id);
  make_post_request(SERVER, START_GAME_ADDRESS, body, response, false);
}

//void make_get_request(char* server, char* address, char* params, char* output, bool https) {
//  // server: e.g. "http://608dev-2.net", address: e.g. /sandbox/sc/jkluong/ex05/wiki_interfacer.py
//  // params: e.g. "user=jkluong&lat=47.00002&lon=-79.38383", output: where the response is put into
//  int offset = 0;
//  request[0] = '\0';
//  offset += sprintf(request, "GET %s?%s HTTP/1.1\r\n", address, params);
//  offset += sprintf(request + offset, "Host: %s\r\n", server);
//  offset += sprintf(request + offset, "\r\n");
//  
//  Serial.printf("GET request:\n%s", request);
//  if (https) {
//    do_https_request(server, request, output, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
//  } else {
//    do_http_request(server, request, output, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
//  }
//
//  Serial.println("-----------");
//  Serial.println(output);
//  Serial.println("-----------");
//}
