#ifndef __IMAGE_H
#define __IMAGE_H

#ifdef USE_LittleFS
  #define SPIFFS LITTLEFS
  #include <LITTLEFS.h>
#else
  #include <SPIFFS.h>
#endif

typedef coord_t unsigned short
typedef frame_millis_t unsigned short
typedef color_t unsigned char
typedef color_index_t unsigned char

enum ColorFormat {
    CF_Unset,
    CF_RGB,
    CF_Palette,
};

struct Keyframe {
    coord_t x, y;
    frame_millis_t millis;
};

struct Color {
    color_t r, g, b;
}

class Image {
public:
    Image() : w(0), h(0), format(CF_Unset), keyframes(0), palette(0), cbuf(0) {}

private:
    coord_t w, h;
    ColorFormat format;
    Keyframe *keyframes;
    Color *palette;

    union {
        Color *cbuf;
        color_index_t *ibuf;
    };
};

#endif//__IMAGE_H
