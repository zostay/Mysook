#ifndef __LED_H
#define __LED_H

#include <Arduino.h>

#include <Network.h>

namespace mysook {

class LED {
public:
    LED(int pin) : pin(pin) { }

    void begin() { pinMode(pin, OUTPUT); }

    bool state() {
        return digitalRead(pin) == HIGH;
    }

    void on() { digitalWrite(pin, HIGH); }
    void off() { digitalWrite(pin, LOW); }

    bool toggle() {
        bool toggle_to = (digitalRead(pin) == HIGH);
        digitalWrite(pin, toggle_to ? LOW : HIGH);
        return toggle_to;
    }

    int led_pin() { return pin; }

private:
    int pin;
};

enum BLINK_MODE {
    BLINK_ON,
    BLINK_OFF,
    BLINK_BLINK,
};

class BlinkyLED : public LED, public TickingVariableTimer {
public:
    BlinkyLED(int pin) : LED(pin) { }

    virtual unsigned long tick_speed() { return speed; }
    void set_tick_speed(unsigned long speed) { 
        this->speed = speed; 
        this->nudge_tick();
    }

    virtual void tick() { 
        if (blink_mode == BLINK_OFF) {
            this->off();
        }
        else if (blink_mode == BLINK_ON) {
            this->on();
        }
        else if (blink_mode == BLINK_BLINK) {
            this->toggle();
        }
    }

    void set_blink_mode(int mode, bool trigger_tick = true) { 
        blink_mode = mode; 
        if (trigger_tick) tick();
    }

    int get_blink_mode() { return blink_mode; }

private:
    int i = 0;
    unsigned long speed = 1000000ul;
    int blink_mode = BLINK_OFF;
};

class WiFiLED : public BlinkyLED {
public:
    WiFiLED(int pin, mysook::Network &network) : BlinkyLED(pin), network(network) { }

    virtual void tick() {
        if (!was_connected && network.connected()) {
             set_blink_mode(BLINK_ON, false);
        }
        else if (!was_connecting && network.connecting()) {
             set_blink_mode(BLINK_BLINK, false);
        }
        else if ((was_connected  && !network.connected())
             ||  (was_connecting && !network.connecting())) {

             set_blink_mode(BLINK_OFF, false);
        }

        BlinkyLED::tick();
    }

private:
    mysook::Network &network;

    bool was_connecting = false;
    bool was_connected = false;
};

};

#endif//__LED_H
