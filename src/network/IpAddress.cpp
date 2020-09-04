
#include "IpAddress.hpp"

#include <arpa/inet.h> // inet_addr

namespace network
{

IpAddress::IpAddress(int addr)
    : _addr(addr)
{}

IpAddress::IpAddress(const char* addr)
    : _addr(inet_addr(addr))
{}

//

void IpAddress::reset()
{
    _addr = 0;
}

//

std::string IpAddress::toString() const
{
    struct in_addr tmpAddr;
    // tmpAddr.s_addr = htonl(_addr);
    tmpAddr.s_addr = _addr;
    return inet_ntoa(tmpAddr);
}

int IpAddress::toInteger() const
{
    return _addr;
}

};
