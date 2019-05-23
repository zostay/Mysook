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
#include <Network.h>
#include <RTC.h>

#include "config.h"

#ifdef NAMETAG
#define URL_API       "http://192.168.205.116:5000" 
#elif
#define URI_API       "https://api.zostay.com"
#endif

#define URL_TIME      URL_API "/time"
#define URL_ALARM     URL_API "/alarm"

class TClockConfig : public mysook::Ticking {
private:
    mysook::Logger *log;
    mysook::Network &network;
    ToddlerClockConfig &clock;
    mysook::RTC *rtc = 0;
    bool ran_already = false;

    HTTPClient ua;

public:
    TClockConfig(mysook::Logger *log, mysook::Network &network, ToddlerClockConfig &clock, mysook::RTC *rtc) : log(log), network(network), clock(clock), rtc(rtc) { }
    virtual ~TClockConfig() { }

    virtual bool ready_for_tick(unsigned long) { 
        if (ran_already) return false;
        if (!network.connected()) return false;
        ran_already = true;
        return true;
    }

    virtual void tick() {
        fetch_time();
        fetch_alarm();
    }

    void fetch_time() {
        ua.begin(String(URL_TIME));
        int code;
        if ((code = ua.GET()) <= 0) {
            log->logf_ln("E [tclock_config] unable to fetch time from %s : (%d) %s", URL_TIME, code, ua.errorToString(code));
            return;
        }

        if (code != 200) {
            log->logf_ln("E [tclock_config] unexpected HTTP status code %d from %s", code, URL_TIME);
            return;
        }

        String payload = ua.getString();

        StaticJsonDocument<200> dt;
        auto error = deserializeJson(dt, payload);
        if (error) {
            log->logf_ln("E [tclock_config] unable to read JSON %s", error.c_str());
            return;
        }

        mysook::DateTime new_now(
            dt["time"]["year"],
            dt["time"]["month"],
            dt["time"]["day"],
            dt["time"]["hour"],
            dt["time"]["minute"],
            dt["time"]["second"]
        );

        rtc->adjust(new_now);

        log->logf_ln("I [tclock_config] Adjusted time to %04d-%02d-%02d %02d:%02d:%02d",
            new_now.year(),
            new_now.month(),
            new_now.day(),
            new_now.hour(),
            new_now.minute(),
            new_now.second()
        );
    }

    void fetch_alarm() {
        ua.begin(URL_ALARM);

        int code;
        if ((code = ua.GET()) <= 0) {
            log->logf_ln("E [tclock_error] unable to fetch alarm from %s", URL_ALARM);
            return;
        }

        if (code != 200) {
            log->logf_ln("E [tclock_config] unexpected HTTP status code %d from %s", code, URL_TIME);
            return;
        }

        String payload = ua.getString();

        StaticJsonDocument<500> al;
        auto error = deserializeJson(al, payload);
        if (error) {
            log->logf_ln("E [tclock_error] unable to read JSON %s", error.c_str());
            return;
        }

        ua.end();

        clock.morning_time.h = al["morning-time"]["hour"];
        clock.morning_time.m = al["morning-time"]["minute"];
        log->logf_ln("I [tclock_config] morning time is %02d:%02d", 
            clock.morning_time.h,
            clock.morning_time.m
        );

        clock.sleeping_time.h = al["sleeping-time"]["hour"];
        clock.sleeping_time.m = al["sleeping-time"]["minute"];
        log->logf_ln("I [tclock_config] sleeping time is %02d:%02d", 
            clock.sleeping_time.h,
            clock.sleeping_time.m
        );

        clock.night_color.r = al["night-color"]["red"];
        clock.night_color.g = al["night-color"]["green"];
        clock.night_color.b = al["night-color"]["blue"];
        clock.night_color.brightness = al["night-color"]["brightness"];
        log->logf_ln("I [tclock_config] night color is (%d, %d, %d) at %d",
            clock.night_color.r,
            clock.night_color.g,
            clock.night_color.b,
            clock.night_color.brightness
        );

        clock.day_color.r = al["day-color"]["red"];
        clock.day_color.g = al["day-color"]["green"];
        clock.day_color.b = al["day-color"]["blue"];
        clock.day_color.brightness = al["day-color"]["brightness"];
        log->logf_ln("I [tclock_config] day color is (%d, %d, %d) at %d",
            clock.day_color.r,
            clock.day_color.g,
            clock.day_color.b,
            clock.day_color.brightness
        );
    }
};

#endif//__CONFIG_H
