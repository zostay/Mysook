#include <SPI.h>
#include <DoorlightSPI.h>

void Controller::start() {
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2);
}

void Controller::tick() {
    if (keyframe_len == 0)
        return;

    digitalWrite(DOORCSPIN, LOW);

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

    digitalWrite(DOORCSPIN, LOW);
}
