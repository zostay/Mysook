#ifndef __SIMDUINO_H
#define __SIMDUINO_H

#include <inttypes.h>
#include <string>

char toUpperCase(char c);

class String {
public:
    String() { }
    String(uint16_t val) : str(std::to_string(val)) { }
    String(const char *str) : str(str) { }
    String(const String &str) : str(str.str) { }
    String(std::string &str) : str(str) { }

    const char *c_str() const;
    const char charAt(int at) const;
    void concat(const String &rhs);

    bool equalsIgnoreCase(const String &rhs) const;

    int indexOf(char c, int start = 0);
    int indexOf(const char *c, int start = 0);

    int length() const;

    char &operator[] (int i);
    const char operator[] (int i) const;
    String operator+ (const String &rhs) const;
    String &operator+= (const char *rhs);
    String &operator+= (const String &rhs);
    bool operator== (const String &rhs) const;

    bool startsWith(const char *needled) const;

    String substring(int start);
    String substring(int start, int end);

    int toInt() const;

    void trim();

private:
    std::string str;
};

class Print { 
public:
    void print(const char *t);
    void print(const String &s);

    virtual size_t write(uint8_t c) { return 0; };
};

typedef bool boolean;

#endif//__SIMDUINO_H
