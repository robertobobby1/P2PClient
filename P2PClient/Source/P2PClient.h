#pragma once

#include <thread>

#include "R.h"
#include "PeerServer.h"
#include "PeerClient.h"

namespace P2PClient {
    inline const char* EXTERNAL_SERVER_IP = "localhost";
    inline int EXTERNAL_SERVER_PORT = 3000;

    inline int SERVER_PORT = 22000;
    inline int BACKLOG = 10;
    inline const int MAX_SERVER_STARTUP_RETRIES = 5;

    inline std::thread CLIENT_THREAD;
    inline std::thread SERVER_THREAD;

    struct NetworkData {
        // Vec2i playerPos
        // Only when ball collides with my player
        // int ballID
        // Vec2i ballPos
        // Vec2i ballVelocity
    };

    inline void init() {
        SERVER_THREAD = std::thread(PeerServer::run, SERVER_PORT, BACKLOG, MAX_SERVER_STARTUP_RETRIES);
        CLIENT_THREAD = std::thread(PeerClient::run, EXTERNAL_SERVER_IP, EXTERNAL_SERVER_PORT);
    }
    inline void wait() {
        CLIENT_THREAD.join();
        SERVER_THREAD.join();
    }
    inline void terminate() {
        PeerServer::terminate();
        PeerClient::terminate();
        wait();
    }

    // int[] ParseData(NetworkData data);
}  // namespace P2PClient
