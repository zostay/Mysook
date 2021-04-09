#ifndef __SIM_WIRE_H
#define __SIM_WIRE_H

#include <zmq.hpp>

class SimWire {
public:
    SimWire(zmq::context_t ctx, bool master, const char *in_port, const char *out_port) 
    : bus_in(ctx, ZMQ_SUB), bus_out(ctx, ZMQ_PUB) {
        if (master) {
            bus_in.bind(in_port);
            bus_out.bind(out_port);
        }
        else {
            bus_in.connect(in_port);
            bus_out.connect(out_port);
        }
    }

    virtual void begin() { 
        message_for = -1;
        ptr = 0;
    }

    virtual void beginTransmission(uint8_t address) {
        message_for = address;
        ptr = 0;
    }

    virtual void write(char byte) {
        buf[ptr++] = byte;
    }

    virtual void write(const char *buf, size_t len) {
        memcpy(buf + ptr, buf, len);
        ptr += len;
    }

    virtual void endTransmission() {
        zmq::message_t tx(ptr);
        memcpy(tx.data(), buf, ptr);
        bus_out.send(buf);

        message_for = -1;
        ptr = 0;
    }

private:
    int message_for;
    char buf[64];
    int ptr;

    zmq::socket_t bus_in;
    zmq::socket_t bus_out;
};

#endif//__SIM_WIRE_H
