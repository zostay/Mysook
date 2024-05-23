#ifndef __DOORLIGHT_H
#define __DOORLIGHT_H

#include <Firmware.h>
#include <Logger.h>
#include <Animation.h>

template <class F>
class Doorlight : public mysook::Firmware {
    mysook::Animation<F> &animation;
    mysook::Logger &log;

public:
    Doorlight(mysook::Logger &log, mysook::Animation<F> &animation)
    : Firmware(log), animation(animation), log(log) {
        this->add_pre_ticker(&animation);
    }

    virtual unsigned long tick_speed() { return 100000; }

    virtual void start();
    virtual void tick();
};

#endif//__DOORLIGHT_H
