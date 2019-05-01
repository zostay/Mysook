#ifndef __WEBSERVER_H
#define __WEBSERVER_H

#ifdef ARDUINO
#include <WiFiServer.h>
#include <ESPmDNS.h>
#else
#include <Simduino.h>
#include <SimWiFi.h>
#endif//ARDUINO

#include <functional>
#include <Firmware.h>

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

typedef std::function<bool(Request&,Response&)> WebDispatcher;

#define MAX_CLIENTS 4

enum RequestState {
    REQ_STATE_START,
    REQ_STATE_HEADER,
    REQ_STATE_BODY,
};

#define MAX_REQ_LENGTH 65535

struct ClientState {
    WiFiClient client;
    bool active;
    Request request;
    String buf;
    RequestState state;
    long bytes_read;

    void reset() {
        active     = false;
        client     = WiFiClient();
        request    = Request();
        buf        = String();
        state      = REQ_STATE_START;
        bytes_read = 0;
    }
};

class WebServer : public Ticking {
protected:
    Network &network;
    WiFiServer server;
    const WebDispatcher &dispatcher;
    Logger &log;

    bool start = false;

    ClientState conn[MAX_CLIENTS];

    void accept_client() {
        auto client = server.available();
        if (!client) return;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (!conn[i].client) {
                conn[i].active = true;
                conn[i].client = client;
                log.logf_ln("I [webserver] New client connected (%d)", i);
                return;
            }
        }
        
