#ifndef __COLOR_H
#define __COLOR_H

#include <Arduino.h>

namespace mysook {

class Color {
    public:
        Color() { r = g = b = 0u; }
        Color(uint8_t r, uint8_t g, uint8_t b) {
            this->r = r;
            this->g = g;
            this->b = b;
        }

        uint8_t r, g, b;
};

};

#endif//__COLOR_H
