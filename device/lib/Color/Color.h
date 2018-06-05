#ifndef __COLOR_H
#define __COLOR_H

#include <inttypes.h>

namespace mysook {

class Color {
    public:
        Color() { r = g = b = 0; }
        Color(uint32_t color) {
            r = (color & 0xFF0000) >> 16;
            g = (color & 0x00FF00) >> 8;
            b = (color & 0x0000FF);
        }
        Color(uint8_t r, uint8_t g, uint8_t b) {
            this->r = r;
            this->g = g;
            this->b = b;
        }

        uint8_t r, g, b;

        uint32_t truecolor() {
            return ((uint32_t) r << 16) | ((uint32_t) g << 8) | b;
        }
};

};

#endif//__COLOR_H
