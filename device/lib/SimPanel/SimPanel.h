#ifndef __SIMPANEL_H
#define __SIMPANEL_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <Panel.h>
#include <inttypes.h>
#include <math.h>

#include <cstdio>

namespace mysook {

template <int W, int H>
class SimPanel : public RGBPanelGrid<W,H> {
public:
    SimPanel() { }
    ~SimPanel() { if (display) al_destroy_display(display); }

    void initialize();

    virtual void draw();

protected:
    ALLEGRO_DISPLAY *display = 0;
};

const float log255 = log(255.0);

template <int W, int H>
void SimPanel<W,H>::initialize() {
    display = al_create_display(W * 80, H * 80);
}

template <int W, int H>
void SimPanel<W,H>::draw() {
    al_set_target_backbuffer(display);

    float brightness_coeff = log(this->brightness) / log255;

    for (int cy = 0; cy < H; ++cy) {
        for (int cx = 0; cx < W; ++cx) {
            uint8_t r, g, b;
            r = this->grid[cx][cy].r * brightness_coeff;
            g = this->grid[cx][cy].g * brightness_coeff;
            b = this->grid[cx][cy].b * brightness_coeff;

            ALLEGRO_COLOR c = al_map_rgb(r, g, b);
            al_draw_filled_circle(cx*160.0f + 80.0f, cy*160.0f + 80.0f, 40.0f, c);
        }
    }

    al_flip_display();
}

};

#endif//__SIMPANEL_H
