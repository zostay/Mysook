#ifndef __FIRMWARE_H
#define __FIRMWARE_H

#include <Arduino.h>
#include <Logger.h>

namespace mysook {

class Firmware {
    protected:
        Logger *log;

    public:
        Firmware(Logger *log) { this->log = log; }

        virtual int tick_speed() { return 0; }
    
        virtual void setup() = 0;
        virtual void loop() = 0;

        void tick() {
            loop();
            delay(tick_speed());
        }

        virtual void write_log(char *msg) { log->write_log(msg); }

        void logf_ln(char *fmt...);
        void logf(char *fmt...);
};

};

#endif//__FIRMWARE_H
