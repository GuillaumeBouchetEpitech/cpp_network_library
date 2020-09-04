
#include "UdpSocket.hpp"

#include <stdexcept>

#include <cstring> // std::memset

#include <netinet/tcp.h> // TCP_NODELAY
#include <arpa/inet.h> // inet_addr

//

namespace network
{

void UdpSocket::create(int handle /*= -1*/)
{
    close();

    if (handle >= 0)
        _handle = handle;
    else
        _handle = socket(PF_INET, SOCK_DGRAM, 0);

    if (_handle == -1)
        throw std::runtime_error("failed to create socket");

    setAsBlocking(true);

    int value = 1;
    if (setsockopt(_handle, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) == -1)
        throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");

    int yes = 1;
    if (setsockopt(_handle, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes)) == -1)
        throw std::runtime_error("setsockopt(SO_BROADCAST) failed");
}

//

void UdpSocket::bind(const IpAddress& address, unsigned short port)
{
    if (_handle == -1)
        throw std::runtime_error("socket not initialised");

    // bind
    sockaddr_in addr = _getIpAddr(address, port);
    if (::bind(_handle, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)
        throw std::runtime_error("failed to bind the socket");
}

void UdpSocket::unbind()
{
    close();
}

//

void UdpSocket::send(
    const void* data,
    std::size_t size,
    const IpAddress& remoteAddress,
    unsigned short remotePort
)
{
    if (_handle == -1)
        throw std::runtime_error("socket not initialised");

    sockaddr_in addr = _getIpAddr(remoteAddress, remotePort);

    int sent = sendto(
        _handle,
        static_cast<const char*>(data),
        static_cast<int>(size),
        0,
        reinterpret_cast<sockaddr*>(&addr),
        sizeof(addr)
    );

    if (sent < 0)
        throw std::runtime_error("failed to send");
}

void UdpSocket::receive(
    void* data,
    std::size_t size,
    std::size_t& received,
    IpAddress& remoteAddress,
    unsigned short& remotePort
)
{
    if (_handle == -1)
        throw std::runtime_error("socket not initialised");

    received = 0;
    remoteAddress.reset();
    remotePort = 0;

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(0);

    socklen_t addressSize = sizeof(addr);

    //

    int sizeReceived = recvfrom(
        _handle,
        static_cast<char*>(data),
        static_cast<int>(size),
        0,
        reinterpret_cast<sockaddr*>(&addr),
        &addressSize
    );

    if (sizeReceived < 0)
        throw std::runtime_error("failed to receive");

    //

    received = static_cast<std::size_t>(sizeReceived);

    // remoteAddress = htonl(addr.sin_addr.s_addr);
    remoteAddress = addr.sin_addr.s_addr;

    remotePort = ntohs(addr.sin_port);
}

};
