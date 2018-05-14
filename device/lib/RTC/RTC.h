#ifndef __RTC_H
#define __RTC_H

// The DateTime/TimeSpan classes are convenient for us too
#include "RTClib.h"

namespace mysook {

class RTC {
    public:
        virtual DateTime now() = 0;
};

};

#endif//__RTC_H
