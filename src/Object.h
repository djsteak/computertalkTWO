#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <map>

#include "SFML/Audio/Listener.hpp"

using ObjectID = std::uint32_t;

enum class Authority : std::uint8_t { Server, Client };

// IMPORTANT: WHEN ADDING NEW OBJECTS HERE YOU MUST IMPLEMENT IT INTO THE FOLLOWING:
// V | | | V      Object.h      ->   std::string TypeToString(Type t) // not needed unless debugging
// V | | | V      Object.cpp    ->   void Object::rebuildRenderer()
// V V V V V      Object.cpp    ->   void Object::applyColor()
enum class Type : std::uint8_t {
    GenericCircle,
    GenericRectangle,
    PlayerVehicle,
};

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
    Color,
    Radius,
    Size,
};


class Object {
public:

    static const char* AuthorityToString(Authority a) {
        switch (a) {
            case Authority::Server: return "Server";
            case Authority::Client: return "Client";
        }
        return "Unknown";
    }

    static const char* TypeToString(Type t) {
        switch (t) {
            case Type::GenericCircle:  return "GenericCircle";
            case Type::GenericRectangle:   return "GenericRectangle";
            case Type::PlayerVehicle:   return "PlayerVehicle";

        }
        return "Unknown or unimplemented";
    }

    [[nodiscard]] std::string toString() const;
    Object(ObjectID id);
    Object() = default;
    ObjectID getID() const { return id; }

    // Simulation state
    sf::Vector2f position{0.f, 0.f};
    sf::Vector2f velocity{0.f, 0.f};
    bool owner = false;
    Authority authority = Authority::Server;
    float rotation = 0.f;

    // rendering stuff
    std::unique_ptr<sf::Drawable> renderer;
    float radius = 0.f; // for CircleShape
    sf::Vector2f size; // for RectangleShape
    sf::Color color = sf::Color::White;
    Type t = Type::GenericCircle;

    template <typename T>
    void setRenderer(T shape) {
        static_assert(std::is_base_of<sf::Drawable, T>::value,
                      "Renderer must inherit from sf::Drawable");
        renderer = std::make_unique<T>(shape);
    }

    void update(float dt) {
        position += velocity * dt;
    }

    void rebuildRenderer();
    void applyColor();

    // Networking helpers
    friend sf::Packet& operator<<(sf::Packet& packet, const Object& obj);
    friend sf::Packet& operator>>(sf::Packet& packet, Object& obj);

    static sf::Packet& serializeField(sf::Packet& packet, const Object& obj, ObjectField field);
    static void deserializeField(sf::Packet& packet, Object& obj, ObjectField field);

private:
    ObjectID id = 0;
};
