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
    else if ((was_connected && !network.connected())
            || (was_connecting && !network.connecting())) {

        light.set_blink_mode(mysook::BLINK_OFF);
    }
    else if (blinky) {
        blinky = false;
        light.set_tick_speed(1000000ul);
        light.set_blink_mode(mysook::BLINK_ON);
    }

    was_connected  = network.connected();
    was_connecting = network.connecting();
}

#define LONG_PRESS_TIME 2000000ul

void NoPushButton::handle_button_press(bool pressed, unsigned long length) {
    if (pressed) logf_ln("I [nopush] Button pushed");
    if (!network.connected()) return;
    if (pressed) {
        light.set_blink_mode(mysook::BLINK_OFF);
        broadcast_signal();
    }
    else {
        light.set_blink_mode(mysook::BLINK_ON);

        logf_ln("I [nopush] Button release after %lu", length);
        if (length > LONG_PRESS_TIME) {
            broadcast_reset();
            light.set_tick_speed(50000ul);
            light.set_blink_mode(mysook::BLINK_BLINK);
            blinky = true;
        }
    }
}

void NoPushButton::broadcast_signal() {
    logf_ln("I [nopush] Broadcast signal");
    IPAddress broadcast(255, 255, 255, 255);
    udp.beginPacket(broadcast, ZOSTAYIFICATION_PORT);
    udp.write("DONT PUSH THE BUTTON!");
    udp.endPacket();
}

void NoPushButton::broadcast_reset() {
    logf_ln("I [nopush] Broadcast reset");
    IPAddress broadcast(255, 255, 255, 255);
    udp.beginPacket(broadcast, ZOSTAYIFICATION_PORT);
    udp.write("RESET");
    udp.endPacket();
}
