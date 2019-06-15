#ifndef __NAMETAG_H
#define __NAMETAG_H

#include <functional>

#include <Firmware.h>
#include <Panel.h>
#include <LED.h>
#include <Logger.h>
#include <Network.h>

#include "nametag-fetch.h"
#include "vm.h"

#define PROGRAM_MAIN 1

extern uint32_t default_program[];

class NameTagFlipper : public mysook::TickingVariableTimer {
private:
    unsigned long speed;
    bool flipper = false;

public:
    NameTagFlipper(unsigned long speed) : speed(speed), flipper(flipper) {}

    virtual unsigned long tick_speed() { return speed; }

    virtual void tick() { flipper = true; }

    bool ready() { return flipper; }
    void reset() { flipper = false; }
};

class NameTag : public mysook::Firmware {
private:
    mysook::WiFiLED &led;
    mysook::RGBPanel<32,8> &matrix;
    mysook::Network &network;
    mysook::Logger &log;

    NameTagFlipper flipper;
    NameTagFetch &fetcher;

    VM<32,8> builtin;
    VM<32,8> *vm;

    const uint32_t *web_binary;

    int x = matrix.panel_width();

    bool fetched = false;
    unsigned long last_flip = 0;

public:
    NameTag(mysook::Logger &log, mysook::RGBPanel<32,8> &_matrix, mysook::WiFiLED &_led, mysook::Network &network, NameTagFetch &fetcher) 
    : flipper(60000000UL), Firmware(log), matrix(_matrix), led(_led), network(network), builtin(log, _matrix, default_program, PROGRAM_MAIN), log(log), fetcher(fetcher) { 
        this->add_pre_ticker(&network);
        this->add_pre_ticker(&led);
        
        vm = &builtin;
        this->add_post_ticker(vm);
    }

    virtual unsigned long tick_speed() { return 100000; }

    virtual void start();
    virtual void tick();

    void replace_program(const uint32_t *binary);
    void clear_program();
};

#endif//__NAMETAG_H
