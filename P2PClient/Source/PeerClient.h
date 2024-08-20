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
