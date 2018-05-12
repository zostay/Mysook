#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "RTClib.h"
#include <Wire.h>

#include "tclock.h"

#include "arduino/panel.h"
#include "arduino/rtc.h"

#define NEOPIXELS 13

Adafruit_NeoMatrix light = Adafruit_NeoMatrix(4, 8, NEOPIXELS,
  NEO_MATRIX_TOP  + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB         + NEO_KHZ800);

RTC_PCF8523 pfc8523;

mysook::ArduinoRGBPanel panel(&light);
mysook::ArduinoRTC<RTC_PCF8523> rtc(&pfc8523);

ToddlerClock tclock(&panel, &rtc);

void setup() {
    pinMode(NEOPIXELS, OUTPUT);

    light.begin();
    rtc.begin();

    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //rtc.adjust(DateTime(2018, 04, 22, 20, 0, 0));
    //rtc.adjust(DateTime(2018, 04, 22, 19, 29, 30));
    //rtc.adjust(DateTime(2018, 04, 22, 6, 59, 30));

    Serial.begin(57600);

    tclock.setup();
}

void loop() {
    tclock.tick();
}
