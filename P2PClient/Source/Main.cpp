#include "P2PClient.h"

inline const int PEER_NUMBER = 10;
inline const int SLEEP_BETWEEN_PEERS = 5;

void onNewMessage(R::Buffer &buffer) {
    buffer.print();
}

int runOne() {
    P2PClient::setOnNewMessageCallback(onNewMessage);
    // blocking until succesful connection
    P2PClient::init();

    RLog("Succesful connection\n");

    while (true) {
        auto message = R::Utils::generateUUID(15);

        R::Buffer buffer(20);
        buffer.write(message.c_str(), message.length());

        P2PClient::sendMessage(buffer);
        R::Utils::sleepThread(R::Time::Seconds(50));
    }
}

int main() {
    // makes 5 children
    // R::Utils::makeXChildren(5);
    // 5 children plus the father
    runOne();
}
