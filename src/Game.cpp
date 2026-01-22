#include "Game.h"

#include "Network.h"


void Game::destroyObject(ObjectID id) {
    objects.erase(id);
}

Object* Game::getObject(ObjectID id) {
    auto it = objects.find(id);
    if (it == objects.end())
        return nullptr;
    return &it->second;
}

const std::unordered_map<ObjectID, Object>& Game::getObjects() const {
    return objects;
}

void Game::update(float dt) {
    for (auto& [id, obj] : objects) {
        if (obj.authority == Authority::Server) {
            obj.update(dt);
        }
    }
}

void Game::render(sf::RenderWindow& window) {
    for (auto& [id, obj] : objects) {
        if (!obj.renderer)
            continue;

        auto* shape = dynamic_cast<sf::Transformable*>(obj.renderer.get());
        if (shape)
            shape->setPosition(obj.position);

        window.draw(*obj.renderer);
    }
}

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
