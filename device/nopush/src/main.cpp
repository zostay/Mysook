#include "secrets.h"

#include <Arduino.h>

#include <MC_MDNS.h>
#include <MC_Logger.h>
#include <MC_Network.h>

#include "nopush.h"

#define BUTTON_PIN 16
#define LIGHT_PIN  0

mysook::MC_Logger<Print> logger(micros, &Serial);
mysook::MC_Network network(logger);
mysook::MC_MDNS mdns("nopush-button", logger);

NoPushButton nopush(logger, network, mdns, BUTTON_PIN, LIGHT_PIN);

void setup() {
    Serial.begin(9600);

    mdns.begin();

    for (int i = 0; i < ap_config_count; ++i) {
        logger.logf_ln("I [main] configuring AP: %s", ap_configs[i*2]);
        network.add_access_point(ap_configs[i * 2], ap_configs[i * 2 + 1]);
    }
    network.connect();

    nopush.setup();
}

void loop() {
    nopush.loop();
}
