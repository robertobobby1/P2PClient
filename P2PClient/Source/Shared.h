#pragma once

#include "R.h"

namespace Rp2p = R::Net::P2P;

namespace Shared {
    inline std::atomic<bool> isPeerClientSocketConnected{false};
    inline std::atomic<bool> isPeerServerSocketConnected{false};

    inline R::Net::Socket peerClientSocket;
    inline R::Net::Socket peerServerSocket;
}  // namespace Shared