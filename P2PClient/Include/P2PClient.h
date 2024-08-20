#pragma once

#include "R.h"

namespace PeerClient {
    inline const char* EXTERNAL_SERVER_IP = "0.tcp.eu.ngrok.io";
    inline int EXTERNAL_SERVER_PORT = 12078;

    inline std::shared_ptr<R::Net::Client> client;

    void run() {
        client = R::Net::Client::makeAndRun(EXTERNAL_SERVER_IP, EXTERNAL_SERVER_PORT);
        if (!client->isRunning) {
            printf("[Peer Client] Client couldn't access the remote server");
        }
    };

    void terminate() {
        client->terminate();
    };

}  // namespace PeerClient

#include <thread>

#include <iostream>

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
