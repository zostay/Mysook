#ifndef __FIRMWARE_H
#define __FIRMWARE_H

#ifdef ARDUINO
#include <Arduino.h>
#endif//ARDUINO

#include <climits>
#include <Logger.h>

namespace mysook {

class Firmware {
    protected:
        Logger *log;

        unsigned long rollover = ULONG_MAX;
        unsigned long next_tick = 0;

#ifdef ARDUINO
        unsigned long get_micros() { return micros(); }
#else
        unsigned long get_micros();
#endif

        // checks if tick_speed() micros have passed, accounting for rollovers,
        // which happen every 4,294.967296 seconds.
        bool ready_for_tick();

    public:
        Firmware(Logger *log) { this->log = log; }

        virtual unsigned long tick_speed() { return 0; }
    
        virtual void start() = 0;
        virtual void tick() = 0;
        virtual void idle() { }

        void setup() {
            start();
        }

        void loop() {
            if (ready_for_tick()) {
                loop();
            }

            else {
                idle();
            }
        }

        virtual void write_log(const char *msg) { log->write_log(msg); }

        void logf_ln(const char *fmt...);
        void logf(const char *fmt...);
};

};

#endif//__FIRMWARE_H
