#ifndef __MC_NETWORK_H
#define __MC_NETWORK_H

#include <functional>

#include <Arduino.h>
#include <Network.h>
#include <Logger.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>

#define SYSTEM_EVENT_STA_CONNECTED    WIFI_EVENT_STAMODE_CONNECTED
#define SYSTEM_EVENT_STA_GOT_IP       WIFI_EVENT_STAMODE_GOT_IP
#define SYSTEM_EVENT_STA_DHCP_TIMEOUT WIFI_EVENT_STAMODE_DHCP_TIMEOUT
#define SYSTEM_EVENT_STA_DISCONNECTED WIFI_EVENT_STAMODE_DISCONNECTED
#else//ESP8266
#include <WiFi.h>
#endif//ESP8266

#define MAX_CONNECTION_MEMORY 10
#define CONNECTION_TIMEOUT 30000000ul
#define ANNOUNCEMENT_PERIOD 10000000ul

using std::placeholders::_1;

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

    WiFiEventHandler connected_handler;
    WiFiEventHandler got_ip_handler;
    WiFiEventHandler dhcp_timeout_handler;
    WiFiEventHandler disconnected_handler;

    unsigned long timeout = 0;
    unsigned long last_announcement = 0;

    bool scanning = false;

#ifdef ESP8266
    bool listening = false;
#else//ESP8266
    WiFiEventId_t listener_id = 0;
#endif//ESP8266

    bool should_connect = false;
    bool connect_status = false;

    Logger &logger;

    bool first_time = true;

    void handle_wifi_event(WiFiEvent_t event) {
        switch (event) {
            case SYSTEM_EVENT_STA_CONNECTED:
                logger.logf_ln("I [network] Connected %s", current->ssid);
                break;

            case SYSTEM_EVENT_STA_GOT_IP:
                connect_status = true;
                logger.logf_ln("I [network] IP assigned %s; IP %s", current->ssid, WiFi.localIP().toString().c_str());
                break;

            case SYSTEM_EVENT_STA_DHCP_TIMEOUT:
                logger.logf_ln("W [network] DHCP timeout");
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
        byte n;
        if (scanning) {
            n = WiFi.scanComplete();
            scanning = false;
        }
        else {
            n = WiFi.scanNetworks(true);
            scanning = true;
        }

        if (n == WIFI_SCAN_RUNNING) {
            return;
        }

        else if (n == WIFI_SCAN_FAILED) {
            logger.logf_ln("W [network] WiFi scan for networks failed");
            return;
        }

        else if (n < 0) {
            logger.logf_ln("E [network] Unknown error scanning for networks (%d)", n);
            return;
        }
        
        else if (n == 0) {
            logger.logf_ln("I [network] No WiFi networks detected");
            return;
        }

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
            if (last_announcement + ANNOUNCEMENT_PERIOD < micros()) {
                logger.logf_ln("W [network] No known SSIDs visible");
                last_announcement = micros();
            }

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
        
        // Make sure we will be notified of a disconnect
#ifdef ESP8266
        if (!listening) {
            logger.logf_ln("D [network] Registering callbacks");
            connected_handler = WiFi.onStationModeConnected([this](const WiFiEventStationModeConnected &) {
                this->handle_wifi_event(SYSTEM_EVENT_STA_CONNECTED);
            });
            got_ip_handler = WiFi.onStationModeGotIP([this](const WiFiEventStationModeGotIP &) {
                this->handle_wifi_event(SYSTEM_EVENT_STA_GOT_IP);
            });
            dhcp_timeout_handler = WiFi.onStationModeDHCPTimeout([this](void) {
                this->handle_wifi_event(SYSTEM_EVENT_STA_DHCP_TIMEOUT);
            });
            disconnected_handler = WiFi.onStationModeDisconnected([this](const WiFiEventStationModeDisconnected &) {
                this->handle_wifi_event(SYSTEM_EVENT_STA_DISCONNECTED);
            });

            listening = true;
        }
#else//ESP8266
        if (!listener_id) {
            logger.logf_ln("D [network] Registering callbacks");
            listener_id = WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
                this->handle_wifi_event(event);
            });
        }
#endif//ESP8266

        logger.logf_ln("I [network] Connecting to %s", current->ssid); 

        // Attempt to connect
        WiFi.begin(current->ssid, current->password);
    }

public:
    MC_Network(Logger &logger) : logger(logger) { }

    virtual bool connecting() { return should_connect && !connect_status; }
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

    virtual void tick() { if (should_connect) find_connection(); }
        
    void connect() { 
        should_connect = true;
        find_connection(); 
    }

    void disconnect() {
        should_connect = false;
        connect_status = false;
        current = 0;

#ifndef ESP8266
        if (listener_id) {
            WiFi.removeEvent(listener_id);
            listener_id = 0;
        }
#endif//ESP8266

        WiFi.disconnect();
    }
};

};

#endif//__MC_NETWORK_H
