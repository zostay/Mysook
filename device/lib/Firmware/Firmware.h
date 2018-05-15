#ifndef __FIRMWARE_H
#define __FIRMWARE_H

#ifdef ARDUINO
#include <Arduino.h>
#endif//ARDUINO

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

            int wait = tick_speed();
            if (wait > 0) delay(wait);
        }

        virtual void write_log(const char *msg) { log->write_log(msg); }

        void logf_ln(const char *fmt...);
        void logf(const char *fmt...);
};

};

#endif//__FIRMWARE_H
