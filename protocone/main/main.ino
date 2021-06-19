#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

/* Screen declarations */
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define CHAR_WIDTH 12
#define CHAR_HEIGHT 16
#define MAX_LINE SCREEN_HEIGHT/CHAR_HEIGHT
#define MAX_COL SCREEN_WIDTH/CHAR_WIDTH

#define OLED_RESET     -1  // Reset pin
#define SCREEN_ADDRESS 0x3C  // See doc
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* GPS declarations */
TinyGPSPlus gps;
HardwareSerial hserial(1);

/* Protocone declarations */
typedef struct Protocone {
  bool is_active;
  uint32_t last_update_date, last_update_time;
  double lon, lat;
  uint8_t power, shields;
  String capture_code;
  String name, team;
} Protocone;

Protocone self;

void setup() {
  /* Debug */
  Serial.begin(115200);

  /* GPS init */
  hserial.begin(9600, SERIAL_8N1, 16, 17);

  /* Screen init */
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
  
  display.display();
  display.clearDisplay();

  /* Protocone init */
  protocone_init(&self);
}

void loop() {
  while (hserial.available() > 0) {
    if (gps.encode(hserial.read())) {
      self.is_active = protocone_update_gps(&self, gps);
    }
  }

  display.clearDisplay();
  if (self.is_active) {
    text_on_screen(String("P:") + self.power, 0, 0);
    text_on_screen(String("B:") + self.shields, MAX_COL - 2 - String(self.shields).length(), 0);

    text_on_line();
  } else {  // Error somewhere
    text_on_line(String(".inactive."), 0);
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("No GPS detected...");
    self.is_active = false;
    display.clearDisplay();
    text_on_line(String(".inactive."), 0);
    text_on_line(String("Erreur GPS"), 1);
    text_on_line(String("Call a mod"), 2);
    while(1);
  }

  delay(100);
}
