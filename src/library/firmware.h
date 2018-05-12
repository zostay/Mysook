#ifndef __FIRMWARE_H
#define __FIRMWARE_H

namespace mysook {

class Firmware {
    public:
        virtual int tick_speed() { return 0; }
    
        virtual void setup() = 0;
        virtual void loop() = 0;

        void tick() {
            loop();
            delay(tick_speed());
        }
}

}

#endif//__FIRMWARE_H
