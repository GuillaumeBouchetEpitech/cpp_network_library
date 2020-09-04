
#pragma once

#include "AbstractSocket.hpp"
#include "IpAddress.hpp"

#include <string>

#include <cstdlib> // std::size_t

namespace network
{

class UdpSocket
    : public AbstractSocket
{
public:
    UdpSocket() = default;
    virtual ~UdpSocket() = default;

public:
    virtual void create(int handle = -1) override;

public:
    void bind(const IpAddress& address, unsigned short port);
    void unbind();

public:
    void send(
        const void* data,
        std::size_t size,
        const IpAddress& remoteAddress,
        unsigned short remotePort
    );
    void receive(
        void* data,
        std::size_t size,
        std::size_t& received,
        IpAddress& remoteAddress,
        unsigned short& remotePort
    );
};

};
