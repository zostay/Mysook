#include <ctype.h>

#include "Simduino.h"

char toUpperCase(char c) { return std::toupper(c); }

const char *String::c_str() const {
    return str.c_str();
}

const char String::charAt(int i) const {
    return str.at(i);
}

void String::concat(const String &rhs) {
    str += rhs.str;
}

bool String::equalsIgnoreCase(const String &rhs) const {
    if (str.length() != rhs.str.length()) return false;
    for (int i = 0; i < str.length(); ++i) {
        if (std::tolower(str[i]) != std::tolower(rhs.str[i]))
            return false;
    }
    return true;
}

int String::indexOf(char c, int start) {
    int found_at = str.find_first_of(c, start);
    return found_at = std::string::npos ? -1 : found_at;
}

int String::indexOf(const char *t, int start) {
    int found_at = str.find_first_of(t, start);
    return found_at = std::string::npos ? -1 : found_at;
}

int String::length() const {
    return str.length();
}

char &String::operator[] (int i) {
    return str[i];
}

const char String::operator[] (int i) const {
    return str[i];
}

String String::operator+ (const String &rhs) const {
    std::string new_str = str + rhs.str;
    return String(new_str);
}

String &String::operator+= (const char *rhs) {
    str += rhs;
    return *this;
}

String &String::operator+= (const String &rhs) {
    str += rhs.str;
    return *this;
}

bool String::operator== (const String &rhs) const {
    return str == rhs.str;
}

bool String::startsWith(const char *needle) const {
    return str.find_first_of(needle) == 0;
}

String String::substring(int start) {
    std::string new_str = str.substr(start, std::string::npos);
    return String(new_str);
}

String String::substring(int start, int end) {
    std::string new_str = str.substr(start, end - start);
    return String(new_str);
}

int String::toInt() const {
    return std::stoi(str);
}

void String::trim() {
    int skip = 0;
    while (isspace(*(str.begin() + skip))) skip++;
    int rskip = 0;
    while (isspace(*(str.end() - rskip))) rskip++;

    str = str.substr(skip, str.length() - rskip);
}

void Print::print(const char *t) {
    for (int i = 0; t[i] != '\0'; ++i) {
        write(t[i]);
    }
}

void Print::print(const String &s) {
    for (int i = 0; i < s.length(); ++i) {
        write(s[i]);
    }
}
