int httpPOSTRequest(const char* endpoint, const char* input) {
  HTTPClient http;
  
  http.begin(endpoint);
  
  http.addHeader("Content-Type", "application/json");

  // Send HTTP POST request
  int httpResponseCode = http.POST(input);
  
  if (httpResponseCode > 0) {
    Serial.print("POST HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("POST Error code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
  
  return httpResponseCode;
}

void httpGETRequest(const char* endpoint, char* output) {
  HTTPClient http;
    
  http.begin(endpoint);
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
    
  if (httpResponseCode > 0) {
    Serial.print("GET HTTP Response code: ");
    Serial.println(httpResponseCode);
    strncpy(output, http.getString().c_str(), SIZE_FROM_EXT);
  } else {
    Serial.print("GET Error code: ");
    Serial.println(httpResponseCode);
    output = NULL;
  }
  
  // Free resources
  http.end();
}

int json_post(const char* endpoint, const char* input) {
  if (WiFi.status() == WL_CONNECTED) {
    return httpPOSTRequest(endpoint, input);
  } else {
    Serial.println("WiFi Disconnected");
    return -1;
  }
}

void json_get(const char* endpoint, char* dest) {
  if (WiFi.status() == WL_CONNECTED) {
    httpGETRequest(endpoint, dest);
  } else {
    Serial.println("WiFi Disconnected");
    dest = NULL;
  }
}

int protocone_to_server(const char* endpoint, Protocone* protocone) {
  char buf[SIZE_TO_EXT];
  protocone_to_json(protocone, buf);
  Serial.println(buf);
  return json_post(endpoint, buf);
}

void server_to_protocone(const char* endpoint, Protocone* protocone) {
  char buf[SIZE_FROM_EXT];  // Beware of the spaces coming from the server...
  json_get(endpoint, buf);
  Serial.println(buf);
  protocone_from_json(protocone, buf);
}
