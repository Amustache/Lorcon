void text_on_line(const String string, const uint8_t line) {
  display.setCursor(0, line * CHAR_HEIGHT);
  display.print(string);
  display.display();
}

void text_on_screen(const String string, const uint8_t col, const uint8_t line) {
  display.setCursor(col * CHAR_WIDTH, line * CHAR_HEIGHT);
  display.print(string);
  display.display();
}
