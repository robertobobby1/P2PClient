#pragma once

#include "R.h"

namespace PeerClient {
    inline const int SECURITY_HEADER_LENGTH = 23;
    inline const char* SECURITY_HEADER = "0sdFGeVi3ItN1qwsHp3mcDF";

    inline std::shared_ptr<R::Net::Client> client;

    inline void run(const char* hostname, int port) {
        client = R::Net::Client::makeAndRun(hostname, port);
        if (!client->isRunning) {
            printf("[Peer Client] Couldn't access the remote server");
            return;
        }

        auto buffer = R::Net::P2P::createClientPublicConnectBuffer();
        client->sendMessage(buffer);

        auto readBuffer = client->readMessage();
        R::Utils::hexDump(readBuffer);
    };

    inline void terminate() {
        client->terminate();
    };
}  // namespace PeerClient
