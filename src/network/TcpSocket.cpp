
#include "TcpSocket.hpp"

#include "SocketSelector.hpp"

#include <stdexcept>

#include <iostream>

#include <cstring> // std::memset

#include <netinet/tcp.h> // TCP_NODELAY
#include <arpa/inet.h> // inet_addr
#include <fcntl.h> // fcntl, F_GETFL, F_SETFL, ~O_NONBLOCK

//

namespace network
{

void TcpSocket::create(int handle /*= -1*/)
{
    disconnect();

    if (handle >= 0)
        _handle = handle;
    else
        _handle = socket(PF_INET, SOCK_STREAM, 0);

    if (_handle == -1)
        throw std::runtime_error("failed to create socket");

    setAsBlocking(true);

    // disable the Nagle algorithm
    int yes = 1;
    if (setsockopt(_handle, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1)
        throw std::runtime_error("failed to set socket option \"TCP_NODELAY\"");

    int value = 1;
    if (setsockopt(_handle, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) == -1)
        throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
}

//

void TcpSocket::listen(const IpAddress& address, unsigned short port)
{
    if (_handle == -1)
        throw std::runtime_error("socket not initialised");

    // bind
    sockaddr_in addr = _getIpAddr(address, port);
    if (bind(_handle, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)
        throw std::runtime_error("failed to bind the socket");

    // listen
    if (::listen(_handle, SOMAXCONN) == -1)
        throw std::runtime_error("failed to listen the socket");
}

void TcpSocket::accept(TcpSocket& remoteSocket)
{
    if (_handle == -1)
        throw std::runtime_error("socket not initialised");

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    socklen_t length = sizeof(addr);

    int remoteHandle = ::accept(_handle, reinterpret_cast<sockaddr*>(&addr), &length);

    if (remoteHandle == -1)
        throw std::runtime_error("failed to accept the socket");

    remoteSocket.create(remoteHandle);
}

//

bool TcpSocket::connect(const IpAddress& remoteAddress, unsigned short remotePort, float timeout /*= 0.0f*/)
{
    if (_handle == -1)
        throw std::runtime_error("socket not initialised");

    sockaddr_in addr = _getIpAddr(remoteAddress, remotePort);

    if (timeout == 0.0f)
    {
        if (::connect(_handle, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)
            return false;
        return true;
    }

    bool wasBlocking = isBlocking();

    // Switch to non-blocking to enable our connection timeout
    if (wasBlocking)
        setAsBlocking(false);

    if (::connect(_handle, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) >= 0)
    {
        // std::cout << "instantly connected" << std::endl;
        setAsBlocking(wasBlocking);
        return true;
    }

    if (!wasBlocking)
        return true;

    SocketSelector selector;
    selector.add(*this);
    int total = selector.waitWrite(timeout);
    if (total == 0)
        return false;

    sockaddr_in address;
    socklen_t size = sizeof(address);
    int result = getpeername(_handle, reinterpret_cast<sockaddr*>(&address), &size);

    if (result != -1)
    {
        int remoteAddr = ntohl(address.sin_addr.s_addr);
        if (remoteAddr != 0)
        {
            setAsBlocking(true);
            return true;
        }
    }

    return false;
}

void TcpSocket::disconnect()
{
    close();
}

//

bool TcpSocket::send(const void* data, std::size_t size, std::size_t& sent)
{
    if (_handle == -1)
        throw std::runtime_error("socket not initialised");

    if (!data || size == 0)
        throw std::runtime_error("cannot send empty data");

    int result = 0;
    for (sent = 0; sent < size; sent += result)
    {
        result = ::send(_handle, static_cast<const char*>(data) + sent, static_cast<int>(size - sent), MSG_NOSIGNAL);

        if (result < 0)
            return false;
    }

    return true;
}

bool TcpSocket::receive(void* data, std::size_t size, std::size_t& received)
{
    if (_handle == -1)
        throw std::runtime_error("socket not initialised");

    if (!data || size == 0)
        throw std::runtime_error("cannot receive empty data");

    received = 0;

    int sizeReceived = recv(_handle, static_cast<char*>(data), static_cast<int>(size), MSG_NOSIGNAL);

    if (sizeReceived > 0)
        received = static_cast<std::size_t>(sizeReceived);
    else if (sizeReceived == 0)
        return false;
    return true;
}

};