
#pragma once

#include "AbstractSocket.hpp"
#include "IpAddress.hpp"

#include <cstdlib> // std::size_t

namespace network
{

class TcpSocket
    : public AbstractSocket
{
public:
    TcpSocket() = default;
    virtual ~TcpSocket() = default;

public:
    virtual void create(int handle = -1) override;

public:
    void listen(const IpAddress& address, unsigned short port);
    void accept(TcpSocket& remoteSocket);

public:
    bool connect(const IpAddress& address, unsigned short remotePort, float timeout = 0.0f);
    void disconnect();

public:
    bool send(const void* data, std::size_t size, std::size_t& sent);
    bool receive(void* data, std::size_t size, std::size_t& received);
};

};
