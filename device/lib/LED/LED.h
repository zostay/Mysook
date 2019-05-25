#ifndef __LED_H
#define __LED_H

#include <Arduino.h>

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

    void set_blink_mode(int mode) { 
        blink_mode = mode; 
        tick();
    }

    int get_blink_mode() { return blink_mode; }

private:
    int i = 0;
    int speed = 1000000;
    int blink_mode = BLINK_OFF;
};

};

#endif//__LED_H
