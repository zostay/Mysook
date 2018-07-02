#ifndef __ON_AIR_SIGN_H
#define __ON_AIR_SIGN_H

#include <Firmware.h>
#include <Panel.h>
#include <Logger.h>

#define BRIGHTNESS 0x01
#define PIXEL      0x02

#define BRIGHTNESS_SIZE 1
#define PIXEL_SIZE      5

class OnAirSign : public Firmware {
public:
    OnAirSign(mysook::Logger &logger, mysook::Panel &display) 
        : Firmware(logger), _display(display) {}

    void send(uint8_t op, uint8_t *msg, size_t msg_size) {
        switch (op) {
        case BRIGHTNESS:
            if (msg_size == BRIGHTNESS_SIZE) {
                _display.set_brightness(msg[0]);
            }
            break;

        case PIXEL:
            if (msg_size == PIXEL_SIZE) {
                _display.put_pixel(
                    msg[0],
                    msg[1],
                    msg[2],
                    msg[3],
                    msg[4],
                );
            }
            break;
    }

    virtual void start() {
        display.set_brightness(30);
        display.fill(Color(0, 0, 0));
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
}

#endif//__ON_AIR_SIGN_H

