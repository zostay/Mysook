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

    bool on() { digitalWrite(pin, HIGH); }
    bool off() { digitalWrite(pin, LOW); }

    bool toggle() {
        digitalWrite(pin, digitalRead(pin) == HIGH ? LOW : HIGH);
    }

private:
    int pin;
};

};

#endif//__LED_H
