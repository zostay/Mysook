#ifndef __WEBTOOLS_H
#define __WEBTOOLS_H

namespace mysook {

struct Header {
    String name;
    String value;

    Header &operator= (const Header &rhs) {
        name  = rhs.name;
        value = rhs.value;
        return *this;
    }
};

class Message {
protected:
    Header *_headers = 0;
    int _header_count = 0;
    int header_buf = 5;

    String _body;

    void stretch_headers(int by = 1) {
        if (_header_count + by > header_buf) {
            Header *old = _headers;
            _headers = new Header[header_buf *= 2];

            for (int i = 0; i < _header_count; ++i) {
                _headers[i].name  = old[i].name;
                _headers[i].value = old[i].value;
            }

            delete[] old;
        }
    }

public:
    Message() : Message("") { }
    Message(String &body) : Message(body.c_str()) { }
    Message(const char *body) : _body(body) { 
        _headers = new Header[header_buf];
    }

    Message(Message &m) : _body(m._body), header_buf(m.header_buf), _header_count(m._header_count) {
        _headers = new Header[header_buf];
        for (int i = 0; i < _header_count; ++i) {
            _headers[i].name = m._headers[i].name;
            _headers[i].value = m._headers[i].value;
        }
    }
    virtual ~Message() { delete[] _headers; }

    Message &operator= (const Message &rhs) {
        _body = rhs._body;

        if (rhs._header_count > header_buf)
            stretch_headers(rhs._header_count - header_buf);

        _header_count = rhs._header_count;
        for (int i = 0; i < _header_count; ++i) {
            _headers[i].name = rhs._headers[i].name;
            _headers[i].value = rhs._headers[i].value;
        }

        return *this;
    }

    bool has_header(const String &name) const {
        return find_header(name) >= 0;
    }

    int find_header(const String &name) const {
        for (int i = 0; i < _header_count; ++i) {
            if (_headers[i].name == name) {
                return i;
            }
        }
        return -1;
    }

    int header_count() const { return _header_count; }

    Header deadheader;

    const Header &header(int i) const {
        if (i > _header_count)
            return deadheader;

        return _headers[i];
    }

    void header(int i, const String &value) {
        if (i > _header_count) return;
        _headers[i].value = value;
    }

    const Header &header(const String &name) const {
        for (int i = 0; i < _header_count; ++i) {
            if (_headers[i].name.equalsIgnoreCase(name)) {
                return _headers[i];
            }
        }

        return deadheader;
    }

    void header(const String &name, const String &value) {
        for (int i = 0; i < _header_count; ++i) {
            if (_headers[i].name.equalsIgnoreCase(name)) {
                _headers[i].value = value;
                return;
            }
        }

        stretch_headers();

        _headers[_header_count].name    = name;
        _headers[_header_count++].value = value;
    }

    const String &header_value(int i) const {
        return header(i).value;
    }

    const String &header_value(const String &name) const {
        return header(name).value;
    }

    const String CONTENT_LENGTH = "Content-Length";

    int content_length() const {
        int content_length = find_header(CONTENT_LENGTH);
        if (content_length) {
            return header_value(content_length).toInt();
        }
        else {
            return -1;
        }
    }

    const String &body() const { return _body; }
    void body(const String &body) { 
        _body = String("");
        for (int i = 0; i < body.length(); ++i)
            _body.concat(body[i]); 
    }

    void finalize() {
        for (int i = 0; i < this->header_count(); ++i) {
            _headers[i].name[0] = toUpperCase(_headers[i].name[0]);
        }
    }

    static Message *parseMessage(String &msg) {
        int eoh = msg.indexOf("\r\n\r\n");
        if (eoh < 0) return 0;

        Message *message = new Message();

        int here = 0;
        while (here < eoh) {
            int header_end = msg.indexOf("\r\n");
            int name_end = msg.indexOf(':');

            if (name_end > header_end) {
                delete message;
                return 0;
            }

            String name = msg.substring(here, name_end);
            String value = msg.substring(name_end + 1, header_end);

            name.trim();
            value.trim();

            message->header(name, value);
        }

        message->body(msg.substring(eoh + 4));

        return message;
    }
};

class Request : public Message {
protected:
    String _method;
    String _path;

public:
    Request() { }
    Request(const char *method, const char *path, const char *body = "") : _method(method), _path(path), Message(body) { }
    Request(String &method, String &path, String &body) : _method(method), _path(path), Message(body) { }
    Request(String &method, String &path, Message &msg) : _method(method), _path(path), Message(msg) { }

    const String &path() const { return _path; }
    void path(const String &path) { _path = path; }

    const String &method() const { return _method; }
    void method(const String &method) { _method = method; }

    Request &operator= (const Request &rhs) {
        Message::operator= (rhs);
        _method = rhs._method;
        _path   = rhs._path;
        return *this;
    }

    static Request *parseRequest(String &msg) {
        if (!(msg.startsWith("HTTP/1.") && (msg[7] == '0' || msg[7] == '1') && msg[8] == ' '))
            return 0;

        int method_end = msg.indexOf(' ', 9);
        int path_end = msg.indexOf("\r\n", method_end);

        String method = msg.substring(9, method_end);
        String path   = msg.substring(method_end + 1, path_end);

        String headers_on = msg.substring(path_end);
        Message *m = Message::parseMessage(headers_on);
        if (m == 0) return 0;

        Request *r = new Request(method, path, *m);
        delete m;
        return r;
    }
};

const String INTERNAL_ERROR = "Internal Server Error";

class Response : public Message {
protected:
    uint16_t _status_code;
    String _status_message;

public:
    Response(const char *body = "") : _status_code(200), _status_message("OK"), Message(body) { }
    Response(String &body) : _status_code(200), _status_message("OK"), Message(body) { }
    Response(int code, const char *status, const char *body = "") : _status_code(code), _status_message(status), Message(body) { }
    Response(int code, String &status, String &body) : _status_code(code), _status_message(status), Message(body) { }
    Response(int code, String &status, Message &msg) : _status_code(code), _status_message(status), Message(msg) { } 

    uint16_t status_code() const { return _status_code; }
    const String &status_message() const { return _status_message; }

    void status(uint16_t sc, const String &sm) {
        _status_code = sc;
        _status_message = sm;
    }

    void finalize() {
        Message::finalize();

        if (_status_code < 100 || _status_code > 599) {
            status(500, INTERNAL_ERROR);
            String empty = "";
            this->body(empty);
        }

        this->header(Message::CONTENT_LENGTH, String(this->body().length()));
    }

    String toString() {
        finalize();

        String res("HTTP/1.0 ");
        res += String(_status_code) + " " + _status_message + "\r\n";

        for (int i = 0; i < this->header_count(); ++i) {
            const Header &h = this->header(i);
            res += h.name + ": " + h.value + "\r\n";
        }

        res += "\r\n";
        res += this->body();

        return res;
    }
};

};

#endif//__WEBTOOLS_H
