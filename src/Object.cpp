#include "Object.h"

#include <sstream>

// -------------------- Constructor --------------------
Object::Object(ObjectID id)
    : id(id) {}


// -------------------- Field Serialization --------------------
// IMPORTANT: this copies the objects the specified values into a packet
// IMPORTANT: requires you to add the object id after the message type
sf::Packet& Object::serializeField(sf::Packet& packet, const Object& obj, ObjectField field) {

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
        case ObjectField::Type:
            packet << static_cast<uint8_t>(obj.t);
            break;
        case ObjectField::Color:
            packet << obj.color.a << obj.color.r << obj.color.g << obj.color.b;
            break;
        case ObjectField::Size:
            packet << obj.size.x << obj.size.y;
            break;
        case ObjectField::Radius:
            packet << obj.radius;
    }
    return packet;
}

// IMPORTANT: this takes a field and a value for that field and applies
// IMPORTANT: requires you to add the object id after the message type in creating the packet
void Object::deserializeField(sf::Packet& packet, Object& obj, ObjectField field) {

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
            packet >> obj.color.r >> obj.color.g >> obj.color.b >> obj.color.a;
            obj.applyColor();

            break;
        }
        case ObjectField::Size: {
            packet >> obj.size.x >> obj.size.y;
        }
        case ObjectField::Radius: {
            packet >> obj.radius;
        }
    }
}


// -------------------- Full Serialization --------------------
sf::Packet& operator<<(sf::Packet& packet, const Object& obj) { // puts the entire object into a packet
    packet << obj.id
    << static_cast<uint8_t>(obj.t)
    << static_cast<uint8_t>(obj.authority)
    << obj.position.x << obj.position.y
    << obj.velocity.x << obj.velocity.y
    << obj.owner
    << obj.rotation
    << obj.radius
    << obj.size.x << obj.size.y
    << obj.color.a << obj.color.r << obj.color.g << obj.color.b;
    return packet;
}

sf::Packet& operator>>(sf::Packet& packet, Object& obj) { // takes the object out of a packet
    uint8_t a, t;

    packet >> obj.id
    >> t
    >> a
    >> obj.position.x >> obj.position.y
    >> obj.velocity.x >> obj.velocity.y
    >> obj.owner
    >> obj.rotation
    >> obj.radius
    >> obj.size.x >> obj.size.y
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
    ss << "color: ("
    << static_cast<int>(color.r) << ", "
    << static_cast<int>(color.g) << ", "
    << static_cast<int>(color.b) << ") "
    << "Opacity = "
    << static_cast<int>(color.a)
    << "\n";
    ss << "}";

    return ss.str();
}
// IMPORTANT: rebuilds the renderer because it isn't created on deserialization (also only for client)
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
        case Type::PlayerVehicle: {
            auto r = std::make_unique<sf::VertexArray>(sf::PrimitiveType::TriangleFan);
            r->append(sf::Vertex{{0.f, 0.f}, sf::Color::White, {100.f, 100.f}});
            r->append(sf::Vertex{{0.f, 25.f}, sf::Color::White, {100.f, 100.f}});
            r->append(sf::Vertex{{100.f, 0.f}, sf::Color::White, {100.f, 100.f}});
            r->append(sf::Vertex{{90.f, -25.f}, sf::Color::White, {100.f, 100.f}});
            r->append(sf::Vertex{{10.f, -20.f}, sf::Color::White, {100.f, 100.f}});
            r->append(sf::Vertex{{0.f, -25.f}, sf::Color::White, {100.f, 100.f}});
            r->append(sf::Vertex{{-10.f, -20.f}, sf::Color::White, {100.f, 100.f}});
            r->append(sf::Vertex{{-90.f, -25.f}, sf::Color::White, {100.f, 100.f}});
            r->append(sf::Vertex{{-100.f, 0.f}, sf::Color::White, {100.f, 100.f}});
            r->append(sf::Vertex{{0.f, 25.f}, sf::Color::White, {100.f, 100.f}});



            renderer = std::move(r);
            break;
        }
        default:
            renderer.reset();
            break;
    }
}

void Object::applyColor() {
    if (!renderer) return;

    if (auto* circle = dynamic_cast<sf::CircleShape*>(renderer.get()))
        circle->setFillColor(color);
    else if (auto* rect = dynamic_cast<sf::RectangleShape*>(renderer.get()))
        rect->setFillColor(color);
    else if (auto* shape = dynamic_cast<sf::VertexArray*>(renderer.get())) {

    }
}