        // Too many clients if we get here. Shouldn't happen, right?
        log.logf_ln("E [webserver] Too many clients connected. Dropping client.");
        client.stop();
    }

    const String NOT_FOUND   = "Not Found";
    const String BAD_REQUEST = "Bad Request";
    const String BAD_METHOD  = "Bad Method";
    const String TOO_LARGE   = "Too Large";

    const String GET  = "GET";
    const String POST = "POST";

    void handle_request() {
        for (int i = 0; i < MAX_CLIENTS; ++i) {

            // Connection is closed, nothing left to do here.
            if (conn[i].active && !conn[i].client.connected()) {
                conn[i].reset();
                continue;
            }

            while (conn[i].client.available()) {
                char c = conn[i].client.read();
                if (++conn[i].bytes_read > MAX_REQ_LENGTH) {
                    log.logf_ln("E [webserver] client request is too long");

                    respond_with(i, 413, TOO_LARGE);
                    break;
                }

                //log.logf("%c", c);

                bool bad_request = false;
                bool bad_method = false;

                switch (conn[i].state) {
                case REQ_STATE_START:
                    if (c == '\n') {
                        int path_start = -1;
                        if (conn[i].buf.startsWith("GET ")) {
                            conn[i].request.method(GET);
                            path_start = 4;

                            //log.logf_ln("D [webserver] method GET (%s) is ok", conn[i].request.method().c_str());
                        }
                        else if (conn[i].buf.startsWith("POST ")) {
                            conn[i].request.method(POST);
                            path_start = 5;

                            //log.logf_ln("D [webserver] method POST (%s) is ok", conn[i].request.method().c_str());
                        }
                        else {
                            bad_method = true;

                            //log.logf_ln("D [webserver] method is bad");
                        }

                        int path_end = conn[i].buf.indexOf(' ', path_start);

                        if (path_start > 0 && path_end > 0) {
                            String path = conn[i].buf.substring(path_start, path_end);
                            conn[i].request.path(path);
                        }

                        //log.logf_ln("D [webserver] path (%s) is ok? %d", conn[i].request.path().c_str(), !bad_request);

                        String http_version = conn[i].buf.substring(path_end + 1);
                        if (!http_version.startsWith("HTTP/1.")) bad_request = true;
                        //log.logf_ln("D [webserver] HTTP version (%s) prefix is ok? %d", http_version.c_str(), !bad_request);
                        if (http_version[7] != '0' && http_version[7] != '1') bad_request = true;
                        //log.logf_ln("D [webserver] HTTP version (%s) suffix is ok? %d", http_version.c_str(), !bad_request);

                        conn[i].buf = String();

                        conn[i].state = REQ_STATE_HEADER;
                    }
                    else if (c != '\r') {
                        conn[i].buf += c;
                    }
                    break;

                case REQ_STATE_HEADER:
                    if (c == '\n') {
                        if (conn[i].buf.length() == 0) {
                            //log.logf_ln("D [webserver] headers are done");
                            if (conn[i].request.method() == GET || conn[i].request.content_length() == 0) {
                                handle_request(i);
                            }
                            else if (conn[i].request.content_length() > 0) {
                                conn[i].state = REQ_STATE_BODY;
                            }
                            else {
                                bad_request = true;
                            }
                        }
                        else {
                            int colon = conn[i].buf.indexOf(':');
                            if (colon > 0) {
                                String name = conn[i].buf.substring(0, colon);
                                String value = conn[i].buf.substring(colon + 1);

                                name.trim();
                                value.trim();

                                if (name.length() > 0) {
                                    //log.logf_ln("D [webserver] good header %s=%s", name.c_str(), value.c_str());
                                    conn[i].request.header(name, value);
                                }
                                else {
                                    //log.logf_ln("D [webserver] bad header %s=%s", name.c_str(), value.c_str());
                                    bad_request = true;
                                }
                            }
                            else {
                                //log.logf_ln("D [webserver] bad header line %s", conn[i].buf.c_str());
                                bad_request = true;
                            }
                        }

                        conn[i].buf = String();
                    }
                    else if (c != '\r') {
                        conn[i].buf += c;
                    }
                    break;

                case REQ_STATE_BODY:
                    int cl = conn[i].request.content_length();
                    if (cl >= 0) {
                        //log.logf_ln("D [webserver] POSTDATA %02X", c);
                        conn[i].buf += c;
                        //log.logf_ln("D [webserver] POSTDATA so far: %d, Content-Length: %d", conn[i].buf.length(), cl);

                        if (cl <= conn[i].buf.length()) {
                            //log.logf_ln("D [webserver] handling POST with POSTDATA %s", conn[i].buf.c_str());
                            conn[i].request.body(conn[i].buf);
                            handle_request(i);
                        }
                    }
                    else {
                        //log.logf_ln("D [webserver] bad stuff");
                        bad_request = true;
                    }
                    break;
                }

                if (bad_request)
                    respond_with(i, 400, BAD_REQUEST);
                else if (bad_method)
                    respond_with(i, 405, BAD_METHOD);
            }
        }
    }

    void respond_with(int i, int code, const String &msg) {
        log.logf_ln("I [webserver] Response (%d) %d %s", i, code, msg.c_str());

        conn[i].client.print("HTTP/1.0 ");
        conn[i].client.print(code);
        conn[i].client.print(" ");
        conn[i].client.print(msg);
        conn[i].client.print("\r\n\r\n");
        conn[i].client.print(msg);
        conn[i].client.stop();

        conn[i].reset();
    }

    void respond_with(int i, Response &res) {
        log.logf_ln("I [webserver] Response (%d) %d %s", i, res.status_code(), res.status_message().c_str());

        //log.logf(res.toString().c_str());
        conn[i].client.print(res.toString());

        conn[i].reset();
    }

    void handle_request(int i) {
        Response res;
        if (dispatcher(conn[i].request, res)) {
            respond_with(i, res);
        }
        else {
            respond_with(i, 404, NOT_FOUND);
        }
    }

    void ensure_started() {
        if (!server) {
            server.begin();
            log.logf_ln("I [webserver] HTTP server listening on port 80.");
        }
    }

    void ensure_stopped() {
        if (server) {
            log.logf_ln("W [webserver] HTTP server stopping due to loss of network.");
            server.end();
        }
    }

public:
    WebServer(Network &network, WebDispatcher &dispatcher, Logger &log) : network(network), server(80, MAX_CLIENTS), dispatcher(dispatcher), log(log) { 
        for (int i = 0; i < MAX_CLIENTS; ++i)
            conn[i].reset();
    }

    void begin() {
        start = true;
    }

    void end() {
        start = false;
    }

    virtual bool ready_for_tick(unsigned long now) { return true; }
    virtual void tick() {
        if (start && network.connected())
            ensure_started();
        else 
            ensure_stopped();

        accept_client();
        handle_request();
    }

};

};

#endif//__WEBSERVER_H
