#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <cstdlib>
#include "common_utils/common_utils.h"
#include "handler/dispatcher.h"
#include "transaction-utils/TransactionContext.h"
#include "socket-utils/socket_io.h"
#include "server-utils/auth.h"

namespace
{
    constexpr int DEFAULT_PORT = 5454;
    constexpr size_t WORKER_COUNT = 8;

    // Thread-safe queue handing accepted connections from the accept loop to worker threads.
    class ConnectionQueue
    {
    public:
        void push(SocketHandle sock)
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                queue_.push(sock);
            }
            cv_.notify_one();
        }

        SocketHandle pop()
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return !queue_.empty(); });
            SocketHandle sock = queue_.front();
            queue_.pop();
            return sock;
        }

    private:
        std::queue<SocketHandle> queue_;
        std::mutex mutex_;
        std::condition_variable cv_;
    };

    // Runs one client's full session: AUTH, then a command loop. `dbMutex` protects both
    // KeySpace mutation and the std::cout redirection below. It's held per-command normally,
    // but kept held across an entire open transaction (BEGIN..COMMIT/ROLLBACK/disconnect) so
    // a second client can never commit work in the middle of another client's transaction —
    // TransactionContext's whole-DB snapshot would otherwise let one client's ROLLBACK erase
    // another client's concurrently-committed change.
    void handleClientSession(SocketHandle sock, KeySpace &db, std::mutex &dbMutex, const UserStore &users)
    {
        std::string authLine;
        if (!recvMessage(sock, authLine))
        {
            closeSocket(sock);
            return;
        }

        std::istringstream authStream(authLine);
        std::string authCmd, username, password;
        authStream >> authCmd >> username >> password;

        if (authCmd != "AUTH" || !users.authenticate(username, password))
        {
            sendMessage(sock, "AUTH FAILED");
            closeSocket(sock);
            return;
        }
        sendMessage(sock, "AUTH OK");

        bool holdingLock = false;
        std::string line;
        while (recvMessage(sock, line))
        {
            if (!holdingLock)
            {
                dbMutex.lock();
                holdingLock = true;
            }

            std::stringstream ss(line);
            std::ostringstream capture;
            std::streambuf *oldBuf = std::cout.rdbuf(capture.rdbuf());
            bool shouldExit = dispatchCommand(ss, db);
            std::cout.rdbuf(oldBuf);

            sendMessage(sock, capture.str());

            if (!TransactionContext::isActive())
            {
                dbMutex.unlock();
                holdingLock = false;
            }

            if (shouldExit)
                break;
        }

        if (holdingLock)
        {
            if (TransactionContext::isActive())
                TransactionContext::rollback(db);
            dbMutex.unlock();
        }

        closeSocket(sock);
    }
}

int main(int argc, char **argv)
{
    int port = argc > 1 ? std::atoi(argv[1]) : DEFAULT_PORT;
    std::string bindAddress = argc > 2 ? argv[2] : "0.0.0.0";
    std::string usersPath = argc > 3 ? argv[3] : "server-data/users.txt";

    if (!initSockets())
    {
        std::cerr << "Failed to initialize networking.\n";
        return 1;
    }

    UserStore users = UserStore::loadOrCreate(usersPath);

    KeySpace db;
    loadDb(db);
    std::mutex dbMutex;

    SocketHandle listenSocket = createListenSocket(bindAddress, port);
    if (listenSocket == INVALID_SOCKET_HANDLE)
    {
        std::cerr << "Failed to listen on " << bindAddress << ":" << port << "\n";
        cleanupSockets();
        return 1;
    }

    ConnectionQueue connections;
    std::vector<std::thread> workers;
    for (size_t i = 0; i < WORKER_COUNT; i++)
    {
        workers.emplace_back([&connections, &db, &dbMutex, &users]
        {
            while (true)
            {
                SocketHandle sock = connections.pop();
                handleClientSession(sock, db, dbMutex, users);
            }
        });
    }

    std::cout << "Project-X VectorDB server listening on " << bindAddress << ":" << port
              << " (" << WORKER_COUNT << " workers)\n";

    while (true)
    {
        SocketHandle client = acceptConnection(listenSocket);
        if (client == INVALID_SOCKET_HANDLE)
            continue;
        connections.push(client);
    }

    return 0;
}
