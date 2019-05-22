#ifndef __CONFIG_H
#define __CONFIG_H

#include <Arduino.h>

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

#endif//__CONFIG_H
