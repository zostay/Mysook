#ifndef __TCLOCK_CONFIG_H
#define __TCLOCK_CONFIG_H

#include <ArduinoJson.h>
#include <WiFiClient.h>

#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#elif ESP32
#include <HTTPClient.h>
#endif

#include <Firmware.h>
#include <Logger.h>
#include <RTC.h>

#include "config.h"

#define URL_API       "https://api.zostay.com"
#define URL_TIME      URL_API "/time"
#define URL_ALARM     URL_API "/alarm"

class TClockConfig : public mysook::Ticking {
private:
    mysook::Logger *log;
    HTTPClient ua;
    ToddlerClockConfig &clock;
    mysook::RTC *rtc = 0;
    bool ran_already = false;

public:
    TClockConfig(mysook::Logger *log, ToddlerClockConfig &clock, mysook::RTC *rtc) : log(log), clock(clock), rtc(rtc) { }
    virtual ~TClockConfig() { }

    virtual bool ready_for_tick(unsigned long) { 
        if (ran_already) return false;
        ran_already = true;
        return true;
    }

    virtual void tick() {
        fetch_time();
        fetch_alarm();
    }

    void fetch_time() {
        ua.begin(String(URL_TIME));
        if (ua.GET() <= 0) {
            log->logf_ln("[error] unable to fetch time from %s", URL_TIME);
            return;
        }

        StaticJsonDocument<200> dt;
        auto error = deserializeJson(dt, ua.getStream());
        if (error) {
            log->logf_ln("[error] unable to read JSON %s", error.c_str());
            return;
        }

        rtc->adjust(
            mysook::DateTime(
                dt["year"],
                dt["month"],
                dt["day"],
                dt["hour"],
                dt["minute"],
                dt["second"]
            )
        );

        log->logf_ln("[info] Adjusted time to %04d-%02d-%02d %02d:%02d:%02d",
            dt["year"],
            dt["month"],
            dt["day"],
            dt["hour"],
            dt["minute"],
            dt["second"]
        );
    }

    void fetch_alarm() {
        ua.begin(URL_ALARM);

        if (ua.GET() <= 0) {
            log->logf_ln("[error] unable to fetch alarm from %s", URL_ALARM);
            return;
        }

        StaticJsonDocument<500> al;
        auto error = deserializeJson(al, ua.getStream());
        if (error) {
            log->logf_ln("[error] unable to read JSON %s", error.c_str());
            return;
        }

        clock.morning_time.h = al["morning_time"]["hour"];
        clock.morning_time.m = al["morning_time"]["minute"];
        log->logf_ln("[info] morning time is %02d:%02d", al["morning_time"]["hour"], al["morning_time"]["minute"]);

        clock.sleeping_time.h = al["sleeping_time"]["hour"];
        clock.sleeping_time.m = al["sleeping_time"]["minute"];
        log->logf_ln("[info] sleeping time is %02d:%02d", al["sleeping_time"]["hour"], al["sleeping_time"]["minute"]);

        clock.night_color.r = al["night_color"]["red"];
        clock.night_color.g = al["night_color"]["green"];
        clock.night_color.b = al["night_color"]["blue"];
        clock.night_color.brightness = al["night_color"]["brightness"];
        log->logf_ln("[info] night color is (%d, %d, %d) at %d",
            al["night_color"]["red"],
            al["night_color"]["green"],
            al["night_color"]["blue"],
            al["night_color"]["brightness"]
        );

        clock.day_color.r = al["day_color"]["red"];
        clock.day_color.g = al["day_color"]["green"];
        clock.day_color.b = al["day_color"]["blue"];
        clock.day_color.brightness = al["day_color"]["brightness"];
        log->logf_ln("[info] day color is (%d, %d, %d) at %d",
            al["day_color"]["red"],
            al["day_color"]["green"],
            al["day_color"]["blue"],
            al["day_color"]["brightness"]
        );
    }
};

#endif//__CONFIG_H
