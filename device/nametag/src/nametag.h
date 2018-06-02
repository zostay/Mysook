#ifndef __NAMETAG_H
#define __NAMETAG_H

#include <functional>

#include <Firmware.h>
#include <Panel.h>
#include <Logger.h>
#include <Network.h>
#include <WebServer.h>

#include "vm.h"

using std::placeholders::_1;
using std::placeholders::_2;

#define PROGRAM_MAIN 1
#define PROGRAM_PILES_INIT 2
#define PROGRAM_PILES 3
#define PROGRAM_TOPPLE 4
#define SKIP_RAISE_X_MINUS_1 5
#define SKIP_RAISE_Y_MINUS_1 6
#define SKIP_RAISE_X_PLUS_1 7
#define SKIP_RAISE_Y_PLUS_1 8
#define LOOP_END_PILES_INIT 9
#define LOOP_START_PILES_Y 10
#define LOOP_START_PILES_X 11
#define LOOP_END_PILES_X 12
#define LOOP_END_PILES_Y 13
#define LOOP_START_PILES_INIT 14
#define SKIP_BLACK 15
#define SKIP_BLUE 16
#define SKIP_YELLOW 17
#define SKIP_RED 18

extern uint32_t basic_program[];

class NameTag : public mysook::Firmware {
private:
    mysook::RGBPanel<12,6> &matrix;
    mysook::Network &network;
    mysook::WebServer web_server;

    VM<12,6> vm;

    int x = matrix.panel_width();

    mysook::WebDispatcher dispatcher = std::bind(&NameTag::handle_web_request, this, _1, _2);

public:
    NameTag(mysook::Logger &log, mysook::RGBPanel<12,6> &_matrix, mysook::Network &network) 
    : Firmware(log), matrix(_matrix), network(network), web_server(network, dispatcher, log), vm(log, _matrix, basic_program, PROGRAM_MAIN) { 
        this->add_pre_ticker(&network);
        this->add_pre_ticker(&web_server);
        this->add_post_ticker(&vm);
    }

    virtual bool handle_web_request(mysook::Request &req, mysook::Response &res) {
        return false;
    }

    virtual unsigned long tick_speed() { return 100000; }

    virtual void start();
    virtual void tick();
};

#endif//__NAMETAG_H
