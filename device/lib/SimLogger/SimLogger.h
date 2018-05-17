#ifndef __SIMLOGGER_H
#define __SIMLOGGER_H

#include <iostream>

namespace mysook {

class SimLogger : public Logger {
public:
    virtual void write_log(const char *str) { std::cerr << str << std::endl; }
};

};

#endif//__SIMLOGGER_H
