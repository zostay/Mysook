#ifndef __LED_H
#define __LED_H

#include <Arduino.h>

namespace mysook {

class LED {
public:
    LED(int pin) : pin(pin) { 
        pinMode(pin, OUTPUT);
    }

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

private:
    int pin;
};

};

#endif//__LED_H
