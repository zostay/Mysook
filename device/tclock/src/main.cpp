#include <esp_system.h>

#include "tclock_config.h"
#include "tclock.h"

#ifdef NAMETAG
#define DATAPIN  27
#define CLOCKPIN 13
#elif
#define NEOPIXELS 13
#endif

#define ZOSTAYIFY_PORT 10101

#ifdef ARDUINO
#include "secrets.h"

#include <Adafruit_GFX.h>
#include <RTClib.h>
#include <Wire.h>

#include <MC_Logger.h>
#include <MC_MDNS.h>
#include <MC_Network.h>
#include <MC_Panel.h>
#include <MC_RTC.h>
#include <MC_UDP.h>

#ifdef NAMETAG
#include <Adafruit_DotStarMatrix.h>

typedef Adafruit_DotStarMatrix LightMatrix;
Adafruit_DotStarMatrix light = Adafruit_DotStarMatrix(
    LIGHT_WIDTH, LIGHT_HEIGHT, DATAPIN, CLOCKPIN,
    DS_MATRIX_BOTTOM  + DS_MATRIX_LEFT +
    DS_MATRIX_COLUMNS + DS_MATRIX_PROGRESSIVE,
    DOTSTAR_BGR);
#elif
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

typedef Adafruit_NeoMatrix LightMatrix;
Adafruit_NeoMatrix light = Adafruit_NeoMatrix(
    LIGHT_WIDTH, LIGHT_HEIGHT, NEOPIXELS,
    NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
    NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
    NEO_GRB            + NEO_KHZ800);
#endif

#ifdef NAMETAG
typedef RTC_Millis MyRTC;
#elif
typedef RTC_PCF8523 MyRTC;
#endif

MyRTC rtc_chip;

mysook::MC_RGBPanel<LIGHT_WIDTH,LIGHT_HEIGHT,LightMatrix> panel(light);
mysook::MC_RTC<MyRTC> rtc(&rtc_chip);
mysook::MC_Logger<Print> logger(micros, &Serial);
mysook::MC_Network network(logger);
mysook::MC_MDNS mdns("isaiah-clock", logger);
#else
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <SimPanel.h>
#include <SimRTC.h>
#include <SimLogger.h>

mysook::SimPanel<4,8> panel;
mysook::SimRTC rtc;
mysook::SimLogger logger;
#endif//ARDUINO

ToddlerClock tclock(logger, &panel, network, &rtc, ZOSTAYIFY_PORT);

// Globals for Watchdog timer
const int wdtTimeout = 5000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}

void setup() {
#ifdef ARDUINO
#ifdef NAMETAG
    pinMode(DATAPIN, OUTPUT);
    pinMode(CLOCKPIN, OUTPUT);
#elif
    pinMode(NEOPIXELS, OUTPUT);
#endif

    light.begin();

#ifdef NAMETAG
    rtc_chip.begin(DateTime(F(__DATE__), F(__TIME__)));
#elif
    rtc_chip.begin();

    //rtc_chip.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //rtc_chip.adjust(DateTime(2018, 5, 15, 18, 58, 55));
    //rtc_chip.adjust(DateTime(2018, 5, 15, 7, 0, 55));
    //rtc_chip.adjust(DateTime(2018, 11, 4, 0, 15, 0));
#endif

    Serial.begin(115200);

    for (int i = 0; i < ap_config_count; ++i) {
        network.add_access_point(ap_configs[i * 2], ap_configs[i * 2 + 1]);
    }
    network.connect();

	mdns.add_service("zostayify", "udp", ZOSTAYIFY_PORT);
#else
    if (!al_init()) {
        std::cerr << "failed to initialize allegro" << std::endl;
        exit(1);
    }

    if (!al_init_primitives_addon()) {
        std::cerr << "failed to initialize primitives" << std::endl;
        exit(1);
    }

    panel.initialize();

    rtc.set_offset(-5 * 60 * 60); // US/Central + DST
    //rtc.adjust_to(mysook::DateTime(2018, 5, 17, 7, 0, 55).unixtime());
#endif//ARDUINO

    tclock.setup();

    // Watchdog Timer to reboot on crash
    timer = timerBegin(0, 80, true);                  //timer 0, div 80
    timerAttachInterrupt(timer, &resetModule, true);  //attach callback
    timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
    timerAlarmEnable(timer);                          //enable interrupt
}

void loop() {
    timerWrite(timer, 0); //reset timer (feed watchdog)
    tclock.loop();
}

#ifndef ARDUINO
int main(int argc, char **argv) {
    setup();
    while (true) loop();
}
#endif//ARDUINO
