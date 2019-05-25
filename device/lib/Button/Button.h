#ifndef __BUTTON_H
#define __BUTTON_H

#include <functional>
#include <Firmware.h>

namespace mysook {

typedef std::function<void(bool)> ButtonDispatcher;

class Button : public Ticking {
public:
    Button(int pin, ButtonDispatcher &listener) : pin(pin), listener(listener) { }
    virtual ~Button() { }

    void begin() { pinMode(pin, INPUT_PULLDOWN_16); }

    virtual bool ready_for_tick(unsigned long now) { return true; }
    virtual void tick() {
        int new_button_state = digitalRead(pin);
        if (new_button_state != button_state) {
            // assumes HIGH is pressed
            listener((button_state = new_button_state) == HIGH);
        }   
    }

    int button_pin() { return pin; }

private:
    int pin;
    int button_state = LOW;

    ButtonDispatcher &listener;
};

};

#endif//__BUTTON_H
