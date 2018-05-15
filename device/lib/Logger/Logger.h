#ifndef __LOGGER_H
#define __LOGGER_H

namespace mysook {

class Logger {
    public:
        virtual void write_log(const char *str) = 0;
};

};

#endif//__LOGGER_H
