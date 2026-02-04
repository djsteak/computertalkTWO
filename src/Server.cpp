#include "Server.h"
#include "Game.h"
#include "Object.h"
#include <SFML/Network.hpp>
//#include <iostream>

Game Server::game;

Server::Server() = default;

Server::~Server() {
    for (auto& client : clients) {
        client.disconnect();
    }
    clients.clear();
    network.closeListener();
};

void Server::run() {
    game.server = true;
    constexpr unsigned short PORT = 34197;

    if (!network.listen(PORT)) {
        return;
    }

    //std::cout << "Server listening on port " << PORT << "...\n";
    // creating the variables needed for the game
    int enemy1s_max = 1;
    int enemy1sCount = 0;
    ObjectID enemy1s[enemy1s_max];
    std::fill_n(enemy1s, enemy1s_max, ObjectID(0));

    sf::Clock clock;
    // creating the mothership (to be defended)
    Object playerMothership = Object(0);
    playerMothership.position = sf::Vector2f(100.f, 100.f);
    playerMothership.t = Type::PlayerVehicle;
    playerMothership.color = sf::Color::Red;
    playerMothership.owner = true;
    playerMothership.drawOrder = 10;
    game.objects[playerMothership.getID()] = std::move(playerMothership);

    // create the circle of testing lol
    Object circle = Object(60000);
    circle.position = sf::Vector2f(0.f, 0.f);
    circle.t = Type::GenericCircle;
    circle.radius = 20.f;
    circle.color = sf::Color::Red;
    circle.drawOrder = 0;
    game.objects[circle.getID()] = std::move(circle);

    // create turret1
    Object turret1 = Object(1);
    turret1.position = sf::Vector2f(100.f, 100.f);
    turret1.t = Type::PlayerTurret1;
    turret1.color = sf::Color::Red;
    turret1.owner = true;
    turret1.drawOrder = 100;
    game.objects[turret1.getID()] = std::move(turret1);


    for (const auto& [id, _] : game.objects) {
        ////std::cout << " - " << id << '\n';
    }
    while (running) {
        float dt = clock.restart().asSeconds();


        // V V V accepting clients V V V
        if (auto maybeClient = network.acceptClientNonBlocking()) {
            sf::TcpSocket& client = **maybeClient;
            client.setBlocking(false); // each client socket should be non-blocking too
            clients.push_back(std::move(client));
            //std::cout << "client connected\n";
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
                ++it;              // no data yet so skip
                continue;
            }

            if (status != sf::Socket::Status::Done) {
                //std::cout << "client disconnected\n";
                it = clients.erase(it);
                continue;
            }

            handlePacket(type, packet, *it);
            ++it;
        }

        // V V V update game state V V V
        while (enemy1sCount < enemy1s_max) { // spawning enemy1s
            enemy1sCount += 1;

            Object enemy1 = Object(game.findFreeID(1000));
            enemy1.position = sf::Vector2f(100.f, 0.f);
            enemy1.t = Type::Enemy1;
            enemy1.color = sf::Color::Red;
            enemy1.owner = true;
            enemy1.drawOrder = 15;
            sf::Packet enemyspawn;
            enemyspawn << enemy1;
            game.objects[enemy1.getID()] = std::move(enemy1);

            for (auto& client : clients) {
                Network::send(client, MessageType::SpawnObject, enemyspawn);
            }
        }


        game.objects[0].velocity.y += 0.05;
        game.update(dt);

        // V V V broadcast state updates V V V
        broadcastGameState();

        sf::sleep(sf::milliseconds(15)); // do not spamming
    }
}




void Server::handlePacket(MessageType type, sf::Packet& packet, sf::TcpSocket& client) {
    ////std::cout << MessageTypetoString(type) << "\n";
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

            Object obj;
            packet >> obj;
            ////std::cout << obj.toString() << '\n';
            if (game.objects.find(obj.getID()) != game.objects.end()) {
                game.objects[obj.getID()] = std::move(obj);
            }
            sf::Packet out;
            out << static_cast<uint8_t>(MessageType::SpawnObject) << obj;

            for (auto& client : clients) {
                client.send(out);
            }
            break;
        }

        case MessageType::DestroyObject: {
            // TODO: handle object removal
            ObjectID id;
            packet >> id;
            game.objects.erase(id);
            sf::Packet out;
            out << id;
            for (auto& c : clients) {
                Network::send(c, MessageType::DestroyObject, packet);
            }
            break;
        }

        case MessageType::UpdateObject: {
            for (auto& client : clients) {
                sf::Socket::Status success = client.send(packet);
            }
            ObjectID id;
            packet >> id;
            if (auto obj = game.getObject(id)) {

                uint8_t fieldCount;
                packet >> fieldCount;

                for (uint8_t i = 0; i < fieldCount; ++i) {
                    uint8_t raw;
                    packet >> raw;

                    ObjectField field = static_cast<ObjectField>(raw);
                    Object::deserializeField(packet, *obj, field);
                }
            }

            break;
        }
        case MessageType::RequestControl: {
            uint8_t a;
            packet >> a;
            ControlType b = static_cast<ControlType>(a);
            ////std::cout << ControlTypetoString(b) << '\n';

            switch (b) {
                case ControlType::Turret1: {
                    if (game.objects[1].owner) {
                        game.objects[1].owner = false;
                        sf::Packet newpacket;

                        newpacket << static_cast<uint8_t>(ControlType::Turret1);
                        Network::send(client, MessageType::RequestControl, newpacket);
                    }
                    break;
                }

                default: ;
            }
        }
        case MessageType::RemoveControl: {
            uint8_t a;
            packet >> a;
            switch (static_cast<ControlType>(a)) {
                case ControlType::Turret1: {
                    if (!game.objects[1].owner) {
                        game.objects[1].owner = true;
                        sf::Packet newpacket;

                        newpacket << static_cast<uint8_t>(ControlType::Turret1);
                        Network::send(client, MessageType::RemoveControl, newpacket);
                    }
                    break;
                }
                default: ;
            }
        }
    }
}

void Server::broadcastGameState() {
    for (auto& [id, obj] : game.objects) {
        if (!obj.sendUpdates or !obj.owner) {
            continue;
        }
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
