#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <optional>
#include <string>
#include "Client.h"
#include "Object.h"
#include "Game.h"

#include <iostream>

Client::Client() = default;


void Client::run() {
    std::string username = _getUsername();
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned>(screensize.x * 2), static_cast<unsigned>(screensize.y * 2)}), "Client");
    window.setFramerateLimit(60);
    Game game;
    sf::Clock clock;

    auto ip = sf::IpAddress::resolve("127.0.0.1");
    sf::IpAddress serverIp = *ip;
    unsigned short port = 34197;
    bool connected = network.connect(serverIp, port);

    for (int i = 0; i < 5; ++i) { // try 5 times
        if (connected) {
            std::cout << "connected to server\n";
            break;
        } else {
            std::cout << "waiting for connection...\n";
            sf::sleep(sf::milliseconds(1000));
        }
    }
    if (!connected) {
        std::cerr << "failed to connect to server\n";
        return;
    }
    std::cout << "joining the game step 1\n";
    sf::Packet handshakeAck;
    std::cout << "joining the game step 1.01\n";

    handshakeAck << username;
    std::cout << "joining the game step 1.1\n";

    if (!network.send(network.getSocket(), MessageType::Join, handshakeAck)) {
        std::cerr << "Failed to send handshake acknowledgment\n";
        return;
    }
    std::cout << "joining the game step 1 complete\n";


    // IMPORTANT GAME LOOP
    std::cout << "joining the game step 1.5 (starting the game loop)\n";
    while (window.isOpen()) {

        // V V V handle window events V V V
        std::cout << "window stuff\n";

        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // V V V INPUT V V V (to be implemented in the future)
        std::cout << "getting input\n";




        // V V V PACKETS! V V V
        std::cout << "getting packets\n";
        auto packets = network.poll();
        while(!packets.empty()) {
            auto [type, packet] = packets.front();
            packets.pop();
            handlePacket(type, packet, game);
        }


        // V V V game update V V V
        std::cout << "updating game objects\n";
        float dt = clock.restart().asSeconds();
        game.update(dt);
        std::cout << "setting camera pos\n";
        if (auto* player = game.getObject(1)) {
            game.cameraPos = player->position - screensize;
        }

        // V V V RENDER! V V V
        std::cout << "rendering\n";
        window.clear();
        //std::cout << game.getObject(1)->toString() << "\n";
        window.clear();
        game.render(window);
        window.display();

        sf::sleep(sf::milliseconds(15));
    }
}


// V V V V V its in the name V V V V V
std::string Client::_getUsername() {
    sf::RenderWindow window(sf::VideoMode({300, 80}), "enter username");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        return "";
    }

    sf::Text prompt(font);
    prompt.setString("Username:");
    prompt.setCharacterSize(20);
    prompt.setFillColor(sf::Color::White);
    prompt.setPosition({10.f, 10.f});

    sf::Text inputText(font);
    inputText.setCharacterSize(20);
    inputText.setFillColor(sf::Color::Cyan);
    inputText.setPosition({10.f, 40.f});

    std::string username;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {

            // Close window
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return "";
            }

            // Text input
            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                char32_t unicode = textEvent->unicode;

                // Enter = finish
                if (unicode == '\r' || unicode == '\n') {
                    window.close();
                    return username;
                }

                // Backspace
                if (unicode == 8) {
                    if (!username.empty())
                        username.pop_back();
                }
                // Printable ASCII (you can widen this later)
                else if (unicode >= 32 && unicode < 127) {
                    username += static_cast<char>(unicode);
                }

                inputText.setString(username);
            }
        }

        window.clear(sf::Color::Black);
        window.draw(prompt);
        window.draw(inputText);
        window.display();
    }

    return username;
}


// V V V yeah this is kinda important V V V
void Client::handlePacket(MessageType type, sf::Packet& packet, Game& game) {
    std::cout << MessageTypetoString(type) << '\n';

    switch (type) {
        case MessageType::Join: {
            std::uint32_t count;
            std::cout << "joining the game step 2\n";
            packet >> count;
            std::cout << "joined the game step 2.1\n";

            for (std::uint32_t i = 0; i < count; ++i) {
                Object obj;
                packet >> obj;
                std::cout << obj.toString() << '\n';
                game.objects[obj.getID()] = std::move(obj);

            }
            break;

        }

        case MessageType::GameState:
            // TODO: handle game state update
            break;

        case MessageType::SpawnObject:
            // TODO: handle object spawn
            break;

        case MessageType::DestroyObject:
            // TODO: handle object removal
            break;

        case MessageType::UpdateObject: { // IMPORTANT!

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

        default: ;
    }
}