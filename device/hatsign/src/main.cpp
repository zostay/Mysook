#ifdef ARDUINO
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
//#include <Fonts/TomThumb.h>

//extern const GFXfont TomThumb;

#include <MC_Logger.h>
#include <MC_Panel.h>
#include <MC_Network.h>
#include <MC_MDNS.h>
#include <LED.h>

#include "secrets.h"
#else
#include <SimLogger.h>
#include <SimPanel.h>
#include <Network.h>
#endif//ARDUINO

#include "nametag.h"

#ifdef ARDUINO
#define MATRIXPIN 14
#define BLINKPIN  13
#define BATPIN    A13

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
    32, 8, MATRIXPIN,
    NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
    NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
    NEO_GRB            + NEO_KHZ800);

mysook::MC_Logger<Print> logger(&micros, Serial);
mysook::MC_RGBPanel<32,8,Adafruit_NeoMatrix> display(matrix);
mysook::MC_Network network(logger);
//mysook::MC_MDNS mdns("zostay-nametag", logger);
mysook::WiFiLED led(BLINKPIN, network);
#else
mysook::SimLogger logger;
mysook::SimPanel<32,8> display;
mysook::Network network;
#endif//ARDUINO

NameTag nametag(logger, display, led, network);

void setup() {
#ifdef ARDUINO
    Serial.begin(115200);

    pinMode(BATPIN, INPUT);
    pinMode(MATRIXPIN, OUTPUT);
    pinMode(BLINKPIN, OUTPUT);

    // uncomment to have wait
    //while (!Serial) delay(500); 

    //Serial.println("\nDotstar Matrix Wing");
    matrix.begin();
    //matrix.setFont(&TomThumb);
    //matrix.setTextWrap(false);

    for (int i = 0; i < ap_config_count; ++i) {
        network.add_access_point(ap_configs[i * 2], ap_configs[i * 2 + 1]);
    }
    network.connect();

    //mdns.add_service("http", "tcp", 80);
#endif//ARDUINO

    nametag.setup();
}

void loop() {
    if (millis() % 10000 == 0) {
        int volts = analogRead(BATPIN);
        logger.logf_ln("I [main] VBAT: %0.3fV", volts * .002);
    }

    nametag.loop();
}

#ifndef ARDUINO
int main(int argc, char **argv) {
    setup();
    while (true) loop();
}
#endif//ARDUINO