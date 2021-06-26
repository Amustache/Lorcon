int httpPOSTRequest(const char* serverName, const char* json) {
  HTTPClient http;
  
  http.begin(serverName);
  
  http.addHeader("Content-Type", "application/json");

  // Send HTTP POST request
  int httpResponseCode = http.POST(json);
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
  
  return httpResponseCode;
}

/*void httpGETRequest(const char* serverName, char* dest) {
  HTTPClient http;
    
  http.begin(serverName);
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
    
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    strcpy(dest, http.getString().c_str());
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  // Free resources
  http.end();
}*/

int json_post(const char* endpoint, const char* json) {
  if (WiFi.status() == WL_CONNECTED) {
    return httpPOSTRequest(endpoint, json);
  } else {
    Serial.println("WiFi Disconnected");
    return -1;
  }
}

/*void json_get(const char* endpoint, char** dest) {
  if (WiFi.status() == WL_CONNECTED) {
    char request[256];
    httpGETRequest(endpoint, request);
    JSONVar myObject = JSON.parse(request);
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
    }
    JSONVar keys = myObject.keys();
    char result[keys.length()][256];
    for (int i = 0; i < keys.length(); i++) {
      JSONVar value = myObject[keys[i]];
      strcpy(result + i * 256 , value);
    }
  } else {
    Serial.println("WiFi Disconnected");
    dest = NULL;
  }
}*/
