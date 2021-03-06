#ifndef __PANEL_H
#define __PANEL_H

#include <Canvas32.h>
#include <Color.h>

namespace mysook {

class Point {
public:
    Point() : x(0), y(0) { }
    Point(const Point &c) : x(c.x), y(c.y) { }
    Point(int x, int y) : x(x), y(y) { }

    int get_x() const { return x; }
    int get_y() const { return y; }

private:
    int x, y;
};

template <int W, int H>
class RGBPanel {
public:
    int panel_width() const { return W; }
    int panel_height() const { return H; }

    virtual Point get_cursor() const { return cursor; }
    virtual void set_cursor(Point p) { cursor = p; }
    virtual void set_cursor(int x, int y) { cursor = Point(x, y); }

    virtual Color get_fg() const { return fg; }
    virtual void set_fg(Color c) { fg = c; }
    virtual void set_fg(uint8_t r, uint8_t g, uint8_t b) { fg = Color(r, g, b); }

    virtual Color get_bg() const { return bg; }
    virtual void set_bg(Color c) { bg = c; }
    virtual void set_bg(uint8_t r, uint8_t g, uint8_t b) { bg = Color(r, g, b); }

    virtual void put_pixel() { put_pixel(cursor.get_x(), cursor.get_y(), fg); }
    virtual void put_pixel_bg() { put_pixel(cursor.get_x(), cursor.get_y(), bg); }
    virtual void put_pixel(int x, int y) { put_pixel(x, y, fg); }
    virtual void put_pixel_bg(int x, int y) { put_pixel(x, y, fg); }
    virtual void put_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
        put_pixel(x, y, Color(r, g, b));
    }
    virtual void put_pixel(int x, int y, Color c) = 0;

    virtual void set_brightness(uint8_t brightness) = 0;

    virtual void fill_screen() { fill_screen(fg); }
    virtual void fill_screen_bg() { fill_screen(bg); }
    virtual void fill_screen(Color c) = 0;
    virtual void fill_screen(uint8_t r, uint8_t g, uint8_t b) {
        fill_screen(Color(r, g, b));
    }

    virtual void put_char(unsigned char c) {
        put_char(c, cursor.get_x(), cursor.get_y(), fg, bg);
    }
    virtual void put_char(unsigned char c, int x, int y) {
        put_char(c, x, y, fg, bg);
    }
    virtual void put_char(unsigned char c, int x, int y, Color fg) {
        put_char(c, x, y, fg, bg);
    }
    virtual void put_char(unsigned char c, int x, int y, Color fg, Color bg) = 0;

    virtual void put_text(const char *t) {
        put_text(t, cursor.get_x(), cursor.get_y(), fg, bg);
    }
    virtual void put_text(const char *t, int x, int y) {
        put_text(t, x, y, fg, bg);
    }
    virtual void put_text(const char *t, int x, int y, Color fg) {
        put_text(t, x, y, fg, bg);
    }
    virtual void put_text(const char *t, int x, int y, Color fg, Color bg) = 0;

    virtual void draw() = 0;

protected:
    Color fg = Color(255, 255, 255);
    Color bg = Color(0, 0, 0);
    Point cursor = Point(0, 0);


};

template <int W, int H>
class RGBPanelGrid : public RGBPanel<W,H> {
protected:
    uint8_t brightness = 255;
    Canvas32 grid;

public:
    using RGBPanel<W,H>::put_pixel;
    using RGBPanel<W,H>::fill_screen;
    using RGBPanel<W,H>::put_char;
    using RGBPanel<W,H>::put_text;

    using RGBPanel<W,H>::set_fg;
    using RGBPanel<W,H>::set_bg;
    using RGBPanel<W,H>::set_brightness;

    RGBPanelGrid() : grid(W, H) { }

    virtual void put_pixel(int x, int y, Color c) {
        grid.setPassthruColor(c.truecolor());
        grid.drawPixel(x, y, 0);
        grid.setPassthruColor();
    }

    virtual void set_brightness(uint8_t brightness) {
        this->brightness = brightness;
    }

    virtual void fill_screen(Color c) {
        grid.setPassthruColor(c.truecolor());
        grid.fillScreen(0);
        grid.setPassthruColor();
    }

    virtual void put_char(unsigned char c, int x, int y, Color fg, Color bg) {
        static char text[2] = { '\0', '\0' };
        text[0] = c;
        put_text(text, x, y, fg, bg);
    }

    virtual void put_text(const char *text, int x, int y, Color fg, Color bg) {
        //grid.setPassthruColor(fg.truecolor());
        grid.setCursor(x, y);
        grid.setTextColor(grid.Color(fg.r, fg.g, fg.b));
        grid.print(text);
        //grid.setPassthruColor();
        this->set_cursor(
            grid.getCursorX(),
            grid.getCursorY()
        );
    }

    uint32_t *get_buffer() { return grid.getBuffer(); }

    virtual void draw() { }
};

template <int W, int H>
class RGBPanelBackBuffer : public RGBPanelGrid<W,H> {
protected:
    RGBPanel<W,H> &panel;

public:
    RGBPanelBackBuffer(RGBPanel<W,H> &panel) : panel(panel) {}

    // Draw = Flip
    virtual void draw() { 
        uint32_t *buf = this->grid.getBuffer();

        int i = 0;
        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                panel.put_pixel(x, y, Color(buf[++i]));
            }
        }

        panel.draw();
    }
};
};

#endif//__PANEL_H
