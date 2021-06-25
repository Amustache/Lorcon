/*int httpPOSTRequest(const String serverName, String json) {
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

String httpGETRequest(const String serverName) {
  HTTPClient http;
    
  http.begin(serverName);
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  // Free resources
  http.end();

  return payload;
}

int json_post(String endpoint, String json) {
  if (WiFi.status() == WL_CONNECTED) {
    return httpPOSTRequest(serverName + endpoint, json);
  } else {
    Serial.println("WiFi Disconnected");
    return -1;
  }
}

String * json_get(String endpoint) {
  if (WiFi.status() == WL_CONNECTED) {
    String request = httpGETRequest(serverName + endpoint);
    JSONVar myObject = JSON.parse(request);
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return NULL;
    }
    JSONVar keys = myObject.keys();
    String result[keys.length()];
    for (int i = 0; i < keys.length(); i++) {
      JSONVar value = myObject[keys[i]];
      sensorReadingsArr[i] = String(value);
    }
  } else {
    Serial.println("WiFi Disconnected");
    return NULL;
  }
}*/
