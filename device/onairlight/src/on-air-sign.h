#ifndef __ON_AIR_SIGN_H
#define __ON_AIR_SIGN_H

#include <Firmware.h>
#include <Panel.h>
#include <Logger.h>

#define BRIGHTNESS 0x01
#define PIXEL      0x02
#define DRAW       0x03

#define BRIGHTNESS_SIZE 2
#define PIXEL_SIZE      6
#define DRAW_SIZE       1

class OnAirSign : public mysook::Firmware {
public:
    OnAirSign(mysook::Logger &logger, mysook::RGBPanel<8,8> &display) 
        : Firmware(logger), _display(display) {}

    void send(uint8_t op) {
        switch (op) {
        case BRIGHTNESS:
            if (Wire.available() >= BRIGHTNESS_SIZE) {
                Wire.read(); // consume the op byte
                _display.set_brightness(Wire.read());
            }
            break;

        case PIXEL:
            if (Wire.available() >= PIXEL_SIZE) {
                Wire.read(); // consume the op byte
                _display.put_pixel(
                    Wire.read(),
                    Wire.read(),
                    Wire.read(),
                    Wire.read(),
                    Wire.read()
                );
            }
            break;

        case DRAW:
            if (Wire.available >= DRAW_SIZE) {
                Wire.read();
                _display.draw();
            }
            break;

        }

        //default: SOMETHING VERY BAD IS GOING TO HAPPEN NOW
    }

    virtual void start() {
        _display.set_brightness(30);
        _display.fill_screen(0, 0, 0);
    }

    virtual void tick() {
        if (messages_received) {
            messages_received = false;
            _display.draw();
        }
    }

    virtual unsigned long tick_speed() { return 50000ul; }
    
private:
    bool messages_received = false;

    mysook::RGBPanel<8,8> &_display;
};

#endif//__ON_AIR_SIGN_H

