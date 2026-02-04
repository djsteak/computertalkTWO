#ifndef CMAKESFMLPROJECT_CLIENT_H
#define CMAKESFMLPROJECT_CLIENT_H
#include <SFML/Network.hpp>
#include "Network.h"
#include <string>

#include "Game.h"

class Client {
public:
    sf::Vector2f screensize = sf::Vector2f(500, 400);
    Client();
    void run();
    ControlType control = ControlType::Pilot;
private:
    static std::string _getUsername();
    void handlePacket(MessageType type, sf::Packet& packet, Game& game);
    Network network;
};

#endif
