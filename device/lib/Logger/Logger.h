#ifndef __LOGGER_H
#define __LOGGER_H

#ifndef NOT_FUNCTIONAL
#include <functional>
#endif//NOT_FUNCTIONAL
#include <cstdarg>

namespace mysook {

#ifdef NOT_FUNCTIONAL
typedef unsigned long (*GetMicrosFunction)();
#else//NOT_FUNCTIONAL
typedef std::function<unsigned long()> GetMicrosFunction;
#endif//NOT_FUNCTIONAL

// Some sinister dark magic is going on here...
#undef logf

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
