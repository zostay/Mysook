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

    void finalize() {
        message::finalize();

        if (_method == GET) {
            string empty = "";
            this->body(empty);
        }

        this->header(message::content_length, string(this->body().length()));
    }

    Request &operator= (const Request &rhs) {
        Message::operator= (rhs);
        _method = rhs._method;
        _path   = rhs._path;
        return *this;
    }

    String toString() {
        finalize();

        String req("HTTP/1.0 ");
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
        message::finalize();

        if (_status_code < 100 || _status_code > 599) {
            status(500, INTERNAL_ERROR);
            string empty = "";
            this->body(empty);
        }

        this->header(message::content_length, string(this->body().length()));
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

enum MessageParserMode {
    MPM_START,
    MPM_HEADER,
    MPM_BODY,
    MPM_DONE,
    MPM_ERROR
};

enum MessageParserError {
    MPE_UNKNOWN,
    MPE_TOO_LONG,
    MPE_BAD_REQUEST,
    MPE_BAD_METHOD,
    MPE_SERVER_ERROR,
};

#define HANDLE_DONE_PARSER(done) \
    if (done) { \
        if (!has_error()) state = MPM_DONE; \
        return done; \
    }

template <MessageClass>
class MessageParser<MessageClass> {
private:
    WiFiClient client;
    bool active = true;
    MessageClass message;
    String buf;
    MessageParserMode state = MPM_START;
    long bytes_read = 0;
    MessageParserError error = MPE_UNKNOWN;

    void set_error(const String &error) {
        state = MPM_ERROR;
        this.error = error;
    }

public:
    MessageParser() { }

    void reset() {
        active     = false;
        message    = MessageClass();
        client     = WiFiClient();
        buf        = String();
        state      = MPM_START;
        bytes_read = 0;
        has_error  = false;
        error      = MPE_UNKNOWN;
    }

    bool active() { return active; }
    bool connected() { return client.connected(); }
    bool stale() { return active() && !connected(); }
    bool finished() { return state == MPM_DONE || state == MPM_ERROR; }
    bool ready() { return client.available(); }

    bool has_error() { return state == MPM_ERROR; }
    const String &error() { return error; }

    void start(WiFiClient &client) {
        this.active = true;
        this.client = client;
    }

    bool process() {
        if (finished()) return true;

        while (client.available()) {
            char c = client.read();
            if (++bytes_read > MAX_MSG_LENGTH) {
                set_error(MPE_TOO_LARGE);
                return true;
            }

            bool done = false;

            switch (state) {
            case MPM_START:
                if (c == '\n') {
                    done = process_start_line();

                    HANDLE_DONE_PARSER(done);

                    state = MPM_HEADER;
                }
                else if (c != '\r') {
                    buf += c;
                }
                break;

            case MPM_HEADER:
                if (c == '\n') {
                    if (buf.length() == 0) {
                        done = process_end_of_header();

                        HANDLE_DONE_PARSER(done);

                        state = MPM_BODY;
                    }
                    else {
                        int colon = buf.indexOf(':');
                        if (colon > 0) {
                            String name = buf.substring(0, colon);
                            String value = buf.substring(colon + 1);

                            name.trim();
                            value.trim();

                            if (name.length() > 0) {
                                request.header(name, value);
                            }
                            else {
                                set_error(MPE_BAD_REQUEST);
                                return true;
                            }
                        }
                        else {
                            set_error(MPE_BAD_REQUEST);
                            return true;
                        }
                    }

                    buf = String();
                }
                else if (c != '\r') {
                    buf += c;
                }
                break;

            case MPM_BODY:
                int cl = request.content_length();
                if (cl >= 0) {
                    buf += c;

                    if (cl <= buf.length()) {
                        request.body(buf);
                        state = MPM_DONE;
                        return true;
                    }
                }
                else {
                    set_error(MPE_BAD_REQUEST);
                    return true;
                }
                break;
            }
        }

        return false;
    }

    virtual bool process_start_line() = 0;
    virtual bool process_end_of_header() = 0;

    MessageClass &message() { return message; }
};

class RequestParser : public MessageParser<Request> {
public:
    bool process_start_line() {
        int path_start = -1;
        if (buf.startsWith("GET ")) {
            message.method(GET);
            path_start = 4;
        }
        else if (buf.startsWith("POST ")) {
            message.method(POST);
            path_start = 5;
        }
        else {
            set_error(MPE_BAD_METHOD);
            return true;
        }

        int path_end = buf.indexOf(' ', path_start);

        if (path_start > 0 && path_end > 0) {
            String path = buf.substring(path_start, path_end);
            request.path(path);
        }

        String http_version = buf.substring(path_end + 1);
        if (!http_version.startsWith("HTTP/1.")) {
            set_error(MPE_BAD_REQUEST);
            return true;
        }

        if (http_version[7] != '0' && http_version[7] != '1') {
            set_error(MPE_BAD_REQUEST);
            return true;
        }

        buf = String();

        return false;
    }

    bool process_end_of_header() {
        if (request.method() == GET || request.content_length() == 0) {
            return true;
        }
        else if (request.content_length() > 0) {
            return false;
        }
        else {
            set_error(MPE_BAD_REQUEST);
            return true;
        }
    }

    Request &request() { return message(); }
};

class ResponseParser : public MessageParser<Response> {
    bool process_start_line() {
        int path_start = -1;

        if (!buf.startsWith("HTTP/1.")) {
            set_error(MPE_SERVER_ERROR);
            return true;
        }

        if (buf[7] != '0' && buf[7] != '1') {
            set_error(MPE_SERVER_ERROR);
            return true;
        }

        String http_version = buf.substring(0, 8);

        if (buf[8] != ' ' || buf[12] != ' ') {
            set_error(MPE_SERVER_ERROR);
            return true;
        }

        String code = buf.substring(9, 12);
        if (code[0] < '1' || code[0] > '5') {
            set_error(MPE_SERVER_ERROR);
            return true;
        }

        if (code[1] < '0' || code[1] > '9') {
            set_error(MPE_SERVER_ERROR);
            return true;
        }

        if (code[2] < '0' || code[1] > '9') {
            set_error(MPE_SERVER_ERROR);
            return true;
        }

        String message = buf.substring(13);
        if (message.length() == 0) {
            set_error(MPE_SERVER_ERROR);
            return true;
        }

        status(code.toInt(), message);

        return false;
    }

    bool process_end_of_header() { return true; }

    Response &response() { return message(); }
};

};

#endif//__WEBTOOLS_H
