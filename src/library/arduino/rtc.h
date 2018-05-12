#ifndef __ARDUINO_RTC_H
#define __ARDUINO_RTC_H

#include "rtc.h"

namespace mysook {

template <class T>
class ArduinoRTC : public RTC {
    protected:
        T *chip;

    public:
        ArduinoRTC(T *chip) { this.chip = chip; }

        virtual DateTime now() { chip.now() }
}

#endif//__ARDUINO_RTC_H
