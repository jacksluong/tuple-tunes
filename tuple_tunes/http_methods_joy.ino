

void make_get_request(char* server, char* address, char* params, char* output, bool https) {
  // server: e.g. "http://608dev-2.net", address: e.g. /sandbox/sc/jkluong/ex05/wiki_interfacer.py
  // params: e.g. "user=jkluong&lat=47.00002&lon=-79.38383", output: where the response is put into
  int offset = 0;
  request[0] = '\0';
  offset += sprintf(request, "GET %s?%s HTTP/1.1\r\n", address, params);
  offset += sprintf(request + offset, "Host: %s\r\n", server);
  offset += sprintf(request + offset, "\r\n");
  
  Serial.printf("GET request:\n%s", request);
  if (https) {
    do_https_request(server, request, output, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  } else {
    do_http_request(server, request, output, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  }

  Serial.println("-----------");
  Serial.println(output);
  Serial.println("-----------");
}
