#ifndef __MESSAGE_BUS_H
#define __MESSAGE_BUS_H

#include <Wire.h>
#include <Firmware.h>

namespace mysook;

template <class W, M>
class MessageBus : public Ticking {
public:
    MessageBus(W &wire) : wire(wire) { }

    virtual void emit(int address, char id, msg, size_t bytes) {
        wire.beginTransmission(address);

        wire.write(id);

        if (msg != 0 && bytes > 0)
            wire.write(msg, bytes);

        wire.endTransmission();
    }

private:
    W &wire;
}

#endif//__MESSAGE_BUS_H
