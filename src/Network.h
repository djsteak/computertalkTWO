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
};

inline const char* MessageTypetoString(MessageType type) {
    switch (type) {
        case MessageType::Join:          return "Join";
        case MessageType::GameState:     return "GameState";
        case MessageType::SpawnObject:   return "SpawnObject";
        case MessageType::DestroyObject: return "DestroyObject";
        case MessageType::UpdateObject:  return "UpdateObject";
        case MessageType::RequestNetworkOwnership: return "RequestNetworkOwnership";
        case MessageType::RemoveNetworkOwnership: return "RemoveNetworkOwnership";

        default:                         return "Unknown";
    }
}

class Network {
public:
    Network() = default;
    ~Network();

    // client-side
    bool connect(const sf::IpAddress& address, unsigned short port);

    // Server-side
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
