#ifndef __MESSAGE_H
#define __MESSAGE_H

namespace mysook {

class Message {
public:
    Message(size_t message_size = 32)
    : _capacity(message_size), _message_size(message_size), _data(new char[message_size]) {
        memset(_data, 0, message_size);
    }
    Message(size_t message_size, const char *data) : Message(message_size) {
        memcpy(_data, data, message_size);
    }
    virtual ~Message() { delete[] _data; }

    size_t capacity() const { return _capacity; }
    size_t message_size() const { return _message_size; }
    const char *data() const { return _data; }

    char *data_ptr() { return _data; }

    void set_message_size(size_t message_size) { 
        if (message_size > _capacity) {
            delete[] _data;
            _data = new char[_message_size];
            _capacity = message_size;
        }

        _message_size = message_size;
    }

    void replace_message(size_t message_size, const char *data) {
        set_message_size(message_size);
        memcpy(_data, data, message_size);
    }

    void replace_message(Message &other) {
        replace_message(other.message_size(), other.data());
    }

    bool fill_message(size_t data_size, const char *data, size_t message_size = 0) {
        if (message_size == 0)
            message_size = _message_size;

        if (data_size > message_size) 
            return false;

        set_message_size(message_size);
        if (data_size < message_size)
            memset(_data + data_size, 0, message_size - data_size);
        memcpy(_data, data, data_size);

        return true;
    }

    bool fill_message(Message &other, size_t message_size = 0) {
        return fill_message(other.message_size(), other.data(), message_size);
    }

private:
    size_t _capacity;
    size_t _message_size;
    char *_data;
};

typedef std::function<void(const Message &)> MessageDispatcher;

};

#endif//__MESSAGE_H
