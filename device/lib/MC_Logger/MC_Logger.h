#ifndef __MC_LOGGER_H
#define __MC_LOGGER_H

#include <Logger.h>

namespace mysook {

template <class T>
class MC_Logger : public Logger {
    protected:
        T *serial;

    public:
        MC_Logger(T *serial) { this->serial = serial; }

        virtual void write_log(char *str) { serial->print(str); }
};

};

#endif//__MC_LOGGER_H
