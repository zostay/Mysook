#ifndef __SIM_WIFI_H
#define __SIM_WIFI_H

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <poll.h>

// Fake in WiFIClient and WiFiServer helpers provided by Arduino-ish libraries.

class WiFiClient : public Print { 
public:
    WiFiClient() : _sockfd(0), _connected(false) { }
    WiFiClient(int sockfd) : _sockfd(sockfd), _connected(true) { }

    void stop() { close(_sockfd); _connected = false; }

    bool connected() const { return _connected; }
    bool available() const {
        if (!_connected) return false;

        pollfd pfd = {
            .fd = _sockfd,
            .events = POLLIN,
        };

        return poll(&pfd, 1, 0) > 0;
    }

    int read() {
        char c;
        int len = recv(_sockfd, &c, 1, 0);
        if (len > 0) return c;
        else return -1;
    }

    operator bool() const { return available(); }
    bool operator! () const { return !available(); }
    WiFiClient &operator= (const WiFiClient &rhs) {
        _sockfd = rhs._sockfd;
        _connected = rhs._connected;
    }

private:
    int _sockfd;
    bool _connected;
};

class WiFiServer { 
public:
    WiFiServer(int port, int maxclients) : _port(port), _maxclients(maxclients) { }

    void begin() {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(_port);
        server.sin_addr.s_addr = htonl(INADDR_ANY);

        int reuse = 1;
        setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        fcntl(_sockfd, F_SETFL, O_NONBLOCK);

        bind(_sockfd, (sockaddr *) &server, sizeof(server));

        listen(_sockfd, _maxclients);
    }
    void end() { close(_sockfd); _listening = false; }

    WiFiClient available() const {
        if (!_listening) return WiFiClient();
        
        sockaddr_in client;
        socklen_t client_len = sizeof(client);
        int client_sockfd = accept(_sockfd, (sockaddr *) &client, &client_len);
        if (client_sockfd < 0) {
            return WiFiClient();
        }
        else {
            return WiFiClient(_sockfd);
        }
    }

    operator bool() const { return _listening; }
    bool operator! () const { return _listening; }

private:
    int _sockfd;
    int _port;
    int _maxclients;
    bool _listening = true;
};

#endif//__SIM_WIFI_H
