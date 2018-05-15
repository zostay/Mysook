#ifndef __TCLOCK_H
#define __TCLOCK_H

#include <Firmware.h>
#include <Logger.h>
#include <Panel.h>
#include <RTC.h>

struct TimeSetting {
  uint8_t h, m;
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
            .morning_time = { .h=7u, .m=0u },
            .sleeping_time = { .h=19u, .m=30u },
            .night_color = { .r=255u, .g=170u, .b=0u, .brightness=2u },
            .day_color = { .r=0u, .g=80u, .b=255u, .brightness=5u }
        };

        void blank_screen();
        void color_screen(DateTime &d, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);
        void color_screen(DateTime &d, ColorSetting &c) {
            color_screen(d, c.r, c.g, c.b, c.brightness);
        }

    public:
        ToddlerClock(mysook::Logger *log, mysook::RGBPanel<4,8> *panel, mysook::RTC *rtc);
        ~ToddlerClock();

        void setup();
        void loop();

        int tick_speed() { return 1000; }
};

#endif//__TCLOCK_H
