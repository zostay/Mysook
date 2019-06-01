#ifndef __NAMETAG_H
#define __NAMETAG_H

#include <functional>

#include <Firmware.h>
#include <Panel.h>
#include <LED.h>
#include <Logger.h>
#include <Network.h>

#include "vm.h"

#define PROGRAM_MAIN 1

extern uint32_t default_program[];

class NameTag : public mysook::Firmware {
private:
    mysook::WiFiLED &led;
    mysook::RGBPanel<32,8> &matrix;
    mysook::Network &network;
    mysook::Logger &log;

    VM<32,8> builtin;
    VM<32,8> *vm;

    uint32_t *web_binary;

    int x = matrix.panel_width();

public:
    NameTag(mysook::Logger &log, mysook::RGBPanel<32,8> &_matrix, mysook::WiFiLED &_led, mysook::Network &network) 
    : Firmware(log), matrix(_matrix), led(_led), network(network), builtin(log, _matrix, default_program, PROGRAM_MAIN), log(log) { 
        this->add_pre_ticker(&network);
        this->add_pre_ticker(&led);
        
        vm = &builtin;
        this->add_post_ticker(vm);
    }

    virtual unsigned long tick_speed() { return 100000; }

    virtual void start();
    virtual void tick();
};

#endif//__NAMETAG_H
