#ifndef __ARDUINO_PANEL_H
#define __ARDUINO_PANEL_H

#include <Panel.h>

namespace mysook {

template <int W, int H, class M> 
class MC_RGBPanel : public RGBPanel<W, H> {
    protected:
        M &panel;

    public:
        MC_RGBPanel(M &panel) : panel(panel) { }

        virtual void put_pixel(int x, int y, Color c) {
            panel.drawPixel(x, y, panel.Color(c.r, c.g, c.b));
        }

        virtual void set_brightness(uint8_t brightness) {
            panel.setBrightness(brightness);
        }

        virtual void fill_screen(Color c) {
            panel.fillScreen(panel.Color(c.r, c.g, c.b));
        }

        virtual void put_char(unsigned char c, int x, int y, Color fg, Color bg) {
            char text[2] = { c, '\0' };
            put_text(text, x, y, fg, bg);
        }

        virtual void put_text(const char *text, int x, int y, Color fg, Color bg) {
            panel.setCursor(x, y);
            panel.setTextColor(
                panel.Color(fg.r, fg.g, fg.b),
                panel.Color(bg.r, bg.g, bg.b)
            );
            panel.print(text);
            this->set_cursor(
                panel.getCursorX(),
                panel.getCursorY()
            );
        }

        virtual void draw() {
            panel.show();
        }
};

};

#endif//__ARDUINO_PANEL_H
