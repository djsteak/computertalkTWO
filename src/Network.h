#ifndef CMAKESFMLPROJECT_NETWORK_H
#define CMAKESFMLPROJECT_NETWORK_H

#include <SFML/Network.hpp>
#include <cstdint>
#include <functional>
#include <unordered_map>

enum class MessageType : std::uint8_t {
    Join, // player joins the game
    SpawnObject, // client or server spawns an object for everyone
    DestroyObject, // client or server despawns an object for everyone
    GameState, // client asks server for the entire game
    UpdateObject, // client or server tells everyone to update a specific value for an object
    RequestNetworkOwnership, // client tells the server that they are taking control of an object
    RemoveNetworkOwnership, // client tells the server to take control over an object
    RequestControl, // this and the one below give or revoke control from a part of the craft to the player
    RemoveControl,
};

enum class ControlType : std::uint8_t {
    Pilot,
    Turret1,
    Turret2,
};

inline const char* MessageTypetoString(MessageType type) {
    switch (type) {
        case MessageType::Join:          return "Join\n";
        case MessageType::GameState:     return "GameState\n";
        case MessageType::SpawnObject:   return "SpawnObject\n";
        case MessageType::DestroyObject: return "DestroyObject\n";
        case MessageType::UpdateObject:  return "";
        case MessageType::RequestNetworkOwnership: return "RequestNetworkOwnersh\n";
        case MessageType::RemoveNetworkOwnership: return "RemoveNetworkOwnership\n";
        case MessageType::RequestControl:   return "RequestControl\n";
        case MessageType::RemoveControl:   return "RemoveControl\n";

        default:                         return "Unknown";
    }
}

inline const char* ControlTypetoString(ControlType type) {
    switch (type) {
        case ControlType::Pilot:        return "pilot\n";
        case ControlType::Turret1:        return "turret1\n";
        case ControlType::Turret2:        return "turret2\n";

    }
    return "control type not supported by this tostring";
}

class Network {
public:
    Network() = default;
    ~Network();

    // client-side
    bool connect(const sf::IpAddress& address, unsigned short port);

    // Server-side
    void closeListener() {
        listener.close();
    }
    bool listen(unsigned short port);
    sf::TcpSocket* acceptClient();
    std::optional<std::unique_ptr<sf::TcpSocket>> acceptClientNonBlocking();

    // Common
    static bool send(sf::TcpSocket& socket, MessageType type, sf::Packet& data);
    static bool receive(sf::TcpSocket& socket, MessageType& type, sf::Packet& data);
    static sf::Socket::Status receiveNonBlocking(sf::TcpSocket &socket, MessageType &type, sf::Packet &packet);

    static void debugpacket(sf::Packet& packet);

    sf::TcpSocket& getSocket();
    void disconnect();
    std::queue<std::pair<MessageType, sf::Packet>> poll();
private:
    sf::TcpListener listener;
    sf::TcpSocket socket; // used by client

};

#endif
