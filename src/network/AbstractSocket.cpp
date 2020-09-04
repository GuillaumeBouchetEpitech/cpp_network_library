
#include "AbstractSocket.hpp"

#include <stdexcept>

#include <fcntl.h> // fcntl, F_GETFL, F_SETFL, ~O_NONBLOCK

#include <cstring> // std::memset
#include <unistd.h> // close()

namespace network
{

sockaddr_in AbstractSocket::_getIpAddr(const IpAddress& address, unsigned short port)
{
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = address.toInteger();
    addr.sin_port = htons(port);
    return addr;
}

void AbstractSocket::close()
{
    if (_handle == -1)
        return;

    ::close(_handle);
    _handle = -1;
}

//

void AbstractSocket::setAsBlocking(bool blocking)
{
    int status = fcntl(_handle, F_GETFL);
    if (blocking)
    {
        if (fcntl(_handle, F_SETFL, status & ~O_NONBLOCK) == -1)
            throw std::runtime_error("failed to set socket as blocking");
    }
    else
    {
        if (fcntl(_handle, F_SETFL, status | O_NONBLOCK) == -1)
            throw std::runtime_error("failed to unset socket as blocking");
    }

    _isBLocking = blocking;
}

bool AbstractSocket::isBlocking() const
{
    return _isBLocking;
}

};
