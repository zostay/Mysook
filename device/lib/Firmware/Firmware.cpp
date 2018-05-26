#include <Firmware.h>
#include <cstdarg>
#include <climits>

#ifndef ARDUINO
#include <sys/time.h> // precision clock
#include <errno.h>
#include <cstdio>
#include <cstring>
#endif

using namespace mysook;

#ifndef ARDUINO
unsigned long Firmware::get_micros() {
    struct timeval src;
    if (gettimeofday(&src, NULL) == 0) {
        return src.tv_sec * 1000000 + src.tv_usec;
    }

    else {
        logf_ln("Failed making micros from gettimeofday(): %s", strerror(errno));

        // Bad Stuff
        return 0ul;
    }
}
#endif//ARDUINO

void Firmware::next_tick_after(unsigned long wait) {
    if (ULONG_MAX - previous_tick < wait) {
        next_tick_at(wait - (ULONG_MAX - previous_tick), previous_tick);
    }
    else {
        next_tick_at(previous_tick + wait);
    }
}

// checks if tick_speed() micros have passed, accounting for rollovers, which
// happen every 4,294.967296 seconds.
bool Firmware::ready_for_tick() {
    unsigned long now = get_micros();

    if ((now >= next_tick && now <= rollover)) {
        previous_tick = next_tick;
        next_tick_after(tick_speed());

        return true;
    }

    return false;
}

