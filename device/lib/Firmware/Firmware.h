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

unsigned long get_micros();

class Ticking {
public:
    virtual bool ready_for_tick(unsigned long now) = 0;
    virtual void tick() = 0;
};

class TickingTimer {
protected:
    const unsigned long tick_speed;
    unsigned long next_tick = 0;

public:
    TickingTimer(unsigned long tick_speed) : tick_speed(tick_speed) { }

    virtual bool ready_for_tick(unsigned long now) {
        if (now > next_tick) {
            if (next_tick + tick_speed > now) {
                next_tick += tick_speed;
            }
            else {
                next_tick = now + tick_speed;
            }

            return true;
        }

        return false;
    }
};

class TickingVariableTimer {
protected:
    unsigned long rollover = ULONG_MAX;
    unsigned long previous_tick = 0;
    unsigned long next_tick = 0;

public:
    virtual unsigned long tick_speed() { return 0; }

    void set_previous_tick() { previous_tick = get_micros(); }
    void set_previous_tick(unsigned long tick) { previous_tick = tick; }

    void nudge_tick() { next_tick_after(tick_speed()); }
    void reset_tick() { set_previous_tick(); next_tick_after(tick_speed()); }

    void next_tick_after(unsigned long wait); 
    void next_tick_at(unsigned long time, unsigned long rollover = ULONG_MAX) {
        next_tick = time;
        this->rollover = rollover;
    }

    virtual bool ready_for_tick(unsigned long now);
};

#define MAX_TICKERS 20

class Firmware : public TickingVariableTimer {
protected:
    Logger &log;

    Ticking *pre_tickers[MAX_TICKERS];
    int pre_ticker_count = 0;

    Ticking *post_tickers[MAX_TICKERS];
    int post_ticker_count = 0;

    void run_pre_ticks(unsigned long now) {
        for (Ticking **t = pre_tickers; t < t + pre_ticker_count; t++)
            if ((*t)->ready_for_tick(now))
                (*t)->tick();
    }

    void run_post_ticks(unsigned long now) {
        for (Ticking **t = post_tickers; t < t + post_ticker_count; t++)
            if ((*t)->ready_for_tick(now))
                (*t)->tick();
    }

public:
    Firmware(Logger *log) : Firmware(*log) { }
    Firmware(Logger &log) : log(log) { 
        next_tick = get_micros();
    }

    void add_pre_ticker(Ticking *ticker) {
        if (pre_ticker_count >= MAX_TICKERS) {
            logf_ln("E [firmware] Too many pre-tickers registered");
            return;
        }

        pre_tickers[pre_ticker_count++];
    }

    void add_post_ticker(Ticking *ticker) {
        if (post_ticker_count >= MAX_TICKERS) {
            logf_ln("E [firmware] Too many post-tickers registered");
            return;
        }

        post_tickers[post_ticker_count++];
    }

    virtual void start() = 0;
    virtual void tick() = 0;

#ifdef ARDUINO
    virtual void idle() { delay(1); }
#else
    virtual void idle() { sleep(1); }
#endif

    void setup() {
        start();

        unsigned long now = get_micros();
        run_pre_ticks(now);
        if (ready_for_tick(now)) {
            tick();
        }
        run_post_ticks(now);
    }

    void loop() {
        unsigned long now = get_micros();
        run_pre_ticks(now);
        if (ready_for_tick(now)) {
            tick();
        }

        else {
            idle();
        }
        run_post_ticks(now);
    }

    virtual void write_log(const char *msg) { log.write_log(msg); }

    void logf_ln(const char *fmt...) { 
        va_list args;
        va_start(args, fmt);
        log.vlogf_ln(fmt, args);
        va_end(args);
    }

    void logf(const char *fmt...) {
        va_list args;
        va_start(args, fmt);
        log.vlogf(fmt, args);
        va_end(args);
    }
};

};

#endif//__FIRMWARE_H
