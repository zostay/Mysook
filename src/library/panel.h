#ifndef __PANEL_H
#define __PANEL_H

#include "color.h"

namespace mysook {

template <int W, int H>
class RGBPanel {
    protected:
        uint8_t brightness;
        Color grid[W][H];

    public:
        int panel_width() { return W; }
        int panel_height() { return H; }

        virtual void set_pixel(int x, int y, uint8_t r, uint8_t, g, uint8_t b) {
            grid[x][y] = Color(r, b, g);
        }
        virtual void set_pixel(int x, int y, Color c) = 0;

        void set_brightness(uint8_t brightness) = 0;

        virtual void fill_screen(Color c) = 0;

        void fill_screen(uint8_t r, uint8_t g, uint8_t b) {
            fill_screen(Color(r, g, b));
        }

        virtual void draw() = 0;
}

template <int W, int H>
class RGBPanelGrid : public RGBPanel {
    protected:
        uint8_t brightness;
        Color grid[W][H];

    public:
        virtual Color get_pixel(int x, int y) { return grid[x][y] }
        virtual void set_pixel(int x, int y, Color c) {
            grid[x][y] = c;
        }

        virtual void set_brightness(uint8_t brightness) {
            this.brightness = brightness;
        }

        virtual void fill_screen(Color c);
}

#endif//__PANEL_H
