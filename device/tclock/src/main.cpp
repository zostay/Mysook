#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <RTClib.h>
#include <Wire.h>

#include "tclock.h"

#include <MC_Panel.h>
#include <MC_RTC.h>

#define NEOPIXELS 13

Adafruit_NeoMatrix light = Adafruit_NeoMatrix(4, 8, NEOPIXELS,
  NEO_MATRIX_TOP  + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB         + NEO_KHZ800);

RTC_PCF8523 pfc8523;

mysook::MC_RGBPanel<4,8> panel(&light);
mysook::MC_RTC<RTC_PCF8523> rtc(&pfc8523);

ToddlerClock tclock(&panel, &rtc);

void setup() {
    pinMode(NEOPIXELS, OUTPUT);

    light.begin();
    pfc8523.begin();

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
