#ifndef __ARDUINO_RTC_H
#define __ARDUINO_RTC_H

#include <RTC.h>

namespace mysook {

template <class T>
class MC_RTC : public RTC {
protected:
    T *chip;

public:
    MC_RTC(T *chip) { this->chip = chip; }

    virtual DateTime now() { 
        return DateTime(chip->now().unixtime()); 
    }

    virtual void adjust(const DateTime &newNow) {
        chip->adjust(
            ::DateTime(
                newNow.year(),
                newNow.month(),
                newNow.day(),
                newNow.hour(),
                newNow.minute(),
                newNow.second()
            )
        );
    }
};

};

#endif//__ARDUINO_RTC_H
