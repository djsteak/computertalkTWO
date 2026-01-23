#include "Game.h"

#include "Network.h"

// self-explanatory
void Game::destroyObject(ObjectID id) {
    objects.erase(id);
}

// self-explanatory
Object* Game::getObject(ObjectID id) {
    auto it = objects.find(id);
    if (it == objects.end())
        return nullptr;
    return &it->second;
}

// gets an unordered map of the objects
const std::unordered_map<ObjectID, Object>& Game::getObjects() const {
    return objects;
}

// V updates all of the objects positions and whatnot
void Game::update(float dt) {
    for (auto& [id, obj] : objects) {
        if (obj.authority == Authority::Server) {
            obj.update(dt);
        }
    }
}

// V renders everything
void Game::render(sf::RenderWindow& window) {
    for (auto& [id, obj] : objects) {
        if (!obj.renderer)
            continue;

        sf::RenderStates states;

        // Transformable shapes (Circle / Rect)
        if (auto* t = dynamic_cast<sf::Transformable*>(obj.renderer.get())) {
            t->setPosition(obj.position - cameraPos);
            t->setRotation(sf::radians(obj.rotation));
            window.draw(*obj.renderer);
        }
        // VertexArray path
        else {
            states.transform.translate(obj.position - cameraPos);
            states.transform.rotate(sf::radians(obj.rotation));
            window.draw(*obj.renderer, states);
        }
    }
}

// V basically just takes a packet from the server of the entire game in its current state and sets the local game to that
void Game::applyFullState(sf::Packet& packet) {
    objects.clear();

    std::uint32_t count;
    packet >> count;

    for (std::uint32_t i = 0; i < count; ++i) {
        Object obj;
        packet >> obj;
        objects.emplace(obj.getID(), Object(obj.getID()));
    }
}
