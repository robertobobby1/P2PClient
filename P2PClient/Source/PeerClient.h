#pragma once

#include "R.h"
#include "PeerServer.h"
#include "Shared.h"

namespace PeerClient {
    inline const int SELECT_TIMEOUT_SECONDS = 2;

    inline Rp2p::ServerConnectPayload peerInformation;
    inline std::shared_ptr<R::Net::Client> client;

    inline bool keepLooping = true;
    inline std::string _uuid = "";

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

        // TODO wait for delay time and then connect
        char ipBuffer[INET_ADDRSTRLEN]{0};
        auto client = R::Net::Client::makeAndRun(
            inet_ntop(AF_INET, &peerInformation.ipAddress, ipBuffer, INET_ADDRSTRLEN), peerInformation.port
        );
        auto peerMessageBuffer = Rp2p::createClientPeerMessageBuffer();
        client->sendMessage(peerMessageBuffer);

        Shared::peerClientSocket = client->_socket;
        Shared::isPeerClientSocketConnected = true;
        Shared::isPeerClientSocketConnected.notify_all();
    }

    inline void tryConnect(const char* hostname, int port) {
        timeval selectTimeout{SELECT_TIMEOUT_SECONDS, 0};
        fd_set socketFdSet;

        client = R::Net::Client::makeAndRun(hostname, port);
        if (!client->isRunning) {
            RLog("[Peer Client] Couldn't access the remote server\n");
            return;
        }

        FD_ZERO(&socketFdSet);
        FD_SET(client->_socket, &socketFdSet);

        auto sendBuffer = Rp2p::createClientPublicConnectBuffer(PeerServer::runningPort);
        client->sendMessage(sendBuffer);

        // no keep alive message in 10 seconds means the server disconnected
        Rt::Timer keepAliveTimer(Rt::Seconds(10));

        while (client->isRunning && keepLooping) {
            auto tempFdSet = socketFdSet;

            auto selectResponse = select(FD_SETSIZE, &tempFdSet, NULL, NULL, &selectTimeout);
            if (selectResponse < 0) {
                R::Net::onError(client->_socket, true, "[Logic Server] Error during select");
                client->isRunning = false;
                continue;
            }

            // timeout
            if (selectResponse == 0) {
                if (keepAliveTimer.isTimerFinished()) {
                    client->terminate();
                }

                continue;
            }

            auto buffer = client->readMessage();
            if (buffer.size == 0) {
                RLog("[Peer Client] End of file, server closed socket!\n");
                client->terminate();
                continue;
            }

            if (!Rp2p::isValidAuthedRequest(buffer)) {
                RLog("[Peer Client] Bad protocol!\n");
                client->terminate();
                continue;
            }

            if (Rp2p::KeepAliveManager::isKeepAlivePackage(buffer)) {
                keepAliveTimer.resetTimer();
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
    }

    inline void run(const char* hostname, int port) {
        PeerServer::isPortAvailable.wait(false);
        // retry until we connect with another peer
        while (keepLooping) {
            tryConnect(hostname, port);
            RLog("[Peer Client] The server probably disconnected, retrying!\n");
            std::this_thread::sleep_for(Rt::Seconds(R::Utils::randomNumber(1, 7)));
        }
    }

    inline void sendPeersConnectSuccess() {
        auto buffer = Rp2p::createClientPeersConnectSuccessBuffer();

        client->sendMessage(buffer);
    }

}  // namespace PeerClient
