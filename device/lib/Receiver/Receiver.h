#ifndef __BUTTON_H
#define __BUTON_H

#include <string.h>

#include <functional>
#include <Firmware.h>
#include <Message.h>

namespace mysook {

class Receiver : public Ticking {

protected:
    Message *store(Message &incoming) {
        if (full())
            return 0;

        ring_start = (ring_start + ring_capacity - 1) % ring_capacity;
        Message &current = ring_buffer[ring_start];
        current.replace_message(incoming);
        
        return &current;
    }

    void notify_listener() {
        if (available()) {
            listener(*receive());
            consume();
        }
    }

public:
    Receiver(MessageDispatcher &listener, int ring_capacity = 2) 
    : listener(listener), ring_capacity(ring_capacity) { 
        ring_buffer = new Message[ring_capacity];
    }
    virtual ~Receiver() { 
        while (available())
            consume();
        delete[] ring_buffer;
    }

    bool available() { return ring_start != ring_end; }
    bool empty() { return !available(); }
    bool full() { return (ring_end + 1) % ring_capacity == ring_start; }

    const Message *receive() { 
        if (available()) {
            return &ring_buffer[ring_start];
        }
        else {
            return 0;
        }
    }

    void consume() { 
        ring_start = (ring_start + 1) % ring_capacity;
    }

protected:
    MessageDispatcher &listener;

    Message *ring_buffer;
    int ring_capacity;
    int ring_start = 0;
    int ring_end = 0;
};

};

#endif//__RECEIVER_H
