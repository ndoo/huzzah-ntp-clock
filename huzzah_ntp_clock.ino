// setup 8266 chip
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// setup LED backpack
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
Adafruit_7segment matrix = Adafruit_7segment();

#include <NTPClient.h>
#include <WiFiUdp.h>

// Wi-Fi Configuration
const char*   ssid        = "YOUR_SSID";
const char*   password    = "YOUR_PASSWORD";

// NTP Configuration
const char*   ntpServer   = "2.sg.pool.ntp.org";
const int8_t  ntpOffset   = 8; // hours
const uint8_t ntpInterval = 5; // minutes

// ESP8266 microseconds of compensation per sleep-cycle minute
const uint16_t espOffset  = 2000000; // microseconds

// Struct to store ESP8266 rst_info
struct rst_info {
  uint32 reason;
  uint32 exccause;
  uint32 epc1;
  uint32 epc2;
  uint32 epc3;
  uint32 excvaddr;
  uint32 depc;
} *rsti;

// Struct to store time, mostly for ESP8266 RTC memory
struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint8_t nextNtp;
} timeStruct;

void setup() {

  matrix.begin(0x70);
  Serial.begin(115200);
  Serial.println();

  rsti = ESP.getResetInfoPtr();
  bool wokeUp = rsti->reason == 5;

  if (wokeUp) {
    Serial.print("Reading time from RTC...");
    ESP.rtcUserMemoryRead(0, (uint32_t*) &timeStruct, sizeof(timeStruct));
    Serial.println(" OK");
    if (--timeStruct.nextNtp == 0) {
      getTimeFromNtp();
      wokeUp = false; // Ensure Deep Sleep compensates any changes to seconds
    }
  } else {
    matrix.writeDigitRaw(0,84); // n
    matrix.writeDigitRaw(1,120); // t
    matrix.writeDigitRaw(3,115); // p
    matrix.writeDigitRaw(4,0);
    matrix.writeDisplay();
    getTimeFromNtp();
  }

  updateDisplay();

  // Increment time
  if (++timeStruct.minutes >= 60) {
    timeStruct.minutes = 0;
    timeStruct.hours++;
  }
  if (timeStruct.hours >= 24) timeStruct.hours = 0;

  Serial.print("Next NTP update in ");
  Serial.println(timeStruct.nextNtp);

  // Write time to ESP8266 RTC memory
  Serial.print("Writing time to RTC...");
  ESP.rtcUserMemoryWrite(0, (uint32_t*) &timeStruct, sizeof(timeStruct));

  // ESP8266 Deep Sleep
  Serial.println(" OK. Going to sleep until next minute...");
  uint32_t sleep = wokeUp ? 60000000 - micros() : (60 - timeStruct.seconds) * 1000000;
  ESP.deepSleep(sleep + espOffset, WAKE_NO_RFCAL);

}

void loop() {};

void getTimeFromNtp() {

  // Begin WiFi
  Serial.print("Wi-Fi starting...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) yield();
  Serial.println(" OK.");

  // Begin NTP
  Serial.print("Begin NTP...");
  WiFiUDP udp;
  NTPClient timeClient(udp, ntpServer, ntpOffset * 3600);
  timeClient.begin();
  while (!timeClient.update()) yield();
  timeStruct.hours = timeClient.getHours();
  timeStruct.minutes = timeClient.getMinutes();
  timeStruct.seconds = timeClient.getSeconds();
  timeStruct.nextNtp = ntpInterval;
  Serial.println(" OK.");

}

void updateDisplay() {
  Serial.print("Updating display...");
  matrix.writeDigitNum(0, timeStruct.hours < 10 ? 0 : timeStruct.hours / 10);
  matrix.writeDigitNum(1, timeStruct.hours % 10);
  matrix.writeDigitNum(3, timeStruct.minutes < 10 ? 0 : timeStruct.minutes / 10);
  matrix.writeDigitNum(4, timeStruct.minutes % 10);
  matrix.drawColon(1);
  matrix.writeDisplay();
  Serial.println(" OK.");
}
