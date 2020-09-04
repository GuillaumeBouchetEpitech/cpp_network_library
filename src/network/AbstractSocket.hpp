
#pragma once

#include "IpAddress.hpp"

#include <arpa/inet.h> // inet_addr

#include <cstdlib> // std::size_t

namespace network
{

class SocketSelector;

class AbstractSocket
{
    friend class SocketSelector;

protected:
    int _handle = -1;
    bool _isBLocking = true;

public:
    AbstractSocket() = default;
    virtual ~AbstractSocket() = default;

protected:
    sockaddr_in _getIpAddr(const IpAddress& address, unsigned short port);

public:
    virtual void create(int handle = -1) = 0;
    void close();

public:
    void setAsBlocking(bool blocking);
    bool isBlocking() const;
};

};
