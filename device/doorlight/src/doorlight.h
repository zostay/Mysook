#ifndef __DOORLIGHT_H
#define __DOORLIGHT_H

#include <Firmware.h>
#include <Logger.h>
#include <Animation.h>

template <int W, int H>
class Doorlight : public mysook::Firmware {
    mysook::Animator<W, H> &animator;
    mysook::Logger &log;

public:
    Doorlight(mysook::Logger &log, mysook::Animator<W, H> &animator)
    : Firmware(log), animator(animator), log(log) {
        this->add_pre_ticker(&animator);
    }

    virtual unsigned long tick_speed() { return 100000; }

    virtual void start() {}
    virtual void tick() {}
};

#endif//__DOORLIGHT_H
