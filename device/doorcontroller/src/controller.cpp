#include <string.h>

#include <SPI.h>
#include <DoorlightSPI.h>

#include "controller.h"

void Controller::start() {
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2);
}

void Controller::tick() {
    if (keyframe_len == 0)
        return;

    digitalWrite(pin, LOW);

    for (int y = 0; y < MAX_Y; ++y) {
        if (y == 0) {
            SPI.transfer(VSYNC_BYTE);
        }
        else {
            SPI.transfer(SYNC_BYTE);
        }

        key_frame *f = frames+keyframe_pos;
        int start = f->x + f->y * w;
        if (start > h) {
            start = h-1;
        }

        int size = w;
        if (f->x + MAX_X > w) {
            size = w - f->x;
        }

        SPI.transfer(bmp+start, size);
        SPI.transfer(END_BYTE);
    }

    digitalWrite(pin, LOW);
}

void Controller::load_bmp(const unsigned char *pix, size_t size) {
    memcpy(bmp, pix, size * 3);
}

void Controller::load_keyframes(const uint16_t *key_frames, size_t size) {
    memcpy(frames, key_frames, size*2);
}

bool Controller::operator() (mysook::Request &req, mysook::Response &res) {
    if (req.method() != "POST") {
        return false;
    }

    if (req.path() == "/bmp") {
        handle_bmp(req, res);
        return true;
    }
    else if (req.path() == "/frames") {
        handle_frames(req, res);
        return true;
    }
    else if (req.path() == "/flip") {
        handle_flip(req, res);
        return true;
    }

    return false;
}

void Controller::handle_bmp(mysook::Request &req, mysook::Response &res) {
}

void Controller::handle_frames(mysook::Request &req, mysook::Response &res) {
}
