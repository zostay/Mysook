#include "tclock.h"

using namespace mysook;

ToddlerClock::ToddlerClock(Logger &log, RGBPanel<LIGHT_WIDTH,LIGHT_HEIGHT> *panel, Network &network, RTC *rtc, int zostayify_port) 
: Firmware(log), network(network), zostayify(network, zostayify_port, dispatcher, log) {
    this->panel     = panel;
    this->rtc       = rtc;

    this->configger = new TClockConfig(log, network, config, rtc);

    this->add_pre_ticker(&zostayify);
    this->add_pre_ticker(&network);
    this->add_pre_ticker(configger);
}

ToddlerClock::~ToddlerClock() { 
    delete configger;
}

void ToddlerClock::blank_screen() {
    panel->set_brightness(20);
    panel->fill_screen(0, 0, 0);
    panel->draw();
}

void ToddlerClock::color_screen(DateTime &d, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
    panel->set_brightness(brightness);

    int stagger = d.day() % 2;
    for (int x = 0; x < panel->panel_width(); ++x) {
        for (int y = 0; y < panel->panel_height(); ++y) {
            if ((x + y + stagger) % 2 == 0) {
                //logf_ln("Set pixel (%d, %d) to (#%02x%02x%02x)", x, y, r, g, b);
                panel->put_pixel(x, y, r, g, b);
            }
            else {
                //logf_ln("Set pixel (%d, %d) to (#%02x%02x%02x)", x, y, 0, 0, 0);
                panel->put_pixel(x, y, 0, 0, 0);
            }
        }
    }

    panel->draw();
}

bool ToddlerClock::before_time(DateTime &check, TimeSetting &against) {
    return check.hour() < against.h
        || (check.hour() == against.h && check.minute() < against.m);
}

bool ToddlerClock::before_transition_time(DateTime &check, TimeSetting &against) {
    TimeSetting t;

    t.h = against.h;
    t.m = against.m - 1;

    if (t.m < 0) {
        t.m = 59;
        t.h -= 1;
    }

    if (t.h < 0) {
        t.h = 23;
    }

    // logf_ln("Checking if %02d:%02d < %02d:%02d",
    //         check.hour(), check.minute(),
    //         t.h, t.m);

    return before_time(check, t);
}

ColorSetting ToddlerClock::make_transition_color(DateTime &now, ColorSetting &from, ColorSetting &to) {
    ColorSetting c;
    int s = now.second();

    c.r = ((from.r * (60 - s)) + (to.r * s)) / 60;
    c.g = ((from.g * (60 - s)) + (to.g * s)) / 60;
    c.b = ((from.b * (60 - s)) + (to.b * s)) / 60;
    c.brightness = ((from.brightness * (60 - 2)) + ((to.brightness * s))) / 60;
    if (c.brightness < 2u) c.brightness = 2u;

    return c;
}

void ToddlerClock::start() {
    zostayify.begin();
    blank_screen();
}

void ToddlerClock::tick() {
    DateTime now = rtc->now();

    this->logf("%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    if (before_transition_time(now, config.morning_time)) {
        logf_ln(" is early morning.");
        color_screen(now, config.night_color);
    }

    else if(before_time(now, config.morning_time)) {
        logf_ln(" is becoming day.");
        ColorSetting c = make_transition_color(now, config.night_color, config.day_color);
        color_screen(now, c);
    }

    else if (before_transition_time(now, config.sleeping_time)) {
        logf_ln(" is day.");
        color_screen(now, config.day_color);
    }

    else if (before_time(now, config.sleeping_time)) {
        logf_ln(" is becoming night.");
        ColorSetting c = make_transition_color(now, config.day_color, config.night_color);
        color_screen(now, c);
    }

    else {
        logf_ln(" is night.");
        color_screen(now, config.night_color);
    }
}

void ToddlerClock::handle_zostayification(String remote_ip, uint16_t remote_port, const char *buf, size_t len) {
    configger->ping();
}
