void protocone_init(Protocone* protocone, const char* uniqueid_) {
  strcpy(protocone->uniqueid, uniqueid_);
  protocone->state = P_Init;
  // y, m, d, h, m, s
  protocone->last_update[0] = protocone->last_update[1] = protocone->last_update[2] = protocone->last_update[3] = protocone->last_update[4] = protocone->last_update[5] = 0;
  protocone->lon = 0.0;
  protocone->lat = 0.0;
  protocone->power = 0;
  protocone->shields = 0;
  strcpy(protocone->capture_code, "d00d2bad");
  strcpy(protocone->name, "");
  strcpy(protocone->team, "");
  strcpy(protocone->debug, "");
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
    strcpy(protocone->debug, "Datetime");
    return;
  }

  if (gps.location.isValid()) {
    protocone->lon = gps.location.lng();
    protocone->lat = gps.location.lat();
  } else {
    Serial.println("GPS not valid.");
    protocone->state = P_Loading;
    strcpy(protocone->debug, "GPS");
    return;
  }

  protocone->state = P_Ok;
  strcpy(protocone->debug, "");

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
    strcpy(dest, protocone->uniqueid);
  } else {
    strcpy(dest, protocone->name);
  }
}

void protocone_get_team(Protocone* protocone, char* dest) {
  if (!strcmp(protocone->team, "")) {
    strcpy(dest, "Personne !");
  } else {
    strcpy(dest, protocone->team);
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
