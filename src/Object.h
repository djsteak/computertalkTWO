#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <map>

using ObjectID = std::uint32_t;

enum class Authority : std::uint8_t { Server, Client };
enum class Type : std::uint8_t { Player, Enemy, Bullet, Generic };

// Each property that can be synced over network
enum class ObjectField : uint8_t {
    Position,
    Velocity,
    Owner,
    Authority,
    Type,
    Rotation,
    ShapeCircle, // radius
    ShapeRect,   // width & height
};

class Object {
public:
    Object(ObjectID id, std::unique_ptr<sf::Drawable> renderer);
    Object() = default;
    ObjectID getID() const { return id; }

    // Simulation state
    sf::Vector2f position{0.f, 0.f};
    sf::Vector2f velocity{0.f, 0.f};
    bool owner = false;
    Authority authority = Authority::Server;
    float rotation = 0.f;

    // Shape
    std::unique_ptr<sf::Drawable> renderer;
    float radius = 0.f; // for CircleShape
    float width = 0.f, height = 0.f; // for RectangleShape

    Type t = Type::Generic;

    template <typename T>
    void setRenderer(T shape) {
        static_assert(std::is_base_of<sf::Drawable, T>::value,
                      "Renderer must inherit from sf::Drawable");
        renderer = std::make_unique<T>(shape);
    }

    void update(float dt) { position += velocity * dt; }

    // Networking helpers
    friend sf::Packet& operator<<(sf::Packet& packet, const Object& obj);
    friend sf::Packet& operator>>(sf::Packet& packet, Object& obj);

    static sf::Packet& serializeField(sf::Packet& packet, const Object& obj, ObjectField field);
    static void deserializeField(sf::Packet& packet, Object& obj, ObjectField field);

private:
    ObjectID id = 0;
};
