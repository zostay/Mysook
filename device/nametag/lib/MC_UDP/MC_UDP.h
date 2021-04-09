#ifndef __MC_UDP_H
#define __MC_UDP_H

#include <UDP.h>

namespace mysook {

class MC_UdpListener : public UdpListener { 
public:
    MC_UdpListener(Network &net) : UdpListener(net) { }

};

};

#endif//__MC_UDP_H
