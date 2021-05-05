/*
  https communication requires certificates to be known ahead of time so each entity will trust one another.
  This is a cert for google's generic servers below, allowing us to connect over https with their resources
*/
const char* CA_CERT = \
                      "-----BEGIN CERTIFICATE-----\n" \
                      "MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
                      "A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
                      "Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
                      "MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
                      "A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
                      "hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
                      "v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
                      "eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
                      "tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
                      "C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
                      "zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
                      "mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
                      "V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
                      "bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
                      "3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
                      "J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
                      "291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
                      "ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
                      "AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
                      "TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
                      "-----END CERTIFICATE-----\n";
const char API_KEY[] = "AIzaSyCwyynsePu7xijUYTOgR7NdVqxH2FAG9DQ"; // for googleapis.com

/////////////
// Globals //
/////////////

const int RESPONSE_TIMEOUT = 6000;
const int PING_INTERVAL = 6000;
uint32_t time_since_last_ping = millis();

//////////////////
// My functions //
//////////////////

/*
 * Performs a GET request with a given address and query.
 */
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

/*
 * Performs a POST request with a given address and body.
 */
void make_post_request(char* server, char* address, char* body, char* output, bool https) { // test variadic function
  int len = strlen(body);
  request[0] = '\0';
  uint16_t offset = 0;
  offset += sprintf(request + offset, "POST %s  HTTP/1.1\r\n", address);
  offset += sprintf(request + offset, "Host: %s\r\n", server);
  offset += sprintf(request + offset, "Content-Type: application/x-www-form-urlencoded\r\n"); // test json?
  offset += sprintf(request + offset, "cache-control: no-cache\r\n");
  offset += sprintf(request + offset, "Content-Length: %d\r\n\r\n", len);
  offset += sprintf(request + offset, "%s\r\n", body);

  Serial.printf("POST request:\n%s", request);
  if (https) {
    do_https_request(server, request, output, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  } else {
    do_http_request(server, request, output, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  }

  Serial.println("-----------");
  Serial.println(output);
  Serial.println("-----------");
}

/////////////////////////////////////////////////////////////////////////////
// Functions given in past templates to do the dirty work of HTTP requests //
/////////////////////////////////////////////////////////////////////////////

/*----------------------------------
  char_append Function:
  Arguments:
     char* buff: pointer to character array which we will append a
     char c:
     uint16_t buff_size: size of buffer buff
  Return value:
     boolean: True if character appended, False if not appended (indicating buffer full)
*/
uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

/*----------------------------------
   do_http_request Function:
   Arguments:
      char* host: null-terminated char-array containing host to connect to
      char* request: null-terminated char-arry containing properly formatted HTTP request
      char* response: char-array used as output for function to contain response
      uint16_t response_size: size of response buffer (in bytes)
      uint16_t response_timeout: duration we'll wait (in ms) for a response from server
      uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
   Return value:
      void (none)
*/
void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  WiFiClient client;
  if (client.connect(host, 80)) {
    if (serial) Serial.print(request);
    client.print(request);
    memset(response, 0, response_size); 
    uint32_t count = millis();
    while (client.connected()) { 
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { 
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) {
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}

/*----------------------------------
   do_https_request Function. Similar to do_http_request, but supports https requests
   Arguments:
      char* host: null-terminated char-array containing host to connect to
      char* request: null-terminated char-arry containing properly formatted HTTP request
      char* response: char-array used as output for function to contain response
      uint16_t response_size: size of response buffer (in bytes)
      uint16_t response_timeout: duration we'll wait (in ms) for a response from server
      uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
   Return value:
      void (none)
*/
void do_https_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  WiFiClientSecure client;
  client.setCACert(CA_CERT); // set cert for https
  if (client.connect(host, 443)) { 
    if (serial) Serial.print(request);
    client.print(request);
    memset(response, 0, response_size); 
    uint32_t count = millis();
    while (client.connected()) { 
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { 
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) { 
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}
