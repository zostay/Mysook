#ifndef __NAMETAG_FETCH_H
#define __NAMETAG_FETCH_H

#include <ArduinoJson.h>
#include <HTTPClient.h>

#include <Logger.h>

class WebProgramInfo {
private:
    int program_id;
    String name;
    String author;
    uint32_t *program;

    int write_ptr = 0;
    bool write_locked = false;

    int refcount = 1;

public:
    WebProgramInfo(int program_id, const String &name, const String &author, int capacity)
    : program_id(program_id), name(name), author(author), program(new uint32_t[capacity]) { 
    }

    ~WebProgramInfo() { delete[] program; }

    void write(uint32_t word) { 
        if (!write_locked) program[write_ptr++] = word; 
    }
    void lock() { write_locked = true; }

    int get_program_id() const { return program_id; }
    const String &get_name() const { return name; }
    const String &get_author() const { return author; }
    const uint32_t *get_program() const { return program; }

    void refinc() { refcount++; }
    void refdec() { refcount--; }
    int get_refcount() const { return refcount; }
};

class NameTagFetch {
private:
    mysook::Logger &log;

    const char *base_url;
    const char *ca_cert;

    StaticJsonDocument<6000> qdata;

    WebProgramInfo *programs[32];
    int length = 0;
    int index = 0;

    void add_program(WebProgramInfo *program);
    bool readd_program(int program_id);
    void clear_programs();

public:
    NameTagFetch(mysook::Logger &log, const char *base_url, const char *ca_cert) : log(log), base_url(base_url), ca_cert(ca_cert) { }
    virtual ~NameTagFetch() { clear_programs(); }

    bool fetch_queue(const char *queue_name);

    WebProgramInfo *get_program(int i) {
        return 0 <= i && i < length ? programs[i] : 0;
    }

    WebProgramInfo *next_program(bool loop = true) { 
        if (loop && index >= length)
            index = 0;

        return get_program(index++);
    }

    int programs_length() { return length; }
};

#endif//__NAMETAG_FETCH_H
