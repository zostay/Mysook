#ifndef __SIMRTC_H
#define __SIMRTC_H

#include <ctime>
#include <RTC.h>

namespace mysook {

class SimRTC : public RTC {
protected:
    long offset = 0l;

public:
    virtual DateTime now() { return DateTime(time(NULL) + offset); }
    
    // allow some tweaking for debugging/timezones/DST/etc.
    void set_offset(long offset) { this->offset = offset; }
    void reset_offset() { offset = 0l; }
    void adjust_to(time_t preferred_time) {
        offset = preferred_time - time(NULL);
    }
};

};

#endif//__SIMRTC_H
