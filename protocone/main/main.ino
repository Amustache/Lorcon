#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "arduino_secret.h"

/* Your SECRETS */
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASSWORD;
String serverName = SECRET_SERVERNAME;

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

/* Debug values */
#define TIMEOUT 300*1000  // 300s or 5min

/* GPS declarations */
TinyGPSPlus gps;
HardwareSerial hserial(1);

/* Protocone declarations */
typedef enum State {
  P_Error = -1,
  P_Init = 0,
  P_Loading = 1,
  P_Ok = 2,
} State;

typedef struct Protocone {
  String uniqueid;
  State state;
  uint8_t last_update[6];  // y, m, d, h, m, s
  double lon, lat;
  uint8_t power, shields;
  String capture_code;
  String name, team;
  String debug;
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

  /* Network init */
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() > TIMEOUT && self.state == P_Loading) {
      Serial.println("No WiFi...");
      self.state = P_Error;
      self.debug = "Timeout";
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Connected!");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
  }

  /* Protocone init */
  protocone_init(&self, String(WiFi.macAddress()));
}

void loop() {
  /* GPS update */
  while (hserial.available() > 0) {
    if (gps.encode(hserial.read())) {
      protocone_update_gps(&self, gps);
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("No GPS detected...");
    self.state = P_Error;
    self.debug = "No GPS";
  }

  if (millis() > TIMEOUT && self.state == P_Loading) {
    Serial.println("Loading taking too long...");
    self.state = P_Error;
    self.debug = "Timeout";
  }
  
  /* Displaying infos */
  display.clearDisplay();

  switch(self.state) {
    case P_Error:
      text_on_line(String(".inactive."), 0);
      text_on_line(String("E:") + self.debug, 1);
      text_on_line(String("Call a mod"), 3);
      while(1);
      break;
    case P_Init:
      text_on_line(String(".not init."), 0);
      break;
    case P_Loading:
      text_on_line(String("loading..."), 0);
      text_on_line(String(self.debug), 1);
      break;
    case P_Ok:
      text_on_line(String("working (:"), 0);
      // TODO
      break;
    default:
      text_on_line(String("... what?."), 0);
      break;
  }
  
  display.display();

  delay(1000);
}
