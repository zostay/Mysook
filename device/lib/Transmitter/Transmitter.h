#ifndef __TRANSMITTER_H
#define __TRANSMITTER_H

#include <Message.h>

namespace mysook {

class Transmitter {
public:
    virtual void send(Message &message) = 0;
};

};

#endif//__TRANSMITTER_H
