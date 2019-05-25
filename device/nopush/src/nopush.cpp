#include "nopush.h"

#define ZOSTAYIFICATION_PORT 10101

void NoPushButton::start() {
    //udp.begin();

    button.begin();
    light.begin();
}

void NoPushButton::tick() {
    if (!was_connected && network.connected()) {
        light.set_blink_mode(mysook::BLINK_ON);
    }
    else if (!was_connecting && network.connecting()) {
        light.set_blink_mode(mysook::BLINK_BLINK);
    }
    else if (was_connected && !network.connected()
            || was_connecting && !network.connecting()) {

        light.set_blink_mode(mysook::BLINK_OFF);
    }

    was_connected  = network.connected();
    was_connecting = network.connecting();
}

void NoPushButton::handle_button_press(bool pressed) {
    if (pressed) logf_ln("I [nopush] Button pushed");
    if (!network.connected()) return;
    if (pressed) {
        light.set_blink_mode(mysook::BLINK_OFF);
        broadcast_signal();
    }
    else {
        light.set_blink_mode(mysook::BLINK_ON);
    }
}

void NoPushButton::broadcast_signal() {
    logf_ln("I [nopush] Broadcast signal");
    IPAddress broadcast(255, 255, 255, 255);
    udp.beginPacket(broadcast, ZOSTAYIFICATION_PORT);
    udp.write("DONT PUSH THE BUTTON!");
    udp.endPacket();
}
