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
#include <WebTools.h>

namespace mysook {

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

    ResponseParser rp[MAX_CLIENTS];

    void accept_client() {
        auto client = server.available();
        if (!client) return;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (!rp[i].active()) {
                rp.start(client);
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
            if (rp[i].stale()) {
                rp[i].reset();
                continue;
            }

            if (rp[i].process()) {
                handle_request(i);
            }
        }
    }

    void respond_with(int i, int code, const String &msg) {
        log.logf_ln("I [webserver] Response (%d) %d %s", i, code, msg.c_str());

        rp[i].client.print("HTTP/1.0 ");
        rp[i].client.print(code);
        rp[i].client.print(" ");
        rp[i].client.print(msg);
        rp[i].client.print("\r\n\r\n");
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
