
#pragma once

#include <netdb.h> // fd_set

namespace network
{

class AbstractSocket;

class SocketSelector
{
private:
    fd_set  _allSockets;
    fd_set  _socketsReady;
    int     _maxSocket = 0;

public:
    SocketSelector() = default;

public:
    void add(const AbstractSocket& socket);
    void remove(const AbstractSocket& socket);
    bool isReady(const AbstractSocket& socket);
    bool waitRead(float timeout = 0.0f);
    bool waitWrite(float timeout = 0.0f);
    void clear();
};

};
