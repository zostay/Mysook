#include <Firmware.h>
#include <cstdarg>
#include <climits>

#ifndef ARDUINO
#include <sys/time.h>
#include <cstdio>
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

bool Firmware::ready_for_tick() {
    unsigned long now = get_micros();
    if (now >= next_tick && now <= rollover) {
        unsigned long wait = tick_speed();

        if (ULONG_MAX - now < wait) {
            rollover = next_tick;
            next_tick = wait - (ULONG_MAX - now);
        }
        else {
            rollover = ULONG_MAX;
            next_tick = now + wait;
        }

        return true;
    }

    return false;
}

#define PRINT_BUF   256
void Firmware::logf_ln(const char *fmt ...) {
    char buf[PRINT_BUF];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf) - 2, fmt, args);
    va_end(args);

    for (int i = 0; i < PRINT_BUF; ++i) {
        if (buf[i] == '\0') {
            buf[i] = '\r';
            buf[i+1] = '\n';
            buf[i+2] = '\0';
            break;
        }
    }

    write_log(buf);
}

void Firmware::logf(const char *fmt ...) {
    char buf[PRINT_BUF];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf) - 2, fmt, args);
    va_end(args);

    write_log(buf);
}
