#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "RTClib.h"
#include <Wire.h>

#define NEOPIXELS 13

Adafruit_NeoMatrix light = Adafruit_NeoMatrix(4, 8, NEOPIXELS,
  NEO_MATRIX_TOP  + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB         + NEO_KHZ800);

RTC_PCF8523 rtc;

struct TimeSetting {
  uint8_t h, m;
};

struct ColorSetting {
  uint8_t r, g, b;
  uint8_t brightness;
};
void blank_light_color() {
  light.setBrightness(5);
  light.fillScreen(light.Color(0, 0, 0));
  light.show();
}

void set_light_color(DateTime &d, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
  light.setBrightness(brightness);
  light.fillScreen(light.Color(r, g, b));

  int i = 0;
  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 4; ++x) {
      if (i++ % 2 == d.day() % 2) {
        light.drawPixel(x, y, light.Color(0, 0, 0));
      }
    }
  }
  
  light.show();
}

void set_light_color(DateTime &d, ColorSetting &color) {
  set_light_color(d, color.r, color.g, color.b, color.brightness);
}

void setup() {
  pinMode(NEOPIXELS, OUTPUT);

  light.begin();
  blank_light_color();

  rtc.begin();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //rtc.adjust(DateTime(2018, 04, 22, 20, 0, 0));
  //rtc.adjust(DateTime(2018, 04, 22, 19, 29, 30));
  //rtc.adjust(DateTime(2018, 04, 22, 6, 59, 30));

  Serial.begin(57600);
}

TimeSetting morning_time = { .h=7, .m=0 };
TimeSetting sleeping_time = { .h=19, .m=30 };
ColorSetting night_color = { .r=255, .g=170, .b=0,   .brightness=2 };
ColorSetting day_color   = { .r=0,   .g=80,  .b=255, .brightness=5 };

bool before_time(DateTime &check, TimeSetting &against) {
  return check.hour() < against.h
      || (check.hour() == against.h && check.minute() < against.m);
}

void loop() {
  DateTime now = rtc.now();

  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);

  if (before_time(now, morning_time)) {
    Serial.println(" is early morning.");
    set_light_color(now, night_color);
  }

  else if (before_time(now, sleeping_time)) {
    Serial.println(" is day.");
    set_light_color(now, day_color);
  }

  else {
    Serial.println(" is night.");
    set_light_color(now, night_color);
  }

  delay(1000);
}
