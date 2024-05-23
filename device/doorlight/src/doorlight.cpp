#ifdef ARDUINO
#include <Arduino.h>
#endif//ARDUINO

#include "doorlight.h"

void Doorlight::start() {
    logf_ln("D [doorlight] Starting Doorlight firmware");
}

void Doorlight::tick() {
    logf_ln("D [doorlight] TICK");
}