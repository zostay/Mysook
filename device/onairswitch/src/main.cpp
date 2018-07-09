#include <WiFiUdp.h>
#include <Wire.h>

#include <LED.h>
#include <MC_Logger.h>
#include <MC_Network.h>

#include "on-air-switch.h"
#include "secrets.h"

#define PIN_CONNECT 2

unsigned int local_udp_port = 10101;
char incoming[500];
bool flipflop_connected = false;

mysook::MC_Logger<Print> logger(&micros, Serial);
mysook::MC_Network network(logger);
mysook::LED status_light(PIN_CONNECT);

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
