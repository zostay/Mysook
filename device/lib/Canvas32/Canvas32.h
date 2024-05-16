#ifndef __CANVAS32_H
#define __CANVAS32_H

#if defined(ARDUINO) || defined(ESP_PLATFORM)
#include <Adafruit_GFX.h>
#else
#include <Sim_Adafruit_GFX.h>
#endif

class Canvas32 : public Adafruit_GFX {
public:
    Canvas32(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) { 
        buffer = new uint32_t[w * h]; 
    }
    ~Canvas32() { delete[] buffer; }

    void drawPixel(int16_t x, int16_t y, uint16_t color);
    uint32_t *getBuffer() { return buffer; };

    void setPassthruColor(uint32_t color) { passthru = color; passthru_flag = true; }
    void setPassthruColor() { passthru_flag = false; }

    uint16_t Color(uint8_t r, uint8_t g, uint8_t b) {
        return ((uint16_t)(r & 0xF8) << 8) |
               ((uint16_t)(g & 0xFC) << 3) |
                          (b         >> 3);
    }

private:
    uint32_t *buffer;
    uint32_t passthru;
    bool passthru_flag;
};

#endif//__CANVAS32_H
