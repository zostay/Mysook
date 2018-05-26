#ifdef ARDUINO
#include <Adafruit_DotStarMatrix.h>
#include <Fonts/TomThumb.h>

#include <MC_Logger.h>
#include <MC_Panel.h>
#include <MC_Network.h>

#include "secrets.h"
#endif//ARDUINO

#include "nametag.h"

#define DATAPIN  27
#define CLOCKPIN 13

#ifdef ARDUINO
Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
        12, 6, DATAPIN, CLOCKPIN,
        DS_MATRIX_BOTTOM     + DS_MATRIX_LEFT +
        DS_MATRIX_ROWS + DS_MATRIX_PROGRESSIVE,
        DOTSTAR_BGR);

mysook::MC_Logger<Print> logger(Serial);
mysook::MC_RGBPanel<12,6,Adafruit_DotStarMatrix> display(matrix);
mysook::MC_Network network(logger);
#endif//ARDUINO

NameTag nametag(logger, display);

void setup() {
#ifdef ARDUINO
    Serial.begin(115200);

    // uncomment to have wait
    //while (!Serial) delay(500); 

    Serial.println("\nDotstar Matrix Wing");
    matrix.begin();
    matrix.setFont(&TomThumb);
    matrix.setTextWrap(false);

    for (int i = 0; i < ap_config_count; ++i) {
        network.add_access_point(ap_configs[i * 2], ap_configs[i * 2 + 1]);
    }
    network.connect();
#endif//ARDUINO

    nametag.setup();
}

void loop() {
    network.connect();
    nametag.loop();
}
