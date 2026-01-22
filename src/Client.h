#ifndef CMAKESFMLPROJECT_CLIENT_H
#define CMAKESFMLPROJECT_CLIENT_H
#include <SFML/Network.hpp>
#include "Network.h"
#include <string>

#include "Game.h"

class Client {
public:
    Client();
    void run();

private:
    static std::string _getUsername();
    static void handlePacket(MessageType type, sf::Packet& packet, Game& game);
    Network network;
};

#endif
