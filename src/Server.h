#ifndef CMAKESFMLPROJECT_SERVER_H
#define CMAKESFMLPROJECT_SERVER_H

#include "Network.h"
#include <SFML/Network.hpp>

#include "Game.h"
#include <list>

class Server {
public:
    Server();
    ~Server();
    static Game game;

    void run();

private:
    Network network;
    std::list<sf::TcpSocket> clients;
    bool running = true;
    void broadcastGameState();
    void handlePacket(MessageType type, sf::Packet& packet, sf::TcpSocket& client);
};

#endif
