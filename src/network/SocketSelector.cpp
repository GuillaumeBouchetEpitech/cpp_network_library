
#include "SocketSelector.hpp"

#include "AbstractSocket.hpp"

#include <stdexcept>

namespace network
{

void SocketSelector::add(const AbstractSocket& socket)
{
    if (socket._handle == -1)
        throw std::runtime_error("trying to add an invalid socket");

    if (socket._handle >= FD_SETSIZE)
        throw std::runtime_error("trying to add too many sockets");

    _maxSocket = std::max(_maxSocket, socket._handle);

    FD_SET(socket._handle, &_allSockets);
}

void SocketSelector::remove(const AbstractSocket& socket)
{
    if (socket._handle == -1)
        throw std::runtime_error("trying to add an invalid socket");

    if (socket._handle >= FD_SETSIZE)
        return;

    FD_CLR(socket._handle, &_allSockets);
    FD_CLR(socket._handle, &_socketsReady);
}

bool SocketSelector::isReady(const AbstractSocket& socket)
{
    if (socket._handle == -1)
        throw std::runtime_error("trying to add an invalid socket");

    if (socket._handle >= FD_SETSIZE)
        return false;

    return (FD_ISSET(socket._handle, &_socketsReady) != 0);
}

bool SocketSelector::waitRead(float timeout /*= 0.0f*/)
{
    long int microSec = timeout * 1000000;

    timeval time;
    time.tv_sec  = static_cast<long int>(microSec / 1000000);
    time.tv_usec = static_cast<long int>(microSec % 1000000);

    _socketsReady = _allSockets;

    int count = select(_maxSocket + 1, &_socketsReady, NULL, NULL, timeout > 0.0f ? &time : NULL);

    return count > 0;
}

bool SocketSelector::waitWrite(float timeout /*= 0.0f*/)
{
    long int microSec = timeout * 1000000;

    timeval time;
    time.tv_sec  = static_cast<long int>(microSec / 1000000);
    time.tv_usec = static_cast<long int>(microSec % 1000000);

    _socketsReady = _allSockets;

    int count = select(_maxSocket + 1, NULL, &_socketsReady, NULL, timeout > 0.0f ? &time : NULL);

    return count > 0;
}

void SocketSelector::clear()
{
    FD_ZERO(&_allSockets);
    FD_ZERO(&_socketsReady);

    _maxSocket = 0;
}

};
