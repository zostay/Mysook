#ifndef __MC_MDNS_H
#define __MC_MDNS_H

#ifdef ESP8266
#include <ESP8266mDNS.h>
#else//ESP8266
#include <ESPmDNS.h>
#endif//ESP8266


namespace mysook {

class MC_MDNS {
protected:
    const std::string &name;
    Logger &log;

public:
    MC_MDNS(const char *name, Logger &log) : name(name), log(log) { }
    MC_MDNS(std::string &name, Logger &log) : name(name), log(log) { }

    void begin() {
        if (!MDNS.begin(name.c_str())) {
            log.logf_ln("E [MC_MDNS] unable to configure mDNS responder");
        }
        else {
            log.logf_ln("I [MC_MDNS] mDNS responding to %s", name.c_str());
        }
    }

    void add_service(const char *name, const char *proto, int port) {
        MDNS.addService(name, proto, port);
    }
};

};

#endif//__MC_MDNS_H
