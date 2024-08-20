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

    inline const int MAX_SERVER_STARTUP_RETRIES = 5;
    inline const int DEFAULT_PORT = 22000;
    inline const int BACKLOG = 10;

    void run() {
        server = R::Net::Server::makeAndRun(DEFAULT_PORT, BACKLOG);
        for (int i = 1; i < MAX_SERVER_STARTUP_RETRIES; i++) {
            if (!server->isRunning) {
                printf("[Peer Server] Retrying to start the server... ");

                auto temp = R::Net::Server::makeAndRun(rand(), BACKLOG);
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
                    printf("[Peer Server] size: %i, message: %s \n", buffer.size, buffer.ini.get());
                }
            }
        }
    };

    inline void terminate() {
        server->terminate();
    };

}  // namespace PeerServer