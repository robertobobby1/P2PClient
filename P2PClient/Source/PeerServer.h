#pragma once

#include <iostream>

#include "R.h"

namespace PeerServer {

    enum ExternalServerState {
        NONE,
        ACCESIBLE,
        UNACCESIBLE
    };

    inline ExternalServerState externalServerState = NONE;
    inline std::shared_ptr<R::Net::Server> server;

    inline void run(int port = 22000, int backlog = 10, int maxServerStartupRetries = 5) {
        server = R::Net::Server::makeAndRun(port, backlog);
        for (int i = 1; i < maxServerStartupRetries; i++) {
            if (!server->isRunning) {
                printf("[Peer Server] Retrying to start the server...\n");

                auto temp = R::Net::Server::makeAndRun(rand(), backlog);
                server.swap(temp);
            } else {
                break;
            }
        }

        R::Net::Socket AcceptSocket = server->acceptNewConnection();
        if (AcceptSocket == -1) {
            return;
        }

        char recvbuf[512];
        bool openConexion = true;

        while (server->isRunning) {
            openConexion = true;
            while (openConexion) {
                auto buffer = server->readMessage(AcceptSocket);
                // error, couldn't correctly read
                if (buffer.size < 0) {
                    openConexion = false;
                } else {
                    printf("[Peer Server] size: %i, message: %s \n", (int)buffer.size, buffer.ini);
                }
            }
        }
    };

    inline void terminate() {
        server->terminate();
    };

}  // namespace PeerServer