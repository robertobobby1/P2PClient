#pragma once

#include <iostream>
#include <atomic>

#include "R.h"
#include "Shared.h"

namespace PeerServer {

    enum ExternalServerState {
        NONE,
        ACCESIBLE,
        UNACCESIBLE
    };

    inline ExternalServerState externalServerState = NONE;
    inline std::shared_ptr<R::Net::Server> server;

    inline std::atomic<bool> isPortAvailable{false};
    inline uint16_t runningPort;

    inline void start(int backlog = 10, int maxServerStartupRetries = 5) {
        auto port = R::Utils::randomNumber(100, 65535);
        server = R::Net::Server::makeAndRun(port, backlog);

        for (int i = 1; i < maxServerStartupRetries; i++) {
            if (!server->isRunning) {
                printf("[Peer Server] Retrying to start the server...\n");

                port = R::Utils::randomNumber(100, 65535);
                auto temp = R::Net::Server::makeAndRun(port, backlog);
                server.swap(temp);
            } else {
                runningPort = port;
                isPortAvailable = true;
                isPortAvailable.notify_all();
                break;
            }
        }
    }

    inline void waitForOtherPeer() {
        auto AcceptSocket = server->acceptNewConnection().socket;
        if (AcceptSocket == SocketError) {
            return;
        }

        bool keepLooping = true;
        while (keepLooping) {
            auto buffer = server->readMessage(AcceptSocket);
            // error, couldn't correctly read
            if (buffer.size <= 0) {
                keepLooping = false;
                return;
            }

            if (!Rp2p::isValidAuthedRequest(buffer)) {
                RLog("[Peer Server] Bad Protocol!\n");
                return;
            }

            auto protocolHeader = Rp2p::getProtocolHeader(buffer);
            if (Rp2p::getClientClientProtocolHeader(protocolHeader) != Rp2p::ClientClientActionType::PeerMessage) {
                RLog("[Peer Server] Not a Peer message!\n");
                return;
            }

            Shared::peerServerSocket = AcceptSocket;
            Shared::isPeerServerSocketConnected = true;
            Shared::isPeerServerSocketConnected.notify_all();

            keepLooping = false;
        }
    };

    inline void run(int backlog = 10, int maxServerStartupRetries = 5) {
        srand((unsigned)time(NULL));
        start(backlog, maxServerStartupRetries);

        waitForOtherPeer();
    };

    inline void terminate() {
        server->terminate();
    };

}  // namespace PeerServer