#include <iostream>
#include <sstream>
#include <cstdlib>
#include "socket-utils/socket_io.h"

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        std::cerr << "Usage: db_client <host> <port> <username> <password>\n";
        return 1;
    }

    std::string host = argv[1];
    int port = std::atoi(argv[2]);
    std::string username = argv[3];
    std::string password = argv[4];

    if (!initSockets())
    {
        std::cerr << "Failed to initialize networking.\n";
        return 1;
    }

    SocketHandle sock = connectToServer(host, port);
    if (sock == INVALID_SOCKET_HANDLE)
    {
        std::cerr << "Could not connect to " << host << ":" << port << "\n";
        cleanupSockets();
        return 1;
    }

    std::ostringstream authLine;
    authLine << "AUTH " << username << " " << password;
    if (!sendMessage(sock, authLine.str()))
    {
        std::cerr << "Connection lost during authentication.\n";
        closeSocket(sock);
        cleanupSockets();
        return 1;
    }

    std::string authReply;
    if (!recvMessage(sock, authReply) || authReply != "AUTH OK")
    {
        std::cerr << "Authentication failed.\n";
        closeSocket(sock);
        cleanupSockets();
        return 1;
    }

    std::cout << "Connected to Project-X VectorDB server at " << host << ":" << port << "\n";

    std::string line;
    while (true)
    {
        std::cout << ">> ";
        if (!std::getline(std::cin, line))
            break;
        if (line.empty())
            continue;

        if (!sendMessage(sock, line))
        {
            std::cerr << "Connection to server lost.\n";
            break;
        }

        std::string response;
        if (!recvMessage(sock, response))
        {
            std::cerr << "Connection to server lost.\n";
            break;
        }

        std::cout << response;

        std::istringstream cmdStream(line);
        std::string cmd;
        cmdStream >> cmd;
        if (cmd == "EXIT")
            break;
    }

    closeSocket(sock);
    cleanupSockets();
    return 0;
}
