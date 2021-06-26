void text_on_line(const uint8_t line, const char* string) {
  display.setCursor(0, (line + 1) * CHAR_HEIGHT);
  display.print(string);
}

void text_on_screen(const uint8_t col, const uint8_t line, const char* string) {
  display.setCursor(col * CHAR_WIDTH, (line + 1) * CHAR_HEIGHT);
  display.print(string);
}

void display_info(Protocone* protocone, uint8_t timer) {
  char buf[11];

  protocone_get_name_or_id(protocone, buf);
  text_on_line(0, buf);

  if (timer%15 / 5 == 0) {
    display_info_main(protocone);
  }

  if (timer%15 / 5 == 1) {
    display_info_gps(protocone);
  }

  if (timer%15 / 5 == 2) {
    display_info_team(protocone);
  }
}

void display_info_main(Protocone* protocone) {
  char buf[11];
  snprintf(buf, sizeof(buf), "P:%d%*cS:%d", protocone->power, 2 + (1 - protocone->power/10) + (1 - protocone->shields/10), ' ', protocone->shields);  // Justify
  text_on_line(1, buf);
  text_on_line(2, "Code :");
  snprintf(buf, sizeof(buf), "%10s", protocone->capture_code);  // Align right
  text_on_line(3, buf);
}

void display_info_team(Protocone* protocone) {
  char buf[11];
  text_on_line(1, "Capture :");
  protocone_get_team(protocone, buf);
  snprintf(buf, sizeof(buf), "%10s", buf);  // Align right
  text_on_line(2, buf);
}

void display_info_gps(Protocone* protocone) {
  char buf[11];
  protocone_get_time(protocone, buf);
  text_on_line(1, buf);
  snprintf(buf, sizeof(buf), "lat %.2f", protocone->lat);
  text_on_line(2, buf);
  snprintf(buf, sizeof(buf), "lon %.2f", protocone->lon);
  text_on_line(3, buf);
}
