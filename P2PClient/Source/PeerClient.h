#pragma once

#include "R.h"
#include "NetworkStructs.h"

namespace PeerClient {
    inline const int SECURITY_HEADER_LENGTH = 23;
    inline const char* SECURITY_HEADER = "0sdFGeVi3ItN1qwsHp3mcDF";

    inline std::shared_ptr<R::Net::Client> client;

    inline uint8_t createProtocolHeader(LobbyPrivacyType lobbyType, ActionType actionType) {
        uint8_t headerFlags = 0;
        if (lobbyType == LobbyPrivacyType::Public) {
            R::Utils::setFlag(headerFlags, ClientServerHeaderFlags_Public);
        }
        switch (actionType) {
            case ActionType::Connect:  // 11
                R::Utils::setFlag(headerFlags, ClientServerHeaderFlags_Bit1);
                R::Utils::setFlag(headerFlags, ClientServerHeaderFlags_Bit2);
                break;
            case ActionType::Create:  // 10
                R::Utils::setFlag(headerFlags, ClientServerHeaderFlags_Bit1);
                break;
            case ActionType::Disconnect:  // 01
                R::Utils::setFlag(headerFlags, ClientServerHeaderFlags_Bit2);
                break;
            case ActionType::PeerConnectSuccess:  // 00
                break;
        }

        return headerFlags;
    }

    inline void run(const char* hostname, int port) {
        client = R::Net::Client::makeAndRun(hostname, port);
        if (!client->isRunning) {
            printf("[Peer Client] Couldn't access the remote server");
            return;
        }

        auto buffer = R::Buffer(SECURITY_HEADER_LENGTH + 1);
        auto headerFlags = createProtocolHeader(LobbyPrivacyType::Public, ActionType::Create);

        buffer.write(SECURITY_HEADER, SECURITY_HEADER_LENGTH);
        buffer.write(headerFlags);
        client->sendMessage(buffer);

        auto readBuffer = client->readMessage();
        R::Utils::hexDump(readBuffer);
    };

    inline void terminate() {
        client->terminate();
    };
}  // namespace PeerClient
