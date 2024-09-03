#pragma once

#include "R.h"
#include "PeerServer.h"
#include "Shared.h"

namespace PeerClient {
    inline std::string _uuid = "";
    inline Rp2p::ServerConnectPayload peerInformation;
    inline std::shared_ptr<R::Net::Client> client;
    inline int counter = 0;

    inline void terminate() {
        client->terminate();
    }

    inline void handleSendUUIDRequest(R::Buffer& buffer) {
        auto uuid = Rp2p::getUUIDFromSendUUIDBuffer(buffer);
        if (uuid == "") {
            RLog("[Peer Client] Unexpected package, expecting a uuid with this response\n");
            return;
        }

        RLog("[Peer Client] Connected to lobby with uuid: %s\n", uuid.c_str());
        _uuid = uuid;
    }

    inline void handleConnectRequest(R::Buffer& buffer) {
        auto payload = Rp2p::getPayloadFromServerConnectBuffer(buffer);
        // error scenario
        if (payload.port == 0) {
            RLog("[Peer Client] Unexpected package, expecting another peer's information package\n");
            return;
        }

        peerInformation = payload;
        payload.Print();

        // wait for delay time and then connect
        auto client = R::Net::Client::makeAndRun(inet_ntoa(peerInformation.ipAddress), peerInformation.port);
        Shared::peerClientSocket = client->_socket;
        RLog("Notifying from peer client");
        Shared::isPeerClientSocketConnected.notify_all();
    }

    inline void run(const char* hostname, int port) {
        client = R::Net::Client::makeAndRun(hostname, port);
        if (!client->isRunning) {
            RLog("[Peer Client] Couldn't access the remote server\n");
            return;
        }

        PeerServer::isPortAvailable.wait(false);

        auto sendBuffer = Rp2p::createClientPublicConnectBuffer(PeerServer::runningPort);
        client->sendMessage(sendBuffer);

        while (client->isRunning) {
            auto buffer = client->readMessage();
            if (!Rp2p::isValidAuthedRequest(buffer)) {
                RLog("[Peer Client] Bad protocol!\n");
                if (++counter > 200) {
                    terminate();
                    return;
                }
                continue;
            }

            if (Rp2p::KeepAliveManager::isKeepAlivePackage(buffer)) {
                RLog("[Peer Client] Keep alive package\n");
                continue;
            }

            auto protocolHeader = Rp2p::getProtocolHeader(buffer);
            auto actionType = Rp2p::getServerActionTypeFromHeaderByte(protocolHeader);
            switch (actionType) {
                case Rp2p::ServerActionType::SendUUID:
                    handleSendUUIDRequest(buffer);
                    break;
                case Rp2p::ServerActionType::Connect:
                    handleConnectRequest(buffer);
                    break;
            }
        }
    };

}  // namespace PeerClient
