#ifndef __READER_H
#define __READER_H

#include <cstddef>
#include <istream>

namespace mysook {

class Reader {
public:
    virtual void read(char *buf, std::size_t len) = 0;
    virtual void seekg(std::size_t pos) = 0;
};

class IStreamReader : public Reader, public std::istream {
};

};

#endif//__READER_H