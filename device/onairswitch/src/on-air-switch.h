#ifndef __ON_AIR_SWITCH_H
#define __ON_AIR_SWITCH_H

#include <functional>
#include <queue>

#include <Color.h>
#include <Firmware.h>
#include <Logger.h>
#include <MC_UDP.h>
#include <LED.h>

#define UDP_PORT 10101

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

#define GRID_ADDRESS 42

#define HEAD_SENTINEL 0xBEEF
#define TAIL_SENTINEL 0xDEAD
#define MESSAGE_SIZE 197

enum GridMessageOp {
    OP_NONE,
    OP_BRIGHTNESS,
    OP_PIXEL,
    OP_DRAW,
};
    
struct GridMessage {
    union {
        struct {
            uint16_t head;

            uint8_t brightness;
            mysook::Color grid[8][8];

            uint16_t tail;
        };

        char bytes[MESSAGE_SIZE];
    } message;

    int length = 0;

    void reset() {
        length = 0;
    }
    
    size_t add_bytes(char *buf, size_t count) {
        if (length + count > MESSAGE_SIZE) {
            size_t use = MESSAGE_SIZE - length;
            memcpy(message.bytes + length, buf, use);
            return use;
        }

        else if (is_complete()) {
            return 0;
        }

        else {
            memcpy(message.bytes + length, buf, count);
            return count;
        }
    }

    bool is_complete() {
        return length == MESSAGE_SIZE;
    }

    bool is_pending() {
        return length < MESSAGE_SIZE;
    }

    bool is_ready() {
        return is_complete() && is_valid();
    }

    bool is_error() {
        return length > MESSAGE_SIZE || (is_complete() && !is_valid());
    }

    bool is_valid() {
        return message.head == HEAD_SENTINEL 
            && message.tail == TAIL_SENTINEL;
    }
};

struct BrightnessMessage {
    uint8_t brightness;
};

struct PixelMessage {
    uint8_t x, y;
    uint8_t r, g, b;
};

class OnAirSwitch : public mysook::Firmware {
    void handle_udp_packet(String remote_ip, uint16_t remote_port, const char *buf, size_t len) {
        status_light.on();

        if (len == 197) {
            logf_ln("I [onairswitch] Received %d bytes from %s:%d",
                len,
                remote_ip.c_str(),
                remote_port
            );

            GridMessage grid;
            memcpy(&grid, buf, len);
            if (grid.is_ready()) {
                logf_ln("I [onairswitch] Valid message received from %s:%d",
                    remote_ip.c_str(),
                    remote_port
                );

                mq.push(grid);
            }
            else {
                logf_ln("E [onairswitch] Invalid message received from %s:%d",
                    remote_ip.c_str(),
                    remote_port
                );
            }
        }
        else {
            logf_ln("W [onairswitch] Ignore ill-sized %d bytes UDP message from %s:%d",
                len,
                remote_ip.c_str(),
                remote_port
            );
        }

        status_light.off();
    }

    void emit(uint8_t op) {
        Wire.beginTransmission(GRID_ADDRESS);
        Wire.write(op);
        Wire.endTransmission();
    }

    void emit(BrightnessMessage &b) {
        Wire.beginTransmission(GRID_ADDRESS);
        Wire.write(OP_BRIGHTNESS);
        Wire.write((char *) &b, sizeof(BrightnessMessage));
        Wire.endTransmission();

    }

    void emit(PixelMessage &p) {
        Wire.beginTransmission(GRID_ADDRESS);
        Wire.write(OP_PIXEL);
        Wire.write((char *) &p, sizeof(PixelMessage));
        Wire.endTransmission();
    }

public:

    OnAirSwitch(mysook::Logger &logger, mysook::Network &network, mysook::LED &status_light) 
    : Firmware(logger), network(network), status_light(status_light),
    udp_server(network, UDP_PORT, dispatcher, logger) {
        this->add_pre_ticker(&network);
        this->add_pre_ticker(&udp_server);
    }

    void transmit_message(GridMessage &grid) {
        BrightnessMessage b = { .brightness=grid.message.brightness };
        emit(b);

        PixelMessage p;
        for (uint8_t y = 0; y < 8; ++y) {
            for (uint8_t x = 0; x < 8; ++x) {
                p.x = x;
                p.y = y;
                p.r = grid.message.grid[x][y].r;
                p.g = grid.message.grid[x][y].g;
                p.b = grid.message.grid[x][y].b;
                emit(p);
            }
        }

        emit(OP_DRAW);

        logf_ln("I [main] Transmitted grid to I2C bus %d", GRID_ADDRESS);
    }

    virtual unsigned long tick_speed() { return 50000ul; }

    virtual void start() {}

    virtual void tick() {
        if (network.connecting()) {
            status_light.toggle();
        }
        else {
            status_light.off();
        }

        if (mq.size()) {
            GridMessage &grid = mq.front();
            transmit_message(grid);
            mq.pop();
        }
    }

private:
    char incoming[500];
    std::queue<GridMessage> mq;

    mysook::Network &network;
    mysook::LED &status_light;

    mysook::UdpDispatcher dispatcher = std::bind(&OnAirSwitch::handle_udp_packet, this, _1, _2, _3, _4);
    mysook::UdpListener udp_server;
};

#endif//__ON_AIR_SWITCH_H
