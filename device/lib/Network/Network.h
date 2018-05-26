#ifndef __NETWORK_H
#define __NETWORK_H

namespace mysook {

class Network {
public:
	virtual ~Network() { }

    virtual uint8_t signal_strength() { return 127; }
    virtual bool connected() { return true; }

    // These are mostly here to give a sense of the scale I am aiming for.
    // Each of these roughly corresponds to 1-5 bars you'd see in a wifi signal
    // display icon. Anything at okay or above ought to be workable.
    virtual bool is_signal_excellent() { return signal_strength() > 127; } // ~ -65dbm
    virtual bool is_signal_good() { return signal_strength() > 110; }      // ~ -70dbm
    virtual bool is_signal_okay() { return signal_strength() > 90; }       // ~ -75dbm
    virtual bool is_signal_poor() { return signal_strength() > 70; }       // ~ -80dbm
    virtual bool is_signal_terrible() { return signal_strength() <= 70; }
};

};

#endif//__NETWORK_H
