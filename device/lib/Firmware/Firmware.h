#ifndef __FIRMWARE_H
#define __FIRMWARE_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <unistd.h>
#endif//ARDUINO

#include <climits>
#include <Logger.h>

namespace mysook {

class Firmware {
    protected:
        Logger *log;

        unsigned long rollover = ULONG_MAX;
        unsigned long previous_tick = 0;
        unsigned long next_tick = 0;

    public:
        Firmware(Logger *log) { 
            next_tick = get_micros();
			this->log = log; 
		}

        virtual unsigned long tick_speed() { return 0; }
    
        virtual void start() = 0;
        virtual void tick() = 0;

#ifdef ARDUINO
        virtual void idle() { delay(1); }
        unsigned long get_micros() { return micros(); }
#else
        virtual void idle() { sleep(1); }
        unsigned long get_micros();
#endif

        void set_previous_tick() { previous_tick = get_micros(); }
        void set_previous_tick(unsigned long tick) { previous_tick = tick; }

        void nudge_tick() { next_tick_after(tick_speed()); }
        void reset_tick() { set_previous_tick(); next_tick_after(tick_speed()); }

        void next_tick_after(unsigned long wait); 
        void next_tick_at(unsigned long time, unsigned long rollover = ULONG_MAX) {
            next_tick = time;
            this->rollover = rollover;
        }

        virtual bool ready_for_tick();

        void setup() {
            start();

            if (ready_for_tick()) {
                tick();
            }
        }

        void loop() {
            if (ready_for_tick()) {
                tick();
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
