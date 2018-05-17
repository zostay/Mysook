#ifndef __RTC_H
#define __RTC_H
 
#define SECONDS_FROM_1970_TO_2000 946684800

namespace mysook {

// shameless copy from Adafruit's RTClib.h
class DateTime {
public:
    DateTime (uint32_t t =0);
    DateTime (uint16_t year, uint8_t month, uint8_t day,
                uint8_t hour =0, uint8_t min =0, uint8_t sec =0);
    DateTime (const DateTime& copy);
    DateTime (const char* date, const char* time);
    uint16_t year() const       { return 2000 + yOff; }
    uint8_t month() const       { return m; }
    uint8_t day() const         { return d; }
    uint8_t hour() const        { return hh; }
    uint8_t minute() const      { return mm; }
    uint8_t second() const      { return ss; }
    uint8_t dayOfTheWeek() const;

    // 32-bit times as seconds since 1/1/2000
    long secondstime() const;   
    // 32-bit times as seconds since 1/1/1970
    uint32_t unixtime(void) const;

protected:
    uint8_t yOff, m, d, hh, mm, ss;
};

class RTC {
public:
    virtual DateTime now() = 0;
};

};

#endif//__RTC_H
