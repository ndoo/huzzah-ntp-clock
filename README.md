# huzzah-ntp-clock

Adafruit Feather Huzzah + 7seg FeatherWing NTP clock

## Caveats/Design Decision

* ESP8266 goes into Deep Sleep between per-minute updates
* Clock may go out of sync in between NTP updates (configurable via variable ntpInterval)
* Clock may go out of sync during Deep Sleep (configurable via variable espOffset)
* Clock requires shorting of GPIO16 to RST for wake from Deep Sleep

## Possible feature additions

* Automatic adjustment of espOffset after subsequent NTP updates

## Bill of Materials

* [Adafruit Feather HUZZAH](https://www.adafruit.com/product/2821)
* [Adafruit 0.56" 4-Digit 7-Segment FeatherWing Display](https://www.adafruit.com/products/3140)

## Software Required

* [Arduino IDE >1.6.4](https://www.arduino.cc/en/Main/Software)
* [ESP8266 Board add-on for Arduino IDE](https://github.com/esp8266/Arduino)

## Additional Libraries Required

* [Adafruit LED backpack](https://github.com/adafruit/Adafruit_LED_Backpack/archive/master.zip)
* [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library/archive/master.zip)
* [Arduino NTPClient (available from Arduino IDE library manager)](https://github.com/arduino-libraries/NTPClient)

## Assembly

* Solder female headers to Feather HUZZAH ([Adafruit guide](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/assembly) - do read the options for different headers/stacking if you are planning to use the board with more than just displays)
* Solder GPIO16 to RST using a short wire (to enable ESP8266 RTC wakeup from Deep Sleep)
* Assemble 7-segment FeatherWing ([Adafruit Guide](https://learn.adafruit.com/adafruit-7-segment-led-featherwings/assembly)
* Stack FeatherWing on Feather
* Edit SSID, password & NTP server
* Upload sketch to Feather HUZZAH
