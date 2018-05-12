#ifndef __ARDUINO_PANEL_H
#define __ARDUINO_PANEL_H

namespace mysook {

template <int W, int H> 
class ArduinoRGBPanel : public RGBPanel<W, H> {
    protected:
        Adafruit_NeoMatrix *panel;

    public:
        virtual void set_pixel(int x, int y, Color c) {
            panel->drawPixel(x, y, panel->Color(c.r, c.g, c.b));
        }

        virtual void set_brightness(uint8_t brightness) {
            panel->setBrightness(brightness);
        }

        virtual void fill_screen(Color c) {
            panel->fillScreen(panel->Color(c.r, c.g, c.b));
        }

        virtual void draw() {
            panel->draw();
        }
}

#endif//__ARDUINO_PANEL_H
