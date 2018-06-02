#ifndef __MC_NETWORK_H
#define __MC_NETWORK_H

#include <Arduino.h>
#include <Network.h>
#include <Logger.h>
#include <WiFi.h>

#define MAX_CONNECTION_MEMORY 10
#define CONNECTION_TIMEOUT 30000000ul

namespace mysook {

struct WiFiAP {
    const char *ssid;
    const char *password;
};

class MC_Network : public Network {
protected:
    int conns = 0;
    WiFiAP memory[MAX_CONNECTION_MEMORY];
	WiFiAP *current = 0;

    unsigned long timeout = 0;

    WiFiEventId_t listener_id = 0;

    bool connect_status = false;

    Logger &logger;

    bool first_time = true;

	void handle_wifi_event(WiFiEvent_t event, WiFiEventInfo_t info) {
        switch (event) {
            case SYSTEM_EVENT_STA_GOT_IP:
                connect_status = true;
                logger.logf_ln("I [network] Connected %s; IP %s", current->ssid, WiFi.localIP().toString().c_str());
                break;

            case SYSTEM_EVENT_STA_DISCONNECTED:
                connect_status = false;
                logger.logf_ln("W [network] Disconnected from %s", current->ssid);
                break;

            default:
                break;
        }
    }

    void find_connection() {
        // nothing to do, we're connected
        if (connect_status) return;

        // unable to connect because we are not configured
        if (conns == 0) {
            logger.logf_ln("E [network] No SSIDs configured. Connection is impossible.");
            return;
        }

        if (first_time) {
            WiFi.mode(WIFI_STA);
            WiFi.disconnect();
            first_time = false;
        }

        // Scan the networks to find the best available
        byte n = WiFi.scanNetworks();

        WiFiAP *best_conn = 0;
        long best_signal = -1000;

        for (int i = 0; i < n; ++i) {
            for (int c = 0; c < conns; ++c) {
                if (WiFi.SSID(i) == memory[c].ssid && WiFi.RSSI(i) > best_signal) {
                    best_signal = WiFi.RSSI(i);
                    best_conn = &memory[c];
                }
            }
        }

        // No configured network was found
        if (best_conn == 0) {
            logger.logf_ln("W [network] No allowed SSIDs visible");
            current = &memory[0];
        }

        // A network was found, pick it and connect
        else {
            timeout = micros() + CONNECTION_TIMEOUT;
            current = best_conn;
            begin_connection();
        }
    }

    void begin_connection() {
        if (!current) return;
        
        logger.logf_ln("I [network] Connecting to %s", current->ssid); 

        // Make sure we will be notified of a disconnect
        if (!listener_id) {
            listener_id = WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
                this->handle_wifi_event(event, info);
            });
        }

        // Attempt to connect
        WiFi.begin(current->ssid, current->password);
    }

public:
    MC_Network(Logger &logger) : logger(logger) { }

    virtual bool connected() { return connect_status; }
    virtual uint8_t signal_strength() {
        long base_value = WiFi.RSSI();
        if (base_value < -100) {
            return 0;
        }
        else if (base_value > -30) {
            return 255;
        }
        else {
            return (base_value + 100) * 255 / 70;
        }
    }

    void add_access_point(const char *ssid, const char *password) {
        logger.logf_ln("I [network] Search SSID %s", ssid);

        memory[conns].ssid     = ssid;
        memory[conns].password = password;
        conns++;
    }

    virtual bool ready_for_tick(unsigned long now) {
        return now >= timeout;
    }

    virtual void tick() { connect(); }
        
    void connect() { find_connection(); }

    void disconnect() {
        connect_status = false;
        current = 0;

        if (listener_id) {
            WiFi.removeEvent(listener_id);
            listener_id = 0;
        }

        WiFi.disconnect();
    }
};

};

#endif//__MC_NETWORK_H
