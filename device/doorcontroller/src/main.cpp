#include <stdint.h>

#include <SPI.h>
#include <LED.h>
#include <MC_MDNS.h>
#include <MC_Network.h>
#include <MC_Logger.h>

#include "controller.h"
#include "gimp_image.h"

#include "secrets.h"

#define BUTTON1PIN 35
#define BUTTON2PIN 0
#define BLINKPIN   27
#define DOORCSPIN  37

extern uint16_t key_frames[];
extern const size_t key_frame_count;
extern const gimp_image_t gimp_image;

mysook::MC_Logger<Print> logger(&micros, Serial);
mysook::MC_Network network(logger);
mysook::MC_MDNS mdns("zostay-door", logger);
mysook::WiFiLED led(BLINKPIN, network);

Controller door_controller(logger, led, network, DOORCSPIN);

void setup() {
    // Start up the serial connection for debugging
    Serial.begin(115200);

    pinMode(BUTTON1PIN, INPUT);
    pinMode(BUTTON2PIN, INPUT);
    pinMode(BLINKPIN, OUTPUT);
    pinMode(DOORCSPIN, OUTPUT);

    digitalWrite(DOORCSPIN, HIGH);

    for (int i = 0; i < ap_config_count; ++i) {
        network.add_access_point(ap_configs[i*2], ap_configs[i*2+1]);
    }
    network.connect();

    mdns.add_service("dooranimator", "tcp", 10101);

    door_controller.setup();

    door_controller.load_bmp(gimp_image.pixel_data, gimp_image.width * gimp_image.height);
    door_controller.load_keyframes(key_frames, key_frame_count);

    Serial.println("Setup done");
}

void loop() {
    door_controller.loop();
}
