#pragma once

#include <thread>

#include "R.h"
#include "PeerServer.h"
#include "PeerClient.h"

namespace P2PClient {
    inline std::thread CLIENT_THREAD;
    inline std::thread SERVER_THREAD;

    struct NetworkData {
        // Vec2i playerPos
        // Only when ball collides with my player
        // int ballID
        // Vec2i ballPos
        // Vec2i ballVelocity
    };

    void init() {
        SERVER_THREAD = std::thread(PeerServer::run);
        CLIENT_THREAD = std::thread(PeerClient::run);
    }
    void wait() {
        CLIENT_THREAD.join();
        SERVER_THREAD.join();
    }
    void terminate() {
        PeerServer::terminate();
        PeerClient::terminate();
        wait();
    }

    // int[] ParseData(NetworkData data);
}  // namespace P2PClient
