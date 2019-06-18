#ifndef __NAMETAG_H
#define __NAMETAG_H

#include <functional>

#include <Firmware.h>
#include <Panel.h>
#include <LED.h>
#include <Logger.h>
#include <Network.h>
#include <MC_UDP.h>

#include "nametag-fetch.h"
#include "vm.h"

#define ZOSTAYIFY_PORT 10101

extern uint32_t PROGRAM_MAIN;
extern uint32_t default_program[];

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

enum FlipperSignal {
    FS_NONE,
    FS_FLIP,
    FS_RESET
};

class NameTagFlipper : public mysook::TickingVariableTimer {
private:
    unsigned long speed;
    FlipperSignal flipper = FS_NONE;

    mysook::Logger &log;

public:
    NameTagFlipper(unsigned long speed, mysook::Network &network, mysook::Logger &logger) : speed(speed), log(logger), flipper(flipper)  {}

    virtual unsigned long tick_speed() { return speed; }

    virtual void tick() { flipper = FS_FLIP; }

    bool ready() { return flipper != FS_NONE; }
    FlipperSignal signal() { return flipper; }
    void clear() { flipper = FS_NONE; }

    void handle_zostayification(String remote_ip, uint16_t remote_port, const char *buf, size_t len);
};

class NameTag : public mysook::Firmware {
private:
    mysook::WiFiLED &led;
    mysook::RGBPanel<32,8> &matrix;
    mysook::Network &network;
    mysook::Logger &log;
    mysook::UdpListener zostayify;

    NameTagFlipper flipper;
    NameTagFetch &fetcher;

    VM<32,8> builtin;
    VM<32,8> *vm;

    const WebProgramInfo *web_binary;

    int x = matrix.panel_width();

    bool fetched = false;
    unsigned long last_flip = 0;

    mysook::UdpDispatcher dispatcher = std::bind(&NameTag::handle_zostayification, this, _1, _2, _3, _4);

public:
    NameTag(mysook::Logger &log, mysook::RGBPanel<32,8> &_matrix, mysook::WiFiLED &_led, mysook::Network &network, NameTagFetch &fetcher) 
    : flipper(60000000UL, network, log), Firmware(log), matrix(_matrix), led(_led), network(network), builtin(log, _matrix, default_program, PROGRAM_MAIN), log(log), fetcher(fetcher), zostayify(network, ZOSTAYIFY_PORT, dispatcher, log)  { 
        this->add_pre_ticker(&network);
        this->add_pre_ticker(&led);
        this->add_pre_ticker(&zostayify);

        zostayify.begin();
        
        vm = &builtin;
        this->add_post_ticker(vm);
    }

    void handle_zostayification(String remote_ip, uint16_t remote_port, const char *buf, size_t len);

    virtual unsigned long tick_speed() { return 100000; }

    virtual void start();
    virtual void tick();

    void replace_program(const WebProgramInfo *binary);
    void clear_program();
};

#endif//__NAMETAG_H
