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

class TickingTimer : public Ticking {
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

class TickingVariableTimer : public Ticking {
protected:
    unsigned long rollover = ULONG_MAX;
    unsigned long previous_tick = get_micros();
    unsigned long next_tick = get_micros();

public:
    TickingVariableTimer() { nudge_tick(); }

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

typedef Ticking *TickerPtr;

class TickArray {
protected:
    TickerPtr *tickers;
    int ticker_count;
    int ticker_capacity;

    void stretch(int by = 5) {
        ticker_capacity += by;
        TickerPtr *new_tickers = new TickerPtr[ticker_capacity];

        for (int i = 0; i < ticker_count; ++i)
            new_tickers[i] = tickers[i];

        delete[] tickers;
        tickers = new_tickers;
    }

public:
    TickArray(int capacity = 5) 
    : tickers(new TickerPtr[capacity]), ticker_count(0), ticker_capacity(capacity) { }

    virtual ~TickArray() { delete[] tickers; }

    void run(unsigned long now) {
        for (int i = 0; i < ticker_count; ++i) 
            if (tickers[i]->ready_for_tick(now))
                tickers[i]->tick();
    }

    void add(TickerPtr ticker) {
        if (ticker_count >= ticker_capacity)
            stretch();

        tickers[ticker_count++] = ticker;
    }

    void remove(TickerPtr ticker) {
        bool searching = true;
        for (int i = 0; i < ticker_count; ++i) {
            if (searching) {
                if (tickers[i] == ticker)
                    searching = false;
            }
            else {
                ticker[i - 1] = ticker[i];
            }

            --ticker_count;
        }
    }
};

class Firmware : public TickingVariableTimer {
public:
    Firmware(Logger *logger) : Firmware(*logger) { }
    Firmware(Logger &logger) : _log(logger) { 
        next_tick = get_micros();
    }

    void add_pre_ticker(Ticking *ticker) {
        pre_tickers.add(ticker);
    }

    void remove_pre_ticker(Ticking *ticker) {
        pre_tickers.remove(ticker);
    }

    void add_post_ticker(Ticking *ticker) {
        post_tickers.add(ticker);
    }

    void remove_post_ticker(Ticking *ticker) {
        post_tickers.remove(ticker);
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
        pre_tickers.run(now);
        if (ready_for_tick(now)) {
            tick();
        }
        post_tickers.run(now);
    }

    void loop() {
        unsigned long now = get_micros();
        pre_tickers.run(now);
        if (ready_for_tick(now)) {
            tick();
        }

        else {
            idle();
        }
        post_tickers.run(now);
    }

    virtual void write_log(const char *msg) { _log.write_log(msg); }

    void logf_ln(const char *fmt...) { 
        va_list args;
        va_start(args, fmt);
        _log.vlogf_ln(fmt, args);
        va_end(args);
    }

    void logf(const char *fmt...) {
        va_list args;
        va_start(args, fmt);
        _log.vlogf(fmt, args);
        va_end(args);
    }
protected:
    Logger &_log;

    TickArray pre_tickers;
    TickArray post_tickers;

};

};

#endif//__FIRMWARE_H
