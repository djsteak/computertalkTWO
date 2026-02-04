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
    std::vector<Object*> drawList;

    sf::Vector2f cameraPos = sf::Vector2f(0, 0);
    // Object management
    Object& createObject(sf::Drawable& drawable, Authority authority = Authority::Server);
    void destroyObject(ObjectID id);
    void createDrawList();
    Object* getObject(ObjectID id);
    [[nodiscard]] const std::unordered_map<ObjectID, Object>& getObjects() const;
    ObjectID findFreeID(ObjectID start) const {
        ObjectID id = start;

        while (objects.find(id) != objects.end()) {
            ++id;
        }

        return id;
    }
    // Simulation
    bool server = false;
    void update(float dt);
    void render(sf::RenderWindow& window);
    int TICK = 0;
    // Networking
    [[nodiscard]] sf::Packet serializeFullState() const {
        sf::Packet packet;
        packet << static_cast<std::uint32_t>(objects.size());

        for (const auto& [id, obj] : objects) {
            packet << obj;
        }
        return packet;
    }
    void applyFullState(sf::Packet& packet);

private:
    ObjectID nextID = 1;

};

#endif
