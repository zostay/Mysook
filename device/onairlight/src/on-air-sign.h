#ifndef __ON_AIR_SIGN_H
#define __ON_AIR_SIGN_H

#include <Firmware.h>
#include <Panel.h>
#include <Logger.h>

#define HEAD_SENTINEL 0xBEEF
#define TAIL_SENTINEL 0xDEAD
#define MESSAGE_SIZE 197

struct GridMessage {
    union {
        struct {
            uint16_t head;

            uint8_t brightness;
            Color grid[8][8];

            uint16_t tail;
        }

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
        return message.head = HEAD_SENTINEL 
            && message.tail = TAIL_SENTINEL;
    }
}

#define MESSAGE_LENGTH 197

class OnAirSign : public Firmware {
public:
    OnAirSign(mysook::Logger &logger, mysook::Panel &display) 
        : _logger(logger), _display(display) {}

    void write_bytes(char *buf, size_t bytes) {
        size_t used = 0;
        char *unused_buf = buf;
        while (used < bytes) {
            used += current_message.add_bytes(unused_buf, bytes);
            if (used < bytes) unused_buf = buf + used;

            if (current_message.is_complete()) {
                messages.push(current_message);
                current_message.reset();
            }
        }

        while (messages.size() > 0 && messages.front().is_error()) {
            messages.pop();
        }
    }

    void has_message() {
        return messages.size() > 0;
    }

    GridMessage get_message() {
        return messages.front();
    }

    void consume_message() {
        messages.pop();
    }

    virtual void start() {
        display.set_brightness(30);
        display.fill(Color(0, 0, 0));
    }

    virtual void tick() {
        if (has_message()) {
            GridMessage m = get_message();

            display.set_brightness(m.message.brightness);

            int i = 1;
            for (int y = 0; y < 8; ++y) {
                for (int x = 0; x < 8; ++x) {
                    display.put_pixel(m.message.grid[x][y]);
                }
            }

            display.draw();

            consume_message();
        }
    }

    virtual unsigned long tick_speed() { return 50000ul; }
    
private:
    GridMessage current;
    std::queue<GridMessage> messages;

    mysook::Logger &_logger;
    mysook::RGBPanel<8,8> &display;
}

#endif//__ON_AIR_SIGN_H

