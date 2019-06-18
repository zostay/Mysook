#ifndef __NOPUSH_H
#define __NOPUSH_H

#include <functional>

#include <WiFiUDP.h>

#include <Button.h>
#include <Firmware.h>
#include <LED.h>
#include <Network.h>
#include <MC_MDNS.h>

using std::placeholders::_1;
using std::placeholders::_2;

class NoPushButton : public mysook::Firmware {
private:
    mysook::ButtonDispatcher dispatcher = std::bind(&NoPushButton::handle_button_press, this, _1, _2);

    void broadcast(const char *message);

public:
    NoPushButton(mysook::Logger &log, mysook::Network &network, mysook::MC_MDNS &mdns, int button_pin, int light_pin) : Firmware(log), network(network), mdns(mdns), button(button_pin, dispatcher), light(light_pin) { 
        add_pre_ticker(&network);
        add_pre_ticker(&button);
        add_post_ticker(&light);
    }
    virtual ~NoPushButton() { }

    virtual void start();
    virtual void tick();

    void broadcast_signal();
    void broadcast_reset();

    virtual unsigned long tick_speed() { return 1000000ul; }

    void handle_button_press(bool pressed, unsigned long length);

private:
    mysook::Network &network;
    mysook::MC_MDNS &mdns;
    mysook::Button button;
    mysook::BlinkyLED light;

    WiFiUDP udp;

    bool was_connecting = false;
    bool was_connected = false;

    bool blinky = false;
};

#endif//__NOPUSH_H
