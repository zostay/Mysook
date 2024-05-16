#ifndef __WEBCLIENT_H
#define __WEBCLIENT_H

#if defined(ARDUINO) || defined(ESP_PLATFORM)
#include <WiFiClient.h>
#else
#include <Simduino.h>
#include <SimWiFi.h>
#endif//ARDUINO

namespace mysook {

#define MAX_CLIENTS    4
#define MAX_RES_LENGTH 65535

struct RequestState {
    WiFiClient client;
    bool active;
    MessageState
}

class WebClient : public Ticking {
protected:
    Network &network;



public:
};

};

#endif//__WEBCLIENT_H
