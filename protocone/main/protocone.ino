void protocone_init(Protocone* protocone, const char* uniqueid_) {
  strncpy(protocone->uniqueid, uniqueid_, 8);
  protocone->state = P_Init;
  // y, m, d, h, m, s
  protocone->last_update[0] = protocone->last_update[1] = protocone->last_update[2] = protocone->last_update[3] = protocone->last_update[4] = protocone->last_update[5] = 0;
  protocone->lon = 0.0;
  protocone->lat = 0.0;
  protocone->power = 0;
  protocone->shields = 0;
  strncpy(protocone->capture_code, "d00d2bad", 8);
  strncpy(protocone->name, "", 10);
  strncpy(protocone->team, "", 10);
  strncpy(protocone->debug, "", 10);
  Serial.println("Protocone init done.");
}

void protocone_update_gps(Protocone* protocone, TinyGPSPlus gps) {
  if (gps.date.isValid() && gps.time.isValid()) {
    protocone->last_update[0] = gps.date.year() - 2000;
    protocone->last_update[1] = gps.date.month();
    protocone->last_update[2] = gps.date.day();
    protocone->last_update[3] = (gps.time.hour() + 2) % 24; // GMT+2
    protocone->last_update[4] = gps.time.minute();
    protocone->last_update[5] = gps.time.second();
  } else {
    Serial.println("Date or time not valid.");
    protocone->state = P_Loading;
    strncpy(protocone->debug, "Datetime", 10);
    return;
  }

  if (gps.location.isValid()) {
    protocone->lon = gps.location.lng();
    protocone->lat = gps.location.lat();
  } else {
    Serial.println("GPS not valid.");
    protocone->state = P_Loading;
    strncpy(protocone->debug, "GPS", 10);
    return;
  }

  protocone->state = P_Ok;
  strncpy(protocone->debug, "", 10);

  // Debug
  Serial.print(F("Location: "));
  Serial.print(gps.location.lat(), 6);
  Serial.print(F(","));
  Serial.print(gps.location.lng(), 6);
  Serial.print(F("  Date/Time: "));
  Serial.print(gps.date.month());
  Serial.print(F("/"));
  Serial.print(gps.date.day());
  Serial.print(F("/"));
  Serial.print(gps.date.year());
  Serial.print(F(" "));
  if (gps.time.hour() < 10) Serial.print(F("0"));
  Serial.print(gps.time.hour());
  Serial.print(F(":"));
  if (gps.time.minute() < 10) Serial.print(F("0"));
  Serial.print(gps.time.minute());
  Serial.print(F(":"));
  if (gps.time.second() < 10) Serial.print(F("0"));
  Serial.print(gps.time.second());
  Serial.print(F("."));
  if (gps.time.centisecond() < 10) Serial.print(F("0"));
  Serial.println(gps.time.centisecond());
}

void protocone_get_name_or_id(Protocone* protocone, char* dest) {
  if (!strcmp(protocone->name, "")) {
    strncpy(dest, protocone->uniqueid, 10);
  } else {
    strncpy(dest, protocone->name, 10);
  }
}

void protocone_get_team(Protocone* protocone, char* dest) {
  if (!strcmp(protocone->team, "")) {
    strncpy(dest, "Personne !", 10);
  } else {
    strncpy(dest, protocone->team, 10);
  }
}

void protocone_get_date(Protocone* protocone, char* dest) {
  // dd-mm-yyyy
  snprintf(dest, sizeof(char) * 11, "%02d-%02d-20%02d", protocone->last_update[2], protocone->last_update[1], protocone->last_update[0]);
}

void protocone_get_time(Protocone* protocone, char* dest) {
  // hh:mm
  snprintf(dest, sizeof(char) * 6, "%02d:%02d", protocone->last_update[3], protocone->last_update[4]);
}

void protocone_from_json(Protocone* protocone, char* input) {
  StaticJsonDocument<SIZE_FROM_EXT> doc;
  
  DeserializationError error = deserializeJson(doc, input, SIZE_FROM_EXT);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  JsonObject json = doc[protocone->uniqueid];

  protocone->power = json["power"];
  protocone->shields = json["shields"];
  strncpy(protocone->capture_code, json["capture_code"], 8);
  strncpy(protocone->name, json["name"], 10);
  strncpy(protocone->team, json["team"], 10);
}

void protocone_to_json(Protocone* protocone, char* output) {
  StaticJsonDocument<SIZE_TO_EXT> doc;

  JsonObject json = doc.createNestedObject(protocone->uniqueid);
  json["state"] = protocone->state;
  
  JsonObject last_update = json.createNestedObject("last_update");
  last_update["year"] = protocone->last_update[0];
  last_update["month"] = protocone->last_update[1];
  last_update["day"] = protocone->last_update[2];
  last_update["hours"] = protocone->last_update[3];
  last_update["minutes"] = protocone->last_update[4];
  last_update["seconds"] = protocone->last_update[5];
  
  JsonObject location = json.createNestedObject("location");
  location["lat"] = protocone->lat;
  location["lon"] = protocone->lon;
  
  json["debug"] = protocone->debug;
  
  serializeJson(doc, output, SIZE_TO_EXT);
}

void protocone_update_with_server(Protocone* protocone, const char* endpoint) {
  char dest[256];
  strncpy(dest, endpoint, 128);
  strncat(dest, protocone->uniqueid, 128);
  
  protocone_to_server(dest, protocone);
  server_to_protocone(dest, protocone);
}
