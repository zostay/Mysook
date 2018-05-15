#include "tclock.h"

#define NEOPIXELS 13

#ifdef ARDUINO
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <RTClib.h>
#include <Wire.h>

#include <MC_Logger.h>
#include <MC_Panel.h>
#include <MC_RTC.h>

Adafruit_NeoMatrix light = Adafruit_NeoMatrix(4, 8, NEOPIXELS,
  NEO_MATRIX_TOP  + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB         + NEO_KHZ800);

RTC_PCF8523 pfc8523;

mysook::MC_RGBPanel<4,8> panel(&light);
mysook::MC_RTC<RTC_PCF8523> rtc(&pfc8523);
mysook::MC_Logger<Print> logger(&Serial);
#endif//ARDUINO

ToddlerClock tclock(&logger, &panel, &rtc);

void setup() {
#ifdef ARDUINO
    pinMode(NEOPIXELS, OUTPUT);

    light.begin();
    pfc8523.begin();

    //pfc8523.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //pfc8523.adjust(DateTime(2018, 5, 15, 18, 58, 55));
    //pfc8523.adjust(DateTime(2018, 5, 15, 7, 0, 55));

    Serial.begin(57600);
#endif//ARDUINO

    tclock.setup();
}

void loop() {
    tclock.tick();
}
