#include "Server.h"
#include "Game.h"
#include "Object.h"
#include <SFML/Network.hpp>
#include <iostream>

Game Server::game;

Server::Server() = default;

Server::~Server() = default;

void Server::run() {
    constexpr unsigned short PORT = 34197;

    if (!network.listen(PORT)) {
        return;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    sf::Clock clock;
    auto testCircleCircle = std::make_unique<sf::CircleShape>(20.f);
    testCircleCircle->setFillColor(sf::Color::White);
    Object testCircle = Object(1, std::move(testCircleCircle));
    game.objects[testCircle.getID()] = std::move(testCircle);
    for (const auto& [id, _] : game.objects) {
        std::cout << " - " << id << '\n';
    }
    while (running) {
        float dt = clock.restart().asSeconds();

        // V V V accepting clients V V V
        if (auto maybeClient = network.acceptClient()) {
            sf::TcpSocket& client = *maybeClient;

            clients.push_back(std::move(client));
            std::cout << "client connected\n";
        }

        // V V V receive packets V V V
        for (auto it = clients.begin(); it != clients.end();) {
            MessageType type;
            sf::Packet packet;

            if (!Network::receive(*it,type, packet)) {
                // disconnected
                std::cout << "dlient disconnected\n";
                it = clients.erase(it);
                continue;
            }

            handlePacket(type, packet, *it);
            ++it;
        }

        // V V V update game state V V V
        game.update(dt);

        // V V V broadcast state updates V V V
        broadcastGameState();

        sf::sleep(sf::milliseconds(1)); // do not spamming
    }
}




void Server::handlePacket(MessageType type, sf::Packet& packet, sf::TcpSocket& client) {
    std::cout << MessageTypetoString(type) << "\n";
    switch (type) {
        case MessageType::Join: {
            sf::Packet packet = game.serializeFullState();
            Network::send(client, MessageType::Join, packet);
            break;
        }

        case MessageType::GameState: {
            // TODO: handle game state update
            break;
        }

        case MessageType::SpawnObject: {
            // TODO: handle object spawn
            break;
        }

        case MessageType::DestroyObject: {
            // TODO: handle object removal
            break;
        }

        case MessageType::UpdateObject: {
            ObjectID id;
            sf::Vector2f velocity;
            sf::Vector2f position;
            packet >> id >> position.x >> position.y >> velocity.x >> velocity.y;

            auto& obj = game.objects[id];

            // SERVER VALIDATION
            if (obj.authority == Authority::Client) {
                obj.velocity = velocity;
            }

            break;
        }
    }
}

void Server::broadcastGameState() {
    for (auto& [id, obj] : game.objects) {

        sf::Packet update;
        Object::serializeField(update, obj, ::ObjectField::Position);

        for (auto& client : clients) {
            sf::Socket::Status success = client.send(update);
        }
    }
}
