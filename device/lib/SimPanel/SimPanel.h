#ifndef __SIMPANEL_H
#define __SIMPANEL_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <Panel.h>
#include <inttypes.h>
#include <math.h>

#include <cstdio>

#define PIXEL_RADIUS 40
#define PIXEL_DIAMETER (PIXEL_RADIUS*2)
#define PIXEL_SPACE 40
#define PIXEL_SIZE (PIXEL_DIAMETER+PIXEL_SPACE)

#define PIXEL_RADIUS_F 40.0f
#define PIXEL_DIAMETER_F (PIXEL_RADIUS_F*2.0f)
#define PIXEL_SPACE_F 40.0f
#define PIXEL_SIZE_F (PIXEL_DIAMETER_F+PIXEL_SPACE_F)

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
    display = al_create_display(W * PIXEL_SIZE, H * PIXEL_SIZE);
}

template <int W, int H>
void SimPanel<W,H>::draw() {
    al_set_target_backbuffer(display);

    float brightness_coeff = log(this->brightness) / log255;

    uint32_t *buffer = this->grid.getBuffer();
    for (int cy = 0; cy < H; ++cy) {
        for (int cx = 0; cx < W; ++cx) {
            int xy = cy * W + cx;

            uint32_t &bc = buffer[xy];
            uint8_t r, g, b;
            r = (bc >> 0x10) * brightness_coeff;
            g = (bc >> 0x08) * brightness_coeff;
            b = (bc        ) * brightness_coeff;

            ALLEGRO_COLOR c = al_map_rgb(r, g, b);
            al_draw_filled_circle(
                cx*PIXEL_SIZE_F + PIXEL_SPACE_F, 
                cy*PIXEL_SIZE_F + PIXEL_SPACE_F, 
                PIXEL_RADIUS_F, c
            );
        }
    }

    al_flip_display();
}

};

#endif//__SIMPANEL_H
