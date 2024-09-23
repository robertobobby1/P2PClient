
#pragma once

#include <thread>

#include "R.h"
#include "PeerServer.h"
#include "PeerClient.h"
#include "Shared.h"

namespace P2PClient {
    inline const char* EXTERNAL_SERVER_IP = "localhost";
    inline int EXTERNAL_SERVER_PORT = 3000;

    inline int BACKLOG = 10;
    inline const int MAX_SERVER_STARTUP_RETRIES = 5;

    inline std::thread messageReaderThread;
    inline std::shared_ptr<R::Net::Client> p2pClient;

    inline std::function<void(R::Buffer&)> onNewMessageCallback = nullptr;
    inline std::mutex sendMessageMutex;
    inline std::condition_variable sendMessageCondition;

    inline void startMessageReaderThread() {
        // reading message
        messageReaderThread = std::thread([]() {
            R::Buffer buffer(255);
            while (p2pClient->isRunning) {
                buffer = p2pClient->readMessage();
                if (buffer.size <= 0) {
                    // other peer closed connection
                    continue;
                }

                if (onNewMessageCallback != nullptr) {
                    onNewMessageCallback(buffer);
                }
            }
        });
    }

    inline void init() {
        auto serverThread = std::thread(PeerServer::run, BACKLOG, MAX_SERVER_STARTUP_RETRIES);
        auto clientThread = std::thread(PeerClient::run, EXTERNAL_SERVER_IP, EXTERNAL_SERVER_PORT);

        Shared::isPeerServerSocketConnected.wait(false);
        Shared::isPeerClientSocketConnected.wait(false);

        // let the server know that it has been a succesfull connection
        PeerClient::sendPeersConnectSuccess();
        PeerClient::keepLooping = false;

        // choose which server to keep by port, the bigger one will be the client we will keep,
        // the other peer has the same information, the only problem would be if they are the same port, very strange

        if (PeerClient::peerInformation.port > PeerServer::runningPort) {
            p2pClient = R::Net::Client::makeAndSet(Shared::peerClientSocket);
            PeerServer::terminate();
        } else {
            p2pClient = R::Net::Client::makeAndSet(Shared::peerServerSocket);
            PeerClient::terminate();
        }

        // make sure threads stopped working, now handle from here
        serverThread.join();
        clientThread.join();

        startMessageReaderThread();
    }

    inline int sendMessage(R::Buffer& buffer) {
        return p2pClient->sendMessage(buffer);
    }

    inline void setOnNewMessageCallback(std::function<void(R::Buffer&)> callback) {
        onNewMessageCallback = callback;
    }
}  // namespace P2PClient
