#ifndef __TRIGGER_H
#define __TRIGGER_H

#include <Firmware.h>
#include <MC_RF24.h>

const char trigger_code[] = { 'U', 'U', 'D', 'D', 'L', 'R', 'L', 'R', 'B', 'A', 'S' };
const char rf_address[][6] = { "1BOOM", "2BOOM" };

class Trigger : public mysook::Firmware {
public:
    Trigger(mysook::Logger &logger, int ce_pin, int csn_pin) 
    : mysook::Firmware(logger), xmit(ce_pin, csn_pin) {
    }

    virtual void start() {
        xmit.begin_writing(rf_address[0]);

        mysook::Message message(sizeof(trigger_code), trigger_code);
        xmit.send(message);

        logf_ln("KONAMI!");
    }

    virtual void tick() { 
        // do nothing
    }

private:
    mysook::MC_RF24_Transmitter xmit;
};

#endif//__TRIGGER_H
