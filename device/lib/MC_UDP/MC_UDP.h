#ifndef __MC_UDP_H
#define __MC_UDP_H

#ifdef NOT_FUNCTIONAL
#include <functional>
#endif

#include <WifiUDP.h>

#include <Firmware.h>
#include <Network.h>

namespace mysook {

#ifdef NOT_FUNCTIONAL
typedef void (*UdpDispatcher)(String&,uint16_t,const char*,size_t);
#else
typedef std::function<void(String&,uint16_t,const char*,size_t)> UdpDispatcher;
#endif

class UdpListener : public Ticking {
    void ensure_started() {
        if (!udp_begun) {
            udp.begin(port);
            udp_begun = true;
            _log.logf_ln("I [udp] UDP server listening on port %d", port);
        }
    }

    void ensure_stopped() {
        if (udp_begun) {
            udp_begun = false;
            _log.logf_ln("W [udp] UDP server on port %d stopping", port);
            udp.stop();
        }
    }

    void receive_packet() {
        if (!udp_begun) return;

        int packet_size = udp.parsePacket();
        if (packet_size) {
            if (packet_size > buffer_size) 
                _log.logf_ln("W [udp] Received packet of size %d from %s:%d, but buffer size is %d. Message truncated.", 
                    packet_size, 
                    udp.remoteIP().toString().c_str(),
                    udp.remotePort(),
                    buffer_size
                );
            else
                _log.logf_ln("I [udp] Received packet of size %d from %s:%d",
                    packet_size,
                    udp.remoteIP().toString().c_str(),
                    udp.remotePort()
                );

            size_t len = udp.read(buffer, buffer_size);
            String remoteIP = udp.remoteIP().toString();
            dispatcher(remoteIP, udp.remotePort(), buffer, len);
        }
        udp.flush();
    }

public:
    UdpListener(Network &net, int port, UdpDispatcher &dispatcher, Logger &log, int buffer_size = 500) 
    : net(net), port(port), dispatcher(dispatcher), _log(log), buffer_size(buffer_size) { 
        buffer = new char[buffer_size];
    }
    
    ~UdpListener() {
        delete[] buffer;
    }

    void begin() {
        start = true;
    }

    void end() {
        start = true;
    }

    virtual bool ready_for_tick(unsigned long now) { return true; }

    virtual void tick() {
        if (start && net.connected())
            ensure_started();
        else
            ensure_stopped();

        receive_packet();
    }

protected:
    char *buffer;
    int buffer_size;

    int port;

    Network &net;
    WiFiUDP udp;
    UdpDispatcher &dispatcher;
    Logger &_log;

    bool start = false;
    bool udp_begun = false;
};

};

#endif//__MC_UDP_H
