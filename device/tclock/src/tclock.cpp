#include "tclock.h"

using namespace mysook;

ToddlerClock::ToddlerClock(Logger *log, RGBPanel<4,8> *panel, RTC *rtc) 
: Firmware(log) {
    this->panel = panel;
    this->rtc   = rtc;
}

ToddlerClock::~ToddlerClock() {
    delete panel;
    delete rtc;
}

void ToddlerClock::blank_screen() {
    panel->set_brightness(5);
    panel->fill_screen(0, 0, 0);
    panel->draw();
}

void ToddlerClock::color_screen(DateTime &d, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
    panel->set_brightness(brightness);
    panel->fill_screen(r, g, b);

    int i = 0;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 4; ++x) {
            if (i++ % 2 == d.day() % 2) {
                panel->set_pixel(x, y, 0, 0, 0);
            }
        }
    }

    panel->draw();
}

bool before_time(DateTime &check, TimeSetting &against) {
    return check.hour() < against.h
        || (check.hour() == against.h && check.minute() < against.m);
}

void ToddlerClock::setup() {
    blank_screen();
}

void ToddlerClock::loop() {
    DateTime now = rtc->now();

    logf("%d:%d:%d", now.hour(), now.minute(), now.second());
    if (before_time(now, config.morning_time)) {
        logf_ln(" is early morning.");
        color_screen(now, config.night_color);
    }

    else if (before_time(now, config.sleeping_time)) {
        logf_ln(" is day.");
        color_screen(now, config.day_color);
    }

    else {
        logf_ln(" is night.");
        color_screen(now, config.night_color);
    }
}
