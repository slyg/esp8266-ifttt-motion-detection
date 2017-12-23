#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial
#define WIFI_SSID "******"
#define WIFI_PWD "******"
#define MOTION_PIN 5
#define ONE_SECOND 1000

String IFTTT_HOST = "maker.ifttt.com";
String IFTTT_EVENT = "motion";
String IFTTT_KEY = "******";
String WEB_HOOK_URL = "http://" + IFTTT_HOST + "/trigger/" + IFTTT_EVENT + "/with/key/" + IFTTT_KEY;

ESP8266WiFiMulti WiFiMulti;

bool motion = false;
bool prev_motion = false;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(MOTION_PIN, INPUT);

  USE_SERIAL.begin(9600);

  for(uint8_t t = 10; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(ONE_SECOND);
  }

  WiFiMulti.addAP(WIFI_SSID, WIFI_PWD);

}

void loop() {
  // wait for WiFi connection
  if((WiFiMulti.run() == WL_CONNECTED)) {
    USE_SERIAL.printf("[WIFI] connected\n");

    prev_motion = motion;
    motion = (digitalRead(MOTION_PIN) == HIGH);

    if (motion) {
      digitalWrite(LED_BUILTIN, HIGH);
      if (prev_motion) {
        USE_SERIAL.print("[STATUS] still motion...\n");
      } else {
        USE_SERIAL.print("[STATUS] motion detected !\n");
        HTTPClient http;
        http.begin(WEB_HOOK_URL);
        int httpCode = http.GET();
        http.end();
        USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
      }
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      USE_SERIAL.printf("[STATUS] no motion\n");
    }

  } else {
      USE_SERIAL.printf("[WIFI] connected\n");
  }
  delay(ONE_SECOND);
}
