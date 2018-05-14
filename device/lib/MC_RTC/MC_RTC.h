#ifndef __ARDUINO_RTC_H
#define __ARDUINO_RTC_H

#include "rtc.h"

namespace mysook {

template <class T>
class MC_RTC : public RTC {
    protected:
        T *chip;

    public:
        MC_RTC(T *chip) { this->chip = chip; }

        virtual DateTime now() { return chip->now(); }
};

};

#endif//__ARDUINO_RTC_H
