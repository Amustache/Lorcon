#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <U8g2lib.h>
#include "arduino_secret.h"
#include <string.h>

/* Your SECRETS */
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASSWORD;
const char* serverName = SECRET_SERVERNAME;

/* Screen declarations */
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define CHAR_WIDTH 12
#define CHAR_HEIGHT 16
#define MAX_LINE SCREEN_HEIGHT/CHAR_HEIGHT
#define MAX_COL SCREEN_WIDTH/CHAR_WIDTH

U8G2_SSD1306_128X64_NONAME_F_SW_I2C display(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);

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
  char uniqueid[8];
  State state;
  uint8_t last_update[6];  // y, m, d, h, m, s
  double lon, lat;
  uint8_t power, shields;
  char capture_code[8];
  char name[16];
  char team[16];
  char debug[16];
} Protocone;

Protocone self;

void setup() {
  /* Debug */
  Serial.begin(115200);

  /* GPS init */
  hserial.begin(9600, SERIAL_8N1, 16, 17);

  /* Screen init */
  display.begin();
  display.setFont(u8g2_font_profont22_tr);

  display.clearBuffer();
  text_on_line(0, "  LORCON  ");
  text_on_line(1, "Bienvenue!");
  display.sendBuffer();
  delay(1000);

  /* Network init */
  WiFi.begin(ssid, password);
  Serial.println("Connecting");

  display.clearBuffer();
  strcpy(self.debug, "WiFi");
  text_on_line(0, "Loading..");
  text_on_line(1, self.debug);
  display.sendBuffer();
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500); 
    Serial.print(".");
    if (millis() > TIMEOUT && self.state == P_Loading) {
      Serial.println("No WiFi...");
      self.state = P_Error;
      strcpy(self.debug, "E:TimeWiFi");
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
  protocone_init(&self, "abcdef");
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
    strcpy(self.debug, "E:No GPS");
  }

  if (millis() > TIMEOUT && self.state == P_Loading) {
    Serial.println("Loading taking too long...");
    self.state = P_Error;
    strcpy(self.debug, "E:TimeGPS");
  }
  
  /* Displaying infos */
  display.clearBuffer();
  
  switch(self.state) {
    case P_Error:
      text_on_line(0, "Inactive");
      text_on_line(1, self.debug);
      text_on_line(3, "Call a mod");
      break;
    case P_Init:
      text_on_line(0, "Initial..");
      text_on_line(1, self.debug);
      break;
    case P_Loading:
      text_on_line(0, "Loading..");
      text_on_line(1, self.debug);
      break;
    case P_Ok:
      text_on_line(0, "Working (:");
      // TODO
      break;
    default:
      text_on_line(0, "DEADBEEF");
      //text_on_line(String("... what?."), 0);
      break;
  }
  
  display.sendBuffer();

  delay(1000);
}
