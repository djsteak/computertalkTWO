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
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Client");
    auto ip = sf::IpAddress::resolve("127.0.0.1");
    sf::IpAddress serverIp = *ip;
    unsigned short port = 34197;
    bool connected = network.connect(serverIp, port);

    for (int i = 0; i < 5; ++i) { // try 5 times
        if (connected) {
            std::cout << "IT DID THE CONNECTION THINGY WOOOOOOOOOOO\n";
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

    sf::Packet handshakeAck;

    handshakeAck << username;

    if (!network.send(network.getSocket(), MessageType::Join, handshakeAck)) {
        std::cerr << "Failed to send handshake acknowledgment\n";
        return;
    }

    window.setFramerateLimit(60);
    Game game;
    sf::Clock clock;
    // IMPORTANT GAME LOOP
    while (window.isOpen()) {

        // V V V handle window events V V V
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // V V V INPUT V V V




        // V V V PACKETS! V V V
        auto packets = network.poll();
        while(!packets.empty()) {
            auto [type, packet] = packets.front();
            packets.pop();
            handlePacket(type, packet, game);
        }


        // V V V game update V V V
        float dt = clock.restart().asSeconds();
        game.update(dt);


        // V V V RENDER! V V V

        window.clear();
        game.render(window);
        window.display();

        sf::sleep(sf::milliseconds(15));
    }
}



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
            std::cout << "COMMENCING JOINING PROCEDURE!\n";
            while (!packet.endOfPacket()) {
                std::cout << packet.getData() << '\n';
                Object obj;
                packet >> obj;
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

        case MessageType::UpdateObject: { // IMPORTANT

        }
        default: ;
    }
}