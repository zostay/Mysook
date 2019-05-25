#ifndef __NOPUSH_H
#define __NOPUSH_H

#include <functional>

#include <WiFiUDP.h>

#include <Button.h>
#include <Firmware.h>
#include <LED.h>
#include <Network.h>

using std::placeholders::_1;

class NoPushButton : public mysook::Firmware {
private:
    mysook::ButtonDispatcher dispatcher = std::bind(&NoPushButton::handle_button_press, this, _1);

public:
    NoPushButton(mysook::Logger &log, mysook::Network &network, int button_pin, int light_pin) : Firmware(log), network(network), button(button_pin, dispatcher), light(light_pin) { 
        add_pre_ticker(&network);
        add_pre_ticker(&button);
        add_post_ticker(&light);
    }
    virtual ~NoPushButton() { }

    virtual void start();
    virtual void tick();

    void broadcast_signal();

    virtual unsigned long tick_speed() { return 1000000ul; }

    void handle_button_press(bool pressed);

private:
    mysook::Network &network;
    mysook::Button button;
    mysook::BlinkyLED light;

    WiFiUDP udp;

    bool was_connecting = false;
    bool was_connected = false;
};

#endif//__NOPUSH_H
