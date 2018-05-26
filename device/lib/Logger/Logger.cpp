#include <Logger.h>
#include <cstdarg>
#include <cstdio>

using namespace mysook;

#define PRINT_BUF   256
void Logger::vlogf_ln(const char *fmt, va_list args) {
    char buf[PRINT_BUF];
    vsnprintf(buf, sizeof(buf) - 2, fmt, args);

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

void Logger::logf_ln(const char *fmt ...) {
    va_list args;
    va_start(args, fmt);
    vlogf_ln(fmt, args);
    va_end(args);
}

void Logger::vlogf(const char *fmt, va_list args) {
    char buf[PRINT_BUF];
    vsnprintf(buf, sizeof(buf) - 2, fmt, args);

    write_log(buf);
}

void Logger::logf(const char *fmt ...) {
    va_list args;
    va_start(args, fmt);
    vlogf(fmt, args);
    va_end(args);
}
