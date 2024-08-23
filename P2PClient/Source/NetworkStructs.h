#pragma once

// Client-Server data flags
enum ClientServerHeaderFlags {
    // Type of the lobby public/private
    ClientServerHeaderFlags_Public = 1 << 5,  // 00100000
    // Type of the action we are trying create/connect/disconnect/peersConnectSuccess
    ClientServerHeaderFlags_Bit1 = 1 << 7,  // 10000000
    ClientServerHeaderFlags_Bit2 = 1 << 6,  // 01000000
};

// Server-Client data flags
enum ServerClientHeaderFlags {
    // Action of the request
    ServerClientHeaderFlags_Action = 1 << 7,  // 10000000
};

enum LobbyPrivacyType {
    Private,
    Public
};

enum ActionType {
    Create,
    Connect,
    Disconnect,
    PeerConnectSuccess
};