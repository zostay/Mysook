#ifndef __NAMETAG_H
#define __NAMETAG_H

#include <Firmware.h>
#include <Panel.h>
#include <Logger.h>
#include <Network.h>

class NameTag : public mysook::Firmware {
private:
    mysook::RGBPanel<12,6> &matrix;
    mysook::Network &network;

    int x = matrix.panel_width();

public:
    NameTag(mysook::Logger &log, mysook::RGBPanel<12,6> &matrix, mysook::Network &network) 
    : Firmware(log), matrix(matrix), network(network) { 
        this->add_pre_ticker(&network);
    }

    virtual unsigned long tick_speed() { return 100000; }

    virtual void start();
    virtual void tick();
};

#endif//__NAMETAG_H
