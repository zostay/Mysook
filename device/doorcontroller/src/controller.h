#ifndef __CONTROLLER_H
#define __CONTROLLER_H

#include <Firmware.h>
#include <LED.h>
#include <MC_Network.h>

//   3 bpp * (2^12 * 5^2) pixels
//   2x51200     51200x2
//   4x25600     25600x4
//   5x20480     20480x5
//   8x12800     12800x8
//   10x10240    10240x10
//   16x6400   * 6400x16
//   20x5120   * 5120x20
//   25x4096   * 4096x25
// * 32x3200   * 3200x32
// * 40x2560   * 2560x40
// * 50x2048   * 2048x50
// * 64x1600   * 1600x64
// * 80x1280   * 1280x80
// * 100x1024  * 1024x100
// * 128x800   * 800x128
// * 160x640   * 640x160
// * 200x512   * 512x200
// * 256x400   * 400x256
// * 320x320

#define MAX_BMP_SIZE 307200
#define MAX_KEY_FRAMES 1000

struct key_frame {
    short x, y;
};

class Controller : public mysook::Firmware {
private:
    mysook::Network &network;
    mysook::Logger &log;
    mysook::WiFiLED &led;

    int pin;

    char bmp[MAX_BMP_SIZE];
    key_frame frames[MAX_KEY_FRAMES];
    short w = 0, h = 0;
    short keyframe_len = 0;
    short keyframe_pos = 0;

public:
    Controller(mysook::Logger &log, mysook::WiFiLED &led, mysook::Network &network, int pin) 
    : Firmware(log), led(led), network(network), log(log), pin(pin) {
        this->add_pre_ticker(&network);
        this->add_pre_ticker(&led);

        for (int i = 0; i < MAX_KEY_FRAMES; i++) {
            frames[i].x = 0;
            frames[i].y = 0;
        }
    }

    virtual unsigned long tick_speed() { return 30000; }

    virtual void start();
    virtual void tick();
};

#endif//__CONTROLLER_H
