void text_on_line(const uint8_t line, const char* string) {
  display.setCursor(0, (line + 1) * CHAR_HEIGHT);
  display.print(string);
}

void text_on_screen(const uint8_t col, const uint8_t line, const char* string) {
  display.setCursor(col * CHAR_WIDTH, (line + 1) * CHAR_HEIGHT);
  display.print(string);
}
