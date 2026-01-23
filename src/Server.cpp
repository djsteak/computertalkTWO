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
    Object playerMothership = Object(1);
    playerMothership.position = sf::Vector2f(100.f, 100.f);
    playerMothership.t = Type::PlayerVehicle;
    playerMothership.color = sf::Color::Red;
    game.objects[playerMothership.getID()] = std::move(playerMothership);


    Object testCircle2 = Object(2);
    testCircle2.position = sf::Vector2f(50.f, 50.f);
    testCircle2.t = Type::GenericCircle;
    testCircle2.radius = 10;
    game.objects[testCircle2.getID()] = std::move(testCircle2);


    for (const auto& [id, _] : game.objects) {
        std::cout << " - " << id << '\n';
    }
    while (running) {
        float dt = clock.restart().asSeconds();


        // V V V accepting clients V V V
        if (auto maybeClient = network.acceptClientNonBlocking()) {
            sf::TcpSocket& client = **maybeClient;
            client.setBlocking(false); // each client socket should be non-blocking too
            clients.push_back(std::move(client));
            std::cout << "client connected\n";
        }

        if (clients.empty()) {
            sf::sleep(sf::milliseconds(1)); // do not spamming
            continue;
        }

        // V V V receive packets V V V
        for (auto it = clients.begin(); it != clients.end();) {
            MessageType type;
            sf::Packet packet;
            sf::Socket::Status status = Network::receiveNonBlocking(*it, type, packet);
            if (status == sf::Socket::Status::NotReady) {
                ++it;              // no data yet — DO NOT BLOCK
                continue;
            }

            if (status != sf::Socket::Status::Done) {
                std::cout << "client disconnected\n";
                it = clients.erase(it);
                continue;
            }

            handlePacket(type, packet, *it);
            ++it;
        }

        // V V V update game state V V V
        game.objects[1].velocity.y += 0.01;
        game.update(dt);

        // V V V broadcast state updates V V V
        broadcastGameState();

        sf::sleep(sf::milliseconds(15)); // do not spamming
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
            obj.velocity = velocity;
            obj.position = position;

            break;
        }
    }
}

void Server::broadcastGameState() {
    for (auto& [id, obj] : game.objects) {

        sf::Packet update;
        update << static_cast<uint8_t>(MessageType::UpdateObject);
        update << obj.getID();
        update << uint8_t(2); // number of fields

        Object::serializeField(update, obj, ObjectField::Position);
        Object::serializeField(update, obj, ObjectField::Velocity);

        for (auto& client : clients) {
            sf::Socket::Status success = client.send(update);
        }
    }
}
