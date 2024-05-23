#ifndef __LITTLEFSREADER_H
#define __LITTLEFSREADER_H

#include <LittleFS.h>

namespace mysook {

class LittleFSReader : public Reader {
    fs::File &file;

public:
    LittleFSReader(fs::File &file) : file(file) { }

    virtual void read(char *buf, size_t len) {
        file.readBytes(buf, len);
    }

    virtual void seekg(size_t pos) {
        file.seek(pos, fs::SeekSet);
    }
};

};

#endif//__LITTLEFSREADER_H