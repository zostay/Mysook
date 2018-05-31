#ifndef __NAMETAG_H
#define __NAMETAG_H

#include <functional>

#include <Firmware.h>
#include <Panel.h>
#include <Logger.h>
#include <Network.h>
#include <WebServer.h>

using std::placeholders::_1;
using std::placeholders::_2;

class NameTag : public mysook::Firmware {
private:
    mysook::RGBPanel<12,6> &matrix;
    mysook::Network &network;
    mysook::WebServer web_server;

    int x = matrix.panel_width();

    mysook::WebDispatcher dispatcher = std::bind(&NameTag::handle_web_request, this, _1, _2);

public:
    NameTag(mysook::Logger &log, mysook::RGBPanel<12,6> &matrix, mysook::Network &network) 
    : Firmware(log), matrix(matrix), network(network), web_server(network, dispatcher, log) { 
        this->add_pre_ticker(&network);
        this->add_pre_ticker(&web_server);
    }

    virtual bool handle_web_request(mysook::Request &req, mysook::Response &res) {
        return false;
    }

    virtual unsigned long tick_speed() { return 100000; }

    virtual void start();
    virtual void tick();
};

#endif//__NAMETAG_H
