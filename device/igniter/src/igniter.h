#ifndef __IGNITER_H
#define __IGNITER_H

#include <functional>
#include <Firmware.h>
#include <LED.h>
#include <MC_RF24.h>


using std::placeholders::_1;

const char trigger_code[] = { 'U', 'U', 'D', 'D', 'L', 'R', 'L', 'R', 'B', 'A', 'S' };
const char rf_address[][6] = { "1BOOM", "2BOOM" };

class Igniter : public mysook::Firmware {
private:
    mysook::MessageDispatcher trigger = std::bind(&Igniter::trigger_igniter, this, _1);

public:
    Igniter(mysook::Logger &log, int light_pin, int relay_pin, int ce_pin, int csn_pin) 
    : mysook::Firmware(log), light(light_pin), relay(relay_pin), 
    recv(trigger, ce_pin, csn_pin) {
        light.begin();
        relay.begin();

        add_pre_ticker(&light);
    }
    virtual ~Igniter() { }

    virtual void start() { 
        recv.begin_reading(rf_address[0]);
        light.set_blink_mode(mysook::BLINK_BLINK);
    }

    virtual unsigned long tick_speed() { return 200000UL; }

    virtual void tick() { 
        if (fire) {
            relay.on();
            fire = false;
        }
        else {
            relay.off();
        }

        //logf_ln("Tick %d", digitalRead(2));
        //light.toggle();
    }

    void trigger_igniter(const mysook::Message &m) {
        const char *data = m.data();
        for (unsigned int i = 0; i < sizeof(trigger_code); ++i)
            if (data[i] != trigger_code[i])
                return;

        fire = true;        
    }

private:
    mysook::BlinkyLED light;
    mysook::LED relay;
    mysook::MC_RF24_Receiver recv;

    bool fire = false;
};

#endif//__IGNITER_H
