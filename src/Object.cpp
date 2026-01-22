#include "Object.h"

// -------------------- Constructor --------------------
Object::Object(ObjectID id, std::unique_ptr<sf::Drawable> renderer)
    : id(id), renderer(std::move(renderer)) {}


// -------------------- Field Serialization --------------------
sf::Packet& Object::serializeField(sf::Packet& packet, const Object& obj, ObjectField field) {
    packet << static_cast<std::uint32_t>(obj.id);
    packet << static_cast<uint8_t>(field);

    switch (field) {
        case ObjectField::Position:
            packet << obj.position.x << obj.position.y;
            break;
        case ObjectField::Velocity:
            packet << obj.velocity.x << obj.velocity.y;
            break;
        case ObjectField::Owner:
            packet << obj.owner;
            break;
        case ObjectField::Authority:
            packet << static_cast<uint8_t>(obj.authority);
            break;
        case ObjectField::Rotation:
            packet << obj.rotation;
            break;
        case ObjectField::ShapeCircle:
            packet << obj.radius;
            break;
        case ObjectField::ShapeRect:
            packet << obj.width << obj.height;
            break;
        case ObjectField::Type:
            packet << static_cast<uint8_t>(obj.t);
            break;
    }
    return packet;
}

void Object::deserializeField(sf::Packet& packet, Object& obj, ObjectField field) {
    switch (field) {
        case ObjectField::Position: {
            float x, y;
            packet >> x >> y;
            obj.position = {x, y};
            break;
        }
        case ObjectField::Velocity: {
            float vx, vy;
            packet >> vx >> vy;
            obj.velocity = {vx, vy};
            break;
        }
        case ObjectField::Owner: {
            bool owner;
            packet >> owner;
            obj.owner = owner;
            break;
        }
        case ObjectField::Authority: {
            uint8_t raw;
            packet >> raw;
            obj.authority = static_cast<Authority>(raw);
            break;
        }
        case ObjectField::Rotation: {
            float rot;
            packet >> rot;
            obj.rotation = rot;
            break;
        }
        case ObjectField::ShapeCircle: {
            float r;
            packet >> r;
            obj.radius = r;
            obj.renderer = std::make_unique<sf::CircleShape>(r);
            break;
        }
        case ObjectField::ShapeRect: {
            float w, h;
            packet >> w >> h;
            obj.width = w;
            obj.height = h;
            obj.renderer = std::make_unique<sf::RectangleShape>(sf::Vector2f(w, h));
            break;
        }
        case ObjectField::Type: {
            uint8_t raw;
            packet >> raw;
            obj.t = static_cast<Type>(raw);
            break;
        }
    }
}

// -------------------- Full Serialization --------------------
sf::Packet& operator<<(sf::Packet& packet, const Object& obj) {
    packet << obj.id;
    // Serialize all fields in a consistent order
    Object::serializeField(packet, obj, ObjectField::Position);
    Object::serializeField(packet, obj, ObjectField::Velocity);
    Object::serializeField(packet, obj, ObjectField::Owner);
    Object::serializeField(packet, obj, ObjectField::Authority);
    Object::serializeField(packet, obj, ObjectField::Rotation);
    Object::serializeField(packet, obj, ObjectField::ShapeCircle);
    Object::serializeField(packet, obj, ObjectField::ShapeRect);
    Object::serializeField(packet, obj, ObjectField::Type);
    return packet;
}

sf::Packet& operator>>(sf::Packet& packet, Object& obj) {
    packet >> obj.id;
    Object::deserializeField(packet, obj, ObjectField::Position);
    Object::deserializeField(packet, obj, ObjectField::Velocity);
    Object::deserializeField(packet, obj, ObjectField::Owner);
    Object::deserializeField(packet, obj, ObjectField::Authority);
    Object::deserializeField(packet, obj, ObjectField::Rotation);
    Object::deserializeField(packet, obj, ObjectField::ShapeCircle);
    Object::deserializeField(packet, obj, ObjectField::ShapeRect);
    Object::deserializeField(packet, obj, ObjectField::Type);
    return packet;
}
