#include <WiFiUdp.h>
#include <Wire.h>

#include <queue>
#include <MC_Network.h>

#include "secrets.h"

#define BRIGHTNESS  0x01
#define PIXEL       0x02

#define GRID_ADDRESS 42

#define HEAD_SENTINEL 0xBEEF
#define TAIL_SENTINEL 0xDEAD
#define MESSAGE_SIZE 197

#define PIN_CONNECT 2

WiFiUDP udp;
unsigned int local_udp_port = 10101;
char incoming[500];
bool flipflop_connected = false;
std::queue<GridMessage> mq;

mysook::MC_Logger<Print> logger(&micros, Serial);
mysook::MC_Network network(logger);
mysook::LED status_light(PIN_CONNECT);
mysook::MessageBus bus;

void setup() {
    Wire.begin();
    Serial.begin(115200);

    network.add_access_point(ap_ssi, ap_password);
    network.connect();

    on_air_switch.setup();
}

void loop() {
    on_air_switch.loop();
}
