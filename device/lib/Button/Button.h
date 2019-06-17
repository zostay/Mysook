#ifndef __BUTTON_H
#define __BUTTON_H

#include <functional>
#include <Firmware.h>

namespace mysook {

typedef std::function<void(bool, unsigned long)> ButtonDispatcher;

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
            unsigned long press_length = micros() - prev_tick;
            listener((button_state = new_button_state) == HIGH, press_length);

            prev_tick = micros();
        }
    }

    int button_pin() { return pin; }

private:
    int pin;
    int button_state = LOW;
    unsigned long prev_tick = 0;

    ButtonDispatcher &listener;
};

};

#endif//__BUTTON_H
