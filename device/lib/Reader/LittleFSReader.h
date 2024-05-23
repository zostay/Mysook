#ifndef __LITTLEFSREADER_H
#define __LITTLEFSREADER_H

#include <LittleFS.h>

namespace mysook {

class LittleFSReader : public Reader {
    fs::File &file;

public:
    ArduinoFileDelegate(fs::File &file) : file(file) { }

    void read(char *buf, size_t len) {
        file.readBytes(buf, len);
    }

    void seekg(size_t pos) {
        file.seek(pos, fs::SeekSet);
    }
};

};

#endif//__LITTLEFSREADER_H