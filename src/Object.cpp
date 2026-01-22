#include "Object.h"

#include <sstream>

// -------------------- Constructor --------------------
Object::Object(ObjectID id)
    : id(id) {}


// -------------------- Field Serialization --------------------
// IMPORTANT: this copies the objects the specified values into a packet
sf::Packet& Object::serializeField(sf::Packet& packet,
                                   const Object& obj,
                                   ObjectField field)
{
    packet << static_cast<uint8_t>(field);
    packet << static_cast<uint8_t>(obj.t);

    if (obj.t == Type::GenericCircle) {
        packet << obj.radius;
    }
    else if (obj.t == Type::GenericRectangle) {
        packet << obj.size.x << obj.size.y;
    }

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
        case ObjectField::Type:
            packet << static_cast<uint8_t>(obj.t);
            break;
        case ObjectField::Color:
            packet << obj.color.a << obj.color.r << obj.color.g << obj.color.b;
            break;
    }
    return packet;
}

// IMPORTANT: this takes whatever values are in the packet and puts them into an object
void Object::deserializeField(sf::Packet& packet, Object& obj) {
    uint8_t raw;
    packet >> raw;
    ObjectField field = static_cast<ObjectField>(raw);
    obj.t = static_cast<Type>(raw);

    if (obj.t == Type::GenericCircle) {
        packet >> obj.radius;
    }
    else if (obj.t == Type::GenericRectangle) {
        packet >> obj.size.x >> obj.size.y;
    }
    switch (field) {
        case ObjectField::Position: {
            packet >> obj.position.x >> obj.position.y;
            break;
        }
        case ObjectField::Velocity: {
            packet >> obj.velocity.x >> obj.velocity.y;
            break;
        }
        case ObjectField::Owner: {
            packet >> obj.owner;
            break;
        }
        case ObjectField::Authority: {
            uint8_t a;
            packet >> a;
            obj.authority = static_cast<Authority>(a);
            break;
        }
        case ObjectField::Rotation: {
            packet >> obj.rotation;
            break;
        }
        case ObjectField::Type: {
            uint8_t t;
            packet >> t;
            obj.t = static_cast<Type>(t);
            break;
        }
        case ObjectField::Color: {
            packet >> obj.color.a >> obj.color.r >> obj.color.b >> obj.color.g;
            break;
        }
    }
}


// -------------------- Full Serialization --------------------
sf::Packet& operator<<(sf::Packet& packet, const Object& obj) { // puts the entire object into a packet
    packet << obj.id
    << obj.position.x << obj.position.y
    << obj.velocity.x << obj.velocity.y
    << obj.owner
    << static_cast<uint8_t>(obj.authority)
    << obj.rotation
    << obj.radius
    << obj.size.x << obj.size.y
    << static_cast<uint8_t>(obj.t)
    << obj.color.a << obj.color.r << obj.color.g << obj.color.b;
    return packet;
}

sf::Packet& operator>>(sf::Packet& packet, Object& obj) { // takes the object out of a packet
    uint8_t a, t;

    packet >> obj.id
    >> obj.position.x >> obj.position.y
    >> obj.velocity.x >> obj.velocity.y
    >> obj.owner
    >> a
    >> obj.rotation
    >> obj.radius
    >> obj.size.x >> obj.size.y
    >> t
    >> obj.color.a >> obj.color.r >> obj.color.g >> obj.color.b;

    obj.authority = static_cast<Authority>(a);
    obj.t = static_cast<Type>(t);

    obj.rebuildRenderer();
    return packet;
}


std::string Object::toString() const {
    std::ostringstream ss;

    ss << "Object {\n";
    ss << "  id: " << id << "\n";
    ss << "  type: " << TypeToString(t) << "\n";
    ss << "  authority: " << AuthorityToString(authority) << "\n";
    ss << "  owner: " << (owner ? "true" : "false") << "\n";
    ss << "  position: (" << position.x << ", " << position.y << ")\n";
    ss << "  velocity: (" << velocity.x << ", " << velocity.y << ")\n";
    ss << "  rotation: " << rotation << "\n";
    ss << "  radius: " << radius << "\n";
    ss << "  size: (" << size.x << ", " << size.y << ")\n";
    ss << "  color: (" << color.r << ", " << color.g << ", " << color.b << ") Opacity = " << color.a << "\n";

    // Renderer info
    if (!renderer) {
        ss << "  renderer: null\n";
    } else if (radius > 0.f) {
        ss << "  renderer: Circle (radius=" << radius << ")\n";
    } else if (size.x > 0.f || size.y > 0.f) {
        ss << "  renderer: Rectangle ("
           << size.x << " x " << size.y << ")\n";
    } else {
        ss << "  renderer: Unknown drawable\n";
    }

    ss << "}";

    return ss.str();
}
// IMPORTANT: rebuolds the renderer because it isnt created on deserialization (also only for client)
void Object::rebuildRenderer() {
    switch (t) {
        case Type::GenericCircle: {
            auto c = std::make_unique<sf::CircleShape>(radius);
            c->setOrigin(sf::Vector2f(radius, radius));
            renderer = std::move(c);
            break;
        }
        case Type::GenericRectangle: {
            auto r = std::make_unique<sf::RectangleShape>(size);
            r->setOrigin(size);
            renderer = std::move(r);
            break;
        }
        default:
            renderer.reset();
            break;
    }
}

