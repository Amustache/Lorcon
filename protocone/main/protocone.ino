void protocone_init(Protocone* protocone, String uniqueid_) {
  protocone->uniqueid = uniqueid_;
  protocone->state = P_Init;
  protocone->last_update[0] = protocone->last_update[1] = protocone->last_update[2] = protocone->last_update[3] = protocone->last_update[4] = protocone->last_update[5] = -1;
  protocone->lon = 0.0;
  protocone->lat = 0.0;
  protocone->power = -1;
  protocone->shields = -1;
  protocone->capture_code = "";
  protocone->name = "";
  protocone->team = "";
  protocone->debug = "";
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
    Serial.println(String("Date and time ok: ") + protocone_get_last_update(protocone));
  } else {
    Serial.println("Date or time not valid.");
    protocone->state = P_Loading;
    protocone->debug = "Datetime";
    return;
  }

  if (gps.location.isValid()) {
    protocone->lon = gps.location.lng();
    protocone->lat = gps.location.lat();
    Serial.println(String("GPS ok: ") + protocone->lon + ", " + protocone->lat);
  } else {
    Serial.println("GPS not valid.");
    protocone->state = P_Loading;
    protocone->debug = "GPS";
    return;
  }

  protocone->state = P_Ok;
  protocone->debug = "";

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

String protocone_get_last_update_date(Protocone* protocone) {
  return String(protocone->last_update[0]) + "-" + protocone->last_update[1] + "-" + protocone->last_update[2];
}

String protocone_get_last_update_time(Protocone* protocone) {
  return String(protocone->last_update[3]) + ":" + protocone->last_update[4] + ":" + protocone->last_update[5];
}

String protocone_get_last_update(Protocone* protocone) {
  return protocone_get_last_update_date(protocone) + " " + protocone_get_last_update_time(protocone);
}
