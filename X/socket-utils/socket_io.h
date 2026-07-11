#pragma once
#include <string>
#include <cstdint>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
using SocketHandle = SOCKET;
constexpr SocketHandle INVALID_SOCKET_HANDLE = INVALID_SOCKET;
#else
using SocketHandle = int;
constexpr SocketHandle INVALID_SOCKET_HANDLE = -1;
#endif

bool initSockets();
void cleanupSockets();

SocketHandle createListenSocket(const std::string &bindAddress, int port, int backlog = 64);
SocketHandle acceptConnection(SocketHandle listenSocket);
SocketHandle connectToServer(const std::string &host, int port);
void closeSocket(SocketHandle sock);

// Length-prefixed framing: a 4-byte big-endian length, then that many payload bytes.
// Used for both directions (client command -> server; captured output -> client).
bool sendMessage(SocketHandle sock, const std::string &message);
bool recvMessage(SocketHandle sock, std::string &out);
