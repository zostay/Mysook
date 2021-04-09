#include "igniter.h"
#include "secrets.h"

#include <MC_Logger.h>
#include <MC_Network.h>

#include <Arduino.h>

#define LIGHT_PIN D4
#define RELAY_PIN D1

#define RF24_CE   D6
#define RF24_CSN  D7

mysook::MC_Logger<Print> logger(micros, &Serial);
mysook::MC_Network network(logger);
Igniter igniter(logger, LIGHT_PIN, RELAY_PIN, RF24_CE, RF24_CSN);

void setup() {
    Serial.begin(9600);
    
    logger.logf_ln("Start");

    for (int i = 0; i < ap_config_count; ++i) {
        logger.logf_ln("I [main] configuring AP: %s", ap_configs[i*2]);
        network.add_access_point(ap_configs[i * 2], ap_configs[i * 2 + 1]);
    }
    network.connect();
    network.enable_ota("norbert01");

    igniter.setup();
}

void loop() {
    igniter.loop();
}
