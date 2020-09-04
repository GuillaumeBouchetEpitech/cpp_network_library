
#include "network/UdpSocket.hpp"
#include "network/SocketSelector.hpp"

#include <thread>
#include <mutex>
#include <chrono> // std::chrono::milliseconds
#include <iostream>

namespace /*anonymous*/
{

std::mutex g_mutex; // do not reproduct at home...

#define D_SAFE_LOG(streamMsg) \
{ \
    std::lock_guard<std::mutex> lock(g_mutex); \
    std::cout << streamMsg << std::endl; \
}

};

void tryUdpSocket()
{
    std::cout << "#######" << std::endl;
    std::cout << "# UDP #" << std::endl;
    std::cout << "#######" << std::endl;

    bool running = true;

    //
    //
    // SERVER

    std::thread serverEchoThread([&running]()
    {
        network::UdpSocket serverSocket;
        serverSocket.create();
        serverSocket.bind("0.0.0.0", 7777);

        std::size_t sdataSize = 512;
        auto data = std::make_unique<char[]>(sdataSize);
        char* sdata = data.get();

        D_SAFE_LOG("[server] echo thread running");

        // std::list<TcpSocket> echoSockets;
        network::SocketSelector serverSelector;
        serverSelector.add(serverSocket);

        while (running)
        {
            int total = serverSelector.waitRead(0.5f);

            if (total == 0)
                continue;

            std::size_t received = 0;
            network::IpAddress remoteAddress;
            unsigned short remotePort;
            serverSocket.receive(sdata, sdataSize, received, remoteAddress, remotePort);

            D_SAFE_LOG("[server] received from client : " << sdata);
            D_SAFE_LOG(" => remoteAddress : " << remoteAddress.toString());
            D_SAFE_LOG(" => remotePort    : " << remotePort);

            D_SAFE_LOG("[server] sending to client : " << sdata);

            serverSocket.send(sdata, sdataSize, remoteAddress, remotePort);
        }

        D_SAFE_LOG("[server] echo thread stopping");
    });

    // SERVER
    //
    //

    {
        //
        //
        // CLIENT

        network::UdpSocket clientSocket;
        clientSocket.create();
        // clientSocket.bind("127.0.0.1", 8888);

        std::thread clientReadThread([&running, &clientSocket]()
        {
            network::SocketSelector clientSocketSelector;
            clientSocketSelector.add(clientSocket);

            std::size_t sdataSize = 512;
            auto data = std::make_unique<char[]>(sdataSize);
            char* sdata = data.get();

            D_SAFE_LOG("[client] read thread running");

            while (running)
            {
                int total = clientSocketSelector.waitRead(0.5f);

                if (total == 0)
                    continue;

                std::size_t received = 0;
                network::IpAddress remoteAddress;
                unsigned short remotePort;
                clientSocket.receive(sdata, sdataSize, received, remoteAddress, remotePort);
                sdata[received] = '\0';

                D_SAFE_LOG("[client] received from server : " << sdata);
                D_SAFE_LOG(" => remoteAddress : " << remoteAddress.toString());
                D_SAFE_LOG(" => remotePort    : " << remotePort);
            }

            D_SAFE_LOG("[client] read thread stopping");
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(750));

        D_SAFE_LOG("[client] send thread running");

        for (int ii = 0; ii < 3; ++ii)
        {
            std::string payload = "Hello World";

            D_SAFE_LOG("[client] sending to server : " << payload);

            clientSocket.send(payload.c_str(), payload.size(), "127.0.0.1", 7777);

            std::this_thread::sleep_for(std::chrono::milliseconds(750));
        }

        D_SAFE_LOG("[client] send thread stopping");

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        D_SAFE_LOG("[client] shutting down experiment");

        running = false;

        if (clientReadThread.joinable())
            clientReadThread.join();
        if (serverEchoThread.joinable())
            serverEchoThread.join();

        // CLIENT
        //
        //
    }

    std::cout << "########" << std::endl;
    std::cout << "# /UDP #" << std::endl;
    std::cout << "########" << std::endl;
}
