#ifndef CMAKESFMLPROJECT_GAME_H
#define CMAKESFMLPROJECT_GAME_H

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "Object.h"

class Game {
public:
    Game() = default;
    ~Game() = default;
    std::unordered_map<ObjectID, Object> objects;

    sf::Vector2f cameraPos = sf::Vector2f(0, 0);
    // Object management
    Object& createObject(sf::Drawable& drawable, Authority authority = Authority::Server);
    void destroyObject(ObjectID id);

    Object* getObject(ObjectID id);
    [[nodiscard]] const std::unordered_map<ObjectID, Object>& getObjects() const;

    // Simulation
    void update(float dt);
    void render(sf::RenderWindow& window);

    // Networking
    sf::Packet serializeFullState() const {
        sf::Packet packet;
        for (const auto& [id, obj] : objects) {
            packet << obj;
        }
        return packet;
    }    void applyFullState(sf::Packet& packet);

private:
    ObjectID nextID = 1;

};

#endif
