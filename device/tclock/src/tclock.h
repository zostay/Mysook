#ifndef __TCLOCK_H
#define __TCLOCK_H

#include <Firmware.h>
#include <Logger.h>
#include <Panel.h>
#include <RTC.h>

struct TimeSetting {
  int8_t h, m;
};

struct ColorSetting {
  uint8_t r, g, b;
  uint8_t brightness;
};

struct ToddlerClockConfig {
    TimeSetting morning_time;
    TimeSetting sleeping_time;
    ColorSetting night_color;
    ColorSetting day_color;
};

class ToddlerClock : public mysook::Firmware {
    private:
        mysook::RGBPanel<4, 8> *panel;
        mysook::RTC *rtc;

        ToddlerClockConfig config = {
            .morning_time = { .h=7, .m=2 },
            .sleeping_time = { .h=19, .m=0 },
            .night_color = { .r=255u, .g=170u, .b=0u, .brightness=2u },
            .day_color = { .r=0u, .g=80u, .b=255u, .brightness=5u }
        };

        void blank_screen();
        void color_screen(DateTime &d, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);
        void color_screen(DateTime &d, ColorSetting &c) {
            color_screen(d, c.r, c.g, c.b, c.brightness);
        }

        bool before_time(DateTime &check, TimeSetting &against);
        bool before_transition_time(DateTime &check, TimeSetting &against);
        ColorSetting make_transition_color(DateTime &now, ColorSetting &from, ColorSetting &to);

    public:
        ToddlerClock(mysook::Logger *log, mysook::RGBPanel<4,8> *panel, mysook::RTC *rtc);
        ~ToddlerClock();

        virtual void start();
        virtual void tick();

        virtual unsigned long tick_speed() { return 1000000ul; }
};

#endif//__TCLOCK_H
