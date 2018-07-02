#ifdef ARDUINO
#include <Adafruit_NeoMatrix.h>

#include <MC_Logger.h>
#include <MC_Panel.h>
#else
#include <SimLogger.h>
#include <SimPanel.h>
#endif//ARDUINO

#include "on-air-sign.h"

#ifdef ARDUINO
#define PIN_GRID  2

#define GRID_ADDRESS 42

Adafruit_NeoMatrix matrix(8, 8, 2,
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
    NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
    NEO_GRBW + NEOKHZ800
);

mysook::MC_Logger<Print> logger(&micros, Serial);
mysook::MC_RGBPanel<8,8,Adafruit_NeoMatrix> display(matrix);
#else
mysook::SimLogger logger;
mysook::SimPanel<8,8> display;
#endif//ARDUINO

OnAirSign sign(logger, display);

void setup() {
#ifdef ARDUINO
    Serial.begin(115200);

    pinMode(PIN_GRID, OUTPUT);

    matrix.begin();

    Wire.begin(GRID_ADDRESS);
#endif//ARDUINO

    sign.setup();
}

void loop() {
    char buf[32];
    if (Wire.available()) {
        size_t s = Wire.read(buf, 32);
        sign.send(buf[0], buf+1, s - 1);
    }
    
    sign.loop();
}   

#ifndef ARDUINO
int main(int argc, char **argv) {
    setup();
    while (true) loop();
}
#endif//ARDUINO
