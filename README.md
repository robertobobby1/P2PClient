# PeerClient

## Description

This is a Peer client, used with the Peer server repository to connect peers in a P2P network system. This uses Tcp hole punching to connect peers without need of any server to mantain the connection. It uses an intermediate server just to make the start connection.
This will allow the connection to be faster given that there is no need for a server for basic communication reducing the amount of indirection in every request.
The use cases for a network model like this could be a real time gameplay where the server only receives the necessary data from the Peers and any of the network specifics can be sent just from peer to peer.

## Usage

This is a header only library, the way of using this is just to import the file P2PClient.h under the Include/ folder. This will also contain the R.h utils functions since it is used by the P2PClient. You can access all this directly from there and there is no need to import this twice.

## Protocol - Server-Client

##### All request contain

- 23 bytes of security
- 1 byte for protocol header:
  - B1 | B2 | B3 | B4 | B5 | B6 | B7 | B8
  - B1 & B2 => Action, possible values are:
    - 00 = peersConnectSuccess
    - 01 = disconnect
    - 10 = createLobby
    - 11 = connect
  - B3 => LobbyPrivacyType, possible values are:
    - 0 = Private
    - 1 = Public

#### Create request includes nothing else

#### Disconnect request includes nothing else

#### Peers connect success request includes nothing else

#### Connect request also includes:

- 5 bytes game hash

## Protocol - Client-Server

#### All responses contain

- 23 bytes of security
- 1 byte for protocol header:
  - B1 | B2 | B3 | B4 | B5 | B6 | B7 | B8
  - B1 => Action, posible values are:
    - 0 => send uuid
    - 1 => connect

#### Connect request also includes:

- 4 bytes for the ipAddress of the other peer
- 2 bytes for the port of the other peer
- 4 bytes for the delay of the other peer

#### Send uuid request also includes:

- 5 bytes for game hash

#### Disclaimer:

- if B1 | B2 | B3 | B4 | B5 | B6 | B7 | B8 are all set to 0 then it is a Keep Alive package
