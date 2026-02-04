#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <map>
#include <random>
#include "Trail.h"

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
    PlayerVehicle, // the thing that the players must defend
    Bullet, // player general bullet
    PlayerTurret1, // the turrets 1 and 2 are basic gun turrets
    PlayerTurret2,
    Enemy1, // generic enemy unit maybe it will shoot at the player later

};



// Each property that can be synced over network
enum class ObjectField : uint8_t {
    Position,
    Velocity,
    Owner,
    Authority,
    SendUpdates,
    Type,
    Rotation,
    ShapeCircle, // radius
    ShapeRect,   // width & height
    Color,
    Radius,
    Size,
    DrawOrder,
};


class Object {
public:

    inline ObjectID randomLocalID() {
        static std::mt19937 rng{std::random_device{}()};
        static std::uniform_int_distribution<ObjectID> dist(
            1'000'000'000, 0xFFFFFFFF
        );
        return dist(rng);
    }

    static const char* AuthorityToString(Authority a) {
        switch (a) {
            case Authority::Server: return "Server";
            case Authority::Client: return "Client";
        }
        return "Unknown";
    }

    static const char* TypeToString(Type t) {
        switch (t) {
            case Type::GenericCircle:  return "GenericCircle\n";
            case Type::GenericRectangle:   return "GenericRectangle\n";
            case Type::PlayerVehicle:   return "PlayerVehicle\n";
            case Type::PlayerTurret1:   return "PlayerTurret1\n";
            case Type::Bullet:     return "Bullet\n";
            case Type::PlayerTurret2:   return "PlayerTurret2\n";
            case Type::Enemy1: return "Enemy1\n";

        }
        return "unknown or not implemented";
    }

    [[nodiscard]] std::string toString() const;
    Object(ObjectID id);
    Object() = default;
    ObjectID getID() const { return id; }

    // Simulation state
    sf::Vector2f position{0.f, 0.f};
    sf::Vector2f velocity{0.f, 0.f};
    bool owner = false;
    bool sendUpdates = true;
    Authority authority = Authority::Server;
    float rotation = 0.f;
    //misc
    int lifetime = -1;
    bool strafe = false; // used by enemies so wont be needed by clients

    // rendering stuff
    std::unique_ptr<sf::Drawable> renderer;
    int drawOrder = 0;
    float radius = 0.f; // for CircleShape
    sf::Vector2f size; // for RectangleShape
    sf::Color color = sf::Color::White;
    Type t = Type::GenericCircle;
    Trail trail = Trail(5);

    template <typename T>
    void setRenderer(T shape) {
        static_assert(std::is_base_of<sf::Drawable, T>::value,
                      "Renderer must inherit from sf::Drawable");
        renderer = std::make_unique<T>(shape);
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
