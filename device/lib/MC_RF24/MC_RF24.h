#ifndef __MC_RF24_H
#define __MC_RF24_H

#include <Receiver.h>
#include <Transmitter.h>
#include <RF24.h>

namespace mysook {

class MC_RF24 {
private:
    size_t get_max_size() const {
        return radio.getPayloadSize();
    }

public:
    MC_RF24(int ce_pin, int csn_pin) : radio(ce_pin, csn_pin), buffer(get_max_size()) { }

protected:
    mutable RF24 radio;
    Message buffer;
};

class MC_RF24_Transmitter : virtual public MC_RF24, public Transmitter {
public:
    MC_RF24_Transmitter(int ce_pin, int csn_pin) 
    : MC_RF24(ce_pin, csn_pin) { }

    virtual void send(Message &message) {
        buffer.fill_message(message);

        // TODO test/report write failure
        radio.write(buffer.data(), buffer.message_size());
    }

    void begin_writing(const char *write_address) {
        radio.openWritingPipe(reinterpret_cast<const uint8_t *>(write_address));
    }
};

class MC_RF24_Receiver : virtual public MC_RF24, public Receiver, public TickingTimer {
public:
    MC_RF24_Receiver(MessageDispatcher &listener, int ce_pin, int csn_pin, unsigned long tick_speed = 20000UL, int ring_capacity = 2) 
    : MC_RF24(ce_pin, csn_pin), Receiver(listener, ring_capacity), TickingTimer(tick_speed) {
    }

    virtual bool ready_for_tick(unsigned long now) {
        return TickingTimer::ready_for_tick(now);
    }

    virtual void tick() {
        if (radio.available()) {
            radio.read(buffer.data_ptr(), buffer.message_size());
            store(buffer);
        }

        notify_listener();
    }

    void begin_reading(const char *read_address) {
        radio.openReadingPipe(1, reinterpret_cast<const uint8_t *>(read_address));
        radio.startListening();
    }

    void pause_reading() {
        radio.stopListening();
    }

    void resume_reading() {
        radio.startListening();
    }
};

class MC_RF24_Transceiver : public MC_RF24_Transmitter, public MC_RF24_Receiver {
public:
    MC_RF24_Transceiver(MessageDispatcher &listener, int ce_pin, int csn_pin, unsigned long tick_speed = 20000UL, int ring_capacity = 2)
    : MC_RF24(ce_pin, csn_pin), MC_RF24_Transmitter(ce_pin, csn_pin), MC_RF24_Receiver(listener, ce_pin, csn_pin, tick_speed, ring_capacity) { }

    virtual void send(Message &message) {
        pause_reading();
        MC_RF24_Transmitter::send(message);
        resume_reading();
    }
};

};

#endif//__MC_RF24_H
