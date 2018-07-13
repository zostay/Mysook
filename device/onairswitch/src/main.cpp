#ifdef ARDUINO
#include <Wire.h>

#include <LED.h>
#include <MC_Logger.h>
#include <MC_Network.h>
#else//ARDUINO
#include <SimWire.h>

#include <SimLED.h>
#include <SimLogger.h>
#include <Network.h>
#endif//ARDUINO

#include "on-air-switch.h"
#include "secrets.h"

#define PIN_CONNECT 2

#ifdef ARDUINO
mysook::MC_Logger<Print> logger(&micros, Serial);
mysook::MC_Network network(logger);
mysook::LED status_light(PIN_CONNECT);
#else//ARDUIONO
mysook::SimLogger logger;
mysook::Network network;
mysook::SimLED status_light;
#endif//ARDUINO

OnAirSwitch on_air_switch(logger, network, status_light);

void setup() {
    Wire.begin();
    Serial.begin(115200);

    network.add_access_point(AP_SSI, AP_PASSWORD);
    network.connect();

    on_air_switch.setup();
}

void loop() {
    on_air_switch.loop();
}
