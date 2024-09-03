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

    inline void
    start(int port = 22000, int backlog = 10, int maxServerStartupRetries = 5) {
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

        bool openConexion = true;
        while (openConexion) {
            auto buffer = server->readMessage(AcceptSocket);
            // error, couldn't correctly read
            if (buffer.size > 0) {
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
                RLog("Notifying from peer server\n");
                Shared::isPeerServerSocketConnected.notify_all();
            } else {
                openConexion = false;
            }
        }
    }

    inline void run(int port = 22000, int backlog = 10, int maxServerStartupRetries = 5) {
        start(port, backlog, maxServerStartupRetries);

        waitForOtherPeer();
    };

    inline void terminate() {
        server->terminate();
    };

}  // namespace PeerServer