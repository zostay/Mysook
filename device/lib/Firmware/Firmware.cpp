#include <Firmware.h>
#include <cstdarg>

using namespace mysook;

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
