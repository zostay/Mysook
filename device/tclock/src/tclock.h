#ifndef __TCLOCK_H
#define __TCLOCK_H

#include <functional>

#include "config.h"
#include "tclock_config.h"

#include <Firmware.h>
#include <Logger.h>
#include <Network.h>
#include <Panel.h>
#include <RTC.h>
#include <MC_UDP.h>

#ifdef NAMETAG
#define LIGHT_WIDTH 6
#define LIGHT_HEIGHT 12
#else
#define LIGHT_WIDTH 4
#define LIGHT_HEIGHT 8
#endif

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

class ToddlerClock : public mysook::Firmware {
private:
    TClockConfig *configger;
    mysook::UdpListener zostayify;

    mysook::UdpDispatcher dispatcher = std::bind(&ToddlerClock::handle_zostayification, this, _1, _2, _3, _4);

    mysook::RGBPanel<LIGHT_WIDTH, LIGHT_HEIGHT> *panel;
    mysook::Network &network;
    mysook::RTC *rtc;

    ToddlerClockConfig config = {
        .morning_time = { .h=7, .m=10 },
        .sleeping_time = { .h=19, .m=0 },
        .night_color = { .r=255u, .g=170u, .b=0u, .brightness=2u },
        .day_color = { .r=0u, .g=80u, .b=255u, .brightness=20u }
    };

    void blank_screen();
    void color_screen(mysook::DateTime &d, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);
    void color_screen(mysook::DateTime &d, ColorSetting &c) {
        color_screen(d, c.r, c.g, c.b, c.brightness);
    }

    bool before_time(mysook::DateTime &check, TimeSetting &against);
    bool before_transition_time(mysook::DateTime &check, TimeSetting &against);
    ColorSetting make_transition_color(mysook::DateTime &now, ColorSetting &from, ColorSetting &to);

    bool is_daytime();

public:
    ToddlerClock(mysook::Logger &log, mysook::RGBPanel<LIGHT_WIDTH,LIGHT_HEIGHT> *panel, mysook::Network &network, mysook::RTC *rtc, int zostayify_port);
    virtual ~ToddlerClock();

    virtual void start();
    virtual void tick();

    virtual unsigned long tick_speed() { return 1000000ul; }

    void handle_zostayification(String remote_ip, uint16_t remote_port, const char *buf, size_t len);

    void go_crazy();
};

#endif//__TCLOCK_H
