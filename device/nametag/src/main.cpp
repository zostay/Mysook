#ifdef ARDUINO
#include <Adafruit_DotStarMatrix.h>
//#include <Fonts/TomThumb.h>

//extern const GFXfont TomThumb;

#include <MC_Logger.h>
#include <MC_Panel.h>
#include <MC_Network.h>

#include "secrets.h"
#else
#include <SimLogger.h>
#include <SimPanel.h>
#include <Network.h>
#endif//ARDUINO

#include "nametag.h"

#ifdef ARDUINO
#define DATAPIN  27
#define CLOCKPIN 13

Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
        12, 6, DATAPIN, CLOCKPIN,
        DS_MATRIX_BOTTOM     + DS_MATRIX_LEFT +
        DS_MATRIX_ROWS + DS_MATRIX_PROGRESSIVE,
        DOTSTAR_BGR);

mysook::MC_Logger<Print> logger(&micros, Serial);
mysook::MC_RGBPanel<12,6,Adafruit_DotStarMatrix> display(matrix);
mysook::MC_Network network(logger);
mysook::MC_MDNS mdns("zostay-nametag", logger);
#else
mysook::SimLogger logger;
mysook::SimPanel<12,6> display;
mysook::Network network;
#endif//ARDUINO

NameTag nametag(logger, display, network);

void setup() {
#ifdef ARDUINO
    Serial.begin(115200);

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

    mdns.add_service("http", "tcp", 80);
#endif//ARDUINO

    nametag.setup();
}

void loop() {
    nametag.loop();
}

#ifndef ARDUINO
int main(int argc, char **argv) {
    setup();
    while (true) loop();
}
#endif//ARDUINO
