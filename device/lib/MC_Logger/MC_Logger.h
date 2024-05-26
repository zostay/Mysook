#ifndef __MC_LOGGER_H
#define __MC_LOGGER_H

#include <Logger.h>

namespace mysook {

template <class T>
class MC_Logger : public Logger {
protected:
    unsigned long (*_micros)();
    T &serial;

    bool _enabled[26];

public:
    MC_Logger(unsigned long (*_micros)(), T &serial) : _micros(_micros), serial(serial) { 
        set_enabled('I');
        set_enabled('W');
        set_enabled('E');
        set_enabled('F');
    }
    MC_Logger(unsigned long (*_micros)(), T *serial) : MC_Logger(_micros, *serial) { }

    void set_enabled(const char level) {
        _enabled[level-'A'] = true;
    }
    
    void set_disabled(const char level) {
        _enabled[level-'A'] = false;
    }

    bool is_enabled(const char level) {
        return _enabled[level-'A'];
    }

    virtual void write_log(const char *str) { 
        // TODO This is a kludge... really ought to revamp the logging code to
        // do this smarter
        if (str[0] >= 'A' && str[0] <= 'Z' && str[1] == ' ') {
            if (!_enabled[str[0]-'A']) return;

            int last = -1;
            for (int i = 0; i < 255; ++i) {
                if (str[i] == '\0') {
                    last = i - 1;
                    break;
                }
            }

            if (last > 1 && str[last] == '\n') {
                unsigned long micros = _micros();
                String seconds(micros / 1000000);
                String decimal(micros % 1000000);
                while (decimal.length() < 6) decimal = "0" + decimal;

                serial.print(seconds);
                serial.print('.');
                serial.print(decimal);
                serial.print(" ");
            }
        }

        serial.print(str); 
    }
};

};

#endif//__MC_LOGGER_H
