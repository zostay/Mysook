#ifndef __LOGGER_H
#define __LOGGER_H

#include <functional>
#include <cstdarg>

namespace mysook {

typedef std::function<unsigned long()> GetMicrosFunction;

class Logger {
public:
    virtual void write_log(const char *str) = 0;

    virtual void vlogf_ln(const char *fmt, va_list args);
    virtual void vlogf(const char *fmt, va_list args);

    virtual void logf_ln(const char *fmt ...);
    virtual void logf(const char *fmt ...);
};

};

#endif//__LOGGER_H
