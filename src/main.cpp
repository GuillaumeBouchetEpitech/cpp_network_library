
#include "test.hpp"

#include <cstdlib> // EXIT_SUCCESS

int main()
{
    tryTcpSocket();
    tryUdpSocket();

    return EXIT_SUCCESS;
}
