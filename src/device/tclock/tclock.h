#ifndef __TCLOCK_H
#define __TCLOCK_H

#include "firmware.h"
#include "panel.h"
#include "rtc.h"

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
    ColroSetting day_color;
}

class ToddlerClock : public Firmware {
    private:
        mysook::RGBPanel<4, 8> *panel;
        mysook::RTC *rtc;

        ToddlerClockConfig config = {
            .morning_time = { .h=7, .m=0 },
            .sleeping_time = { .h=19, .m=30 },
            .night_color = { .r=255, .g=170, .b=0, .brightness=2 },
            .day_color = { .r=0, .g=80, .b=255, .brightness=5 }
        };

        void blank_screen();
        void color_screen(mysook::DateTime &d, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {

    public:
        ToddlerClock(RGBPanel *panel, RTC *rtc);
        ~ToddlerClock();

        void setup();
        void loop();

        int tick_speed() { return 1000; }
}

#endif//__TCLOCK_H
