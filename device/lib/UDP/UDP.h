#ifndef __UDP_H
#define __UDP_H

#ifdef NOT_FUNCTIONAL
#include <functional>
#endif

#include <Network.h>

namespace mysook {

#ifdef NOT_FUNCTIONAL
typedef void (*UdpDispatcher)(String&,int,char*,size_t);
#else
typedef std::function<void(String&,int,char*,size_t)> UdpDispatcher;
#endif

class UdpListener : public Ticking {
    void ensure_started() {
        if (!udp) {
            udp.begin(port);
            _log.logf_ln("I [udp] UDP server listening on port %d", port);
        }
    }

    void ensure_stopped() {
        if (udp) {
            _log.logf_ln("W [udp] UDP server on port %d stopping", port);
            udp.stop();
        }
    }

    void receive_packet() {
        int packet_size = udp.parsePacket();
        if (packet_size) {
            if (packet_size > buffer_size) 
                _log.logf_ln("W [udp] Received packet of size %d from %s:%d, but buffer size is %d. Message truncated.", 
                    packet_size, 
                    udp.rempveIP().toString().c_str(),
                    udp.removePort(,
                    buffer_size
                );
            else
                _log.logf_ln("I [udp] Received packet of size %d from %s:%d",
                    packet_size,
                    udp.rempveIP().toString().c_str(),
                    udp.removePort()
                );

            int len = udp.read(buffer, buffer_size);
            dispatcher(udp.remoteIP().toString(), udp.remotePort(), buffer, len);
        }
        udp.flush();
    }

public:
    UdpListener(Network &net, int port, UdpDispatcher &dispatcher, Logger &log, int buffer_size = 500) 
    : network(net), port(port), dispatcher(dispatcher), _log(log), buffer_size(buffer_size) { 
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
        if (start && network.connected())
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
    const UdpDispatcher &dispatcher;
    Logger &_log;

    bool start = false;

};

};

#endif//__UDP_H

