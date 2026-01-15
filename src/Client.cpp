#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include "Client.h"
#include "Object.h"

#include <iostream>

Client::Client() {
    std::string username = _getUsername();
    std::cout << "Username: " << username << std::endl;
    sf::RenderWindow display(sf::VideoMode({1000, 800}), "SHITSHITSHITSHITSHIT");
    display.setFramerateLimit(60);
    std::list<Object> objects;
    Object playerCharacter;
    playerCharacter.setShape(std::make_unique<sf::CircleShape>(20.f));
    playerCharacter.getShape()->setFillColor(sf::Color::White);
    objects.push_back(std::move(playerCharacter));
    while (display.isOpen()) {
        while (const std::optional event = display.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                display.close();
        }
        display.clear();
        for (std::list<Object>::iterator object = objects.begin(); object != objects.end(); object++) {
            object->draw(display);
        }
        display.display();
    }
}

Client::~Client() = default;


std::string Client::_getUsername() {
    sf::RenderWindow window(sf::VideoMode({300, 80}), "Enter Username");
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
