#ifndef __PANEL_H
#define __PANEL_H

#include <Color.h>

namespace mysook {

template <int W, int H>
class RGBPanel {
public:
    int panel_width() { return W; }
    int panel_height() { return H; }

    virtual void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
        set_pixel(x, y, Color(r, g, b));
    }
    virtual void set_pixel(int x, int y, Color c) = 0;

    virtual void set_brightness(uint8_t brightness) = 0;

    virtual void fill_screen(Color c) = 0;

    void fill_screen(uint8_t r, uint8_t g, uint8_t b) {
        fill_screen(Color(r, g, b));
    }

    virtual void draw() = 0;
};

template <int W, int H>
class RGBPanelGrid : public RGBPanel<W,H> {
protected:
    uint8_t brightness = 255;
    Color grid[W][H];

public:
    virtual Color get_pixel(int x, int y) { return grid[x][y]; }
    virtual void set_pixel(int x, int y, Color c) {
        grid[x][y] = c;
    }

    virtual void set_brightness(uint8_t brightness) {
        this->brightness = brightness;
    }

    virtual void fill_screen(Color c);
};

template <int W, int H> 
void RGBPanelGrid<W,H>::fill_screen(Color c) {
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            grid[x][y] = c;
        }
    }
}

};

#endif//__PANEL_H
