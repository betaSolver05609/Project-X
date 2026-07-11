#include "socket_io.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

namespace
{
    bool sendAll(SocketHandle sock, const char *data, size_t len)
    {
        size_t sent = 0;
        while (sent < len)
        {
#ifdef _WIN32
            int n = send(sock, data + sent, static_cast<int>(len - sent), 0);
#else
            ssize_t n = send(sock, data + sent, len - sent, 0);
#endif
            if (n <= 0)
                return false;
            sent += static_cast<size_t>(n);
        }
        return true;
    }

    bool recvAll(SocketHandle sock, char *buffer, size_t len)
    {
        size_t received = 0;
        while (received < len)
        {
#ifdef _WIN32
            int n = recv(sock, buffer + received, static_cast<int>(len - received), 0);
#else
            ssize_t n = recv(sock, buffer + received, len - received, 0);
#endif
            if (n <= 0)
                return false;
            received += static_cast<size_t>(n);
        }
        return true;
    }
}

bool initSockets()
{
#ifdef _WIN32
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
    return true;
#endif
}

void cleanupSockets()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

SocketHandle createListenSocket(const std::string &bindAddress, int port, int backlog)
{
    SocketHandle sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET_HANDLE)
        return INVALID_SOCKET_HANDLE;

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&opt), sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if (bindAddress.empty() || bindAddress == "0.0.0.0")
    {
        addr.sin_addr.s_addr = INADDR_ANY;
    }
    else if (inet_pton(AF_INET, bindAddress.c_str(), &addr.sin_addr) != 1)
    {
        closeSocket(sock);
        return INVALID_SOCKET_HANDLE;
    }

    if (bind(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0)
    {
        closeSocket(sock);
        return INVALID_SOCKET_HANDLE;
    }

    if (listen(sock, backlog) != 0)
    {
        closeSocket(sock);
        return INVALID_SOCKET_HANDLE;
    }

    return sock;
}

SocketHandle acceptConnection(SocketHandle listenSocket)
{
    sockaddr_in clientAddr{};
#ifdef _WIN32
    int addrLen = sizeof(clientAddr);
#else
    socklen_t addrLen = sizeof(clientAddr);
#endif
    return accept(listenSocket, reinterpret_cast<sockaddr *>(&clientAddr), &addrLen);
}

SocketHandle connectToServer(const std::string &host, int port)
{
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo *result = nullptr;
    std::string portStr = std::to_string(port);
    if (getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result) != 0)
        return INVALID_SOCKET_HANDLE;

    SocketHandle sock = INVALID_SOCKET_HANDLE;
    for (addrinfo *p = result; p != nullptr; p = p->ai_next)
    {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == INVALID_SOCKET_HANDLE)
            continue;

        if (connect(sock, p->ai_addr, static_cast<int>(p->ai_addrlen)) == 0)
            break;

        closeSocket(sock);
        sock = INVALID_SOCKET_HANDLE;
    }

    freeaddrinfo(result);
    return sock;
}

void closeSocket(SocketHandle sock)
{
    if (sock == INVALID_SOCKET_HANDLE)
        return;
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

bool sendMessage(SocketHandle sock, const std::string &message)
{
    uint32_t len = static_cast<uint32_t>(message.size());
    uint32_t netLen = htonl(len);
    if (!sendAll(sock, reinterpret_cast<const char *>(&netLen), sizeof(netLen)))
        return false;
    if (len == 0)
        return true;
    return sendAll(sock, message.data(), len);
}

bool recvMessage(SocketHandle sock, std::string &out)
{
    uint32_t netLen = 0;
    if (!recvAll(sock, reinterpret_cast<char *>(&netLen), sizeof(netLen)))
        return false;
    uint32_t len = ntohl(netLen);

    // Guard against a corrupt/hostile length prefix forcing a huge allocation.
    constexpr uint32_t MAX_MESSAGE_SIZE = 64u * 1024u * 1024u;
    if (len > MAX_MESSAGE_SIZE)
        return false;

    out.resize(len);
    if (len == 0)
        return true;
    return recvAll(sock, &out[0], len);
}
