
#pragma once

#include <string>

namespace network
{

class IpAddress
{
private:
    unsigned int _addr;

public:
    IpAddress() = default;
    IpAddress(int addr);
    IpAddress(const char* addr);

public:
    void reset();

public:
    std::string toString() const;
    int toInteger() const;
};

};
