
#pragma once

#include <thread>

#include "R.h"
#include "PeerServer.h"
#include "PeerClient.h"
#include "Shared.h"

namespace P2PClient {
    inline const char* EXTERNAL_SERVER_IP = "localhost";
    inline int EXTERNAL_SERVER_PORT = 3000;

    inline int SERVER_PORT = 22000;
    inline int BACKLOG = 10;
    inline const int MAX_SERVER_STARTUP_RETRIES = 5;

    inline std::thread CLIENT_THREAD;
    inline std::thread SERVER_THREAD;

    inline bool arePeersConnected = false;
    inline std::shared_ptr<R::Net::Client> peerServerSocket;
    inline std::shared_ptr<R::Net::Client> peerClientSocket;

    inline void wait() {
        CLIENT_THREAD.join();
        SERVER_THREAD.join();
    }

    inline void terminate() {
        PeerServer::terminate();
        PeerClient::terminate();
        wait();
    }

    inline void init() {
        SERVER_THREAD = std::thread(PeerServer::run, SERVER_PORT, BACKLOG, MAX_SERVER_STARTUP_RETRIES);
        CLIENT_THREAD = std::thread(PeerClient::run, EXTERNAL_SERVER_IP, EXTERNAL_SERVER_PORT);

        Shared::isPeerServerSocketConnected.wait(false);
        Shared::isPeerClientSocketConnected.wait(false);

        terminate();
        RLog("[Initializer] Peer's have connected, sockets are %i and %i\n", Shared::peerClientSocket, Shared::peerServerSocket);
        peerServerSocket = R::Net::Client::makeAndSet(Shared::peerClientSocket);
        peerClientSocket = R::Net::Client::makeAndSet(Shared::peerServerSocket);

        auto x = 5;
    }

}  // namespace P2PClient
