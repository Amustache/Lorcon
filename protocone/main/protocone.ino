void protocone_init(Protocone* protocone) {
  protocone->is_active = true;  //false;
  protocone->last_update_date = 0;
  protocone->last_update_time = 0;
  protocone->lon = 0.0;
  protocone->lat = 0.0;
  protocone->power = 0;
  protocone->shields = 0;
  protocone->capture_code = "";
  protocone->name = "";
  protocone->team = "";
}

bool protocone_update_gps(Protocone& protocone, TinyGPSPlus gps) {
  if (gps.date.isValid() && gps.time.isValid()) {
    protocone->last_update_date = gps.date.value();
    protocone->last_update_time = gps.time.value();
  } else {
    return false;
  }

  if (gps.location.isValid()) {
    protocone->lon = gps.location.lng();
    protocone->lat = gps.location.lat();
  } else {
    return false;
  }
}
