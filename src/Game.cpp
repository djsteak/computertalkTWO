#include "Game.h"

//#include <iostream>

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
    TICK += 1;
    if (TICK > 3600) {
        TICK = 0;
    }
    for (auto& [id, obj] : objects) {

        //std::cout << obj.TypeToString(obj.t);
        switch (obj.t) {
            default: {
                obj.position += obj.velocity * dt;
                break;
            }
            case Type::Bullet: {
                obj.position += obj.velocity * dt;

                if (TICK % 5 == 0) {
                    obj.trail.addPoint(obj.position, obj.radius, obj.color);
                }

                obj.trail.update(dt);
                break;
            }
            case Type::PlayerVehicle: {
                sf::Transform transform;
                transform.rotate(sf::degrees(objects[0].rotation));
                obj.velocity = transform.transformPoint(sf::Vector2f(0, 30));
                obj.position += obj.velocity * dt;
                //std::cout << "<" <<obj.velocity.x << " , " << obj.velocity.y << "> rotation: " << obj.rotation << '\n';
                break;
            }
            case Type::PlayerTurret1: {
                //std::cout << obj.getID() << '\n';
                sf::Transform transform;
                transform.rotate(sf::degrees(objects[0].rotation));
                obj.position = transform.transformPoint(sf::Vector2f(-20, -5)) + objects[0].position;
                break;
            }
            case Type::Enemy1: {
                if (obj.owner) {
                    sf::Vector2f targetDeviation = sf::Vector2f(
                        obj.position.x - objects[0].position.x,
                        obj.position.y - objects[0].position.y);

                    double targetDistance = sqrt(
                        pow(targetDeviation.x, 2) +
                        pow(targetDeviation.y, 2));

                    double targetAngle = targetDeviation.angle().asDegrees();
                    double objectAngle = (obj.rotation + 90);
                    double angleDiff;
                    angleDiff = std::remainder(targetAngle - objectAngle, 360.0f);

                    //std::cout << "enemy \t" << obj.getID() << "\t is \t" << targetDistance << "\t away from target the rotations are \t" << targetAngle << "\t" << objectAngle << "\t" << angleDiff << std::endl;
                    if (targetDistance > 360) { // turn around for another strafe
                        obj.strafe = true;
                        if (angleDiff > 0) {
                            obj.rotation -= 3;
                        } else if (angleDiff < 0) {
                            obj.rotation += 3;
                        }
                    } else if (targetDistance > 125) {
                        if (obj.strafe) { // strafing
                            if (angleDiff > 0) {
                                obj.rotation -= 3;
                            } else if (angleDiff < 0) {
                                obj.rotation += 3;
                            }
                        } else if (!obj.strafe) { // running
                            if (angleDiff > 0) {
                                obj.rotation += 3;
                            } else if (angleDiff < 0) {
                                obj.rotation -= 3;
                            }
                        }
                        if (abs(objectAngle - targetAngle) < 0.5) { // fire! (put that code here when ready)

                        }
                    } else if (targetDistance < 125) { // end the strafe
                        obj.strafe = false;
                        if (angleDiff > 0) {
                            obj.rotation += 3;
                        } else if (angleDiff < 0) {
                            obj.rotation -= 3;
                        }
                    }
                    // update velocity after the rotation calculations
                    sf::Transform transform;
                    transform.rotate(sf::degrees(obj.rotation - 90));
                    obj.velocity = transform.transformPoint(sf::Vector2f(-60, 0));
                }
                // and then finally update position
                obj.position += obj.velocity * dt;

                break;
            }
        }
    }
}

void Game::createDrawList() {
    drawList.clear();
    drawList.reserve(objects.size());

    for (auto& [id, obj] : objects) {
        if (std::abs(obj.position.x - cameraPos.x) < 1000 &&
            std::abs(obj.position.y - cameraPos.y) < 800) {
            drawList.push_back(&obj);
            }
    }

    std::sort(drawList.begin(), drawList.end(),
        [](const Object* a, const Object* b) {
            return a->drawOrder < b->drawOrder;
        });
}



// V renders everything
void Game::render(sf::RenderWindow& window) {
    for (auto* obj : drawList) {
        if (!obj->renderer)
            continue;

        sf::RenderStates states;

        // Transformable shapes (Circle / Rect)
        if (auto* t = dynamic_cast<sf::Transformable*>(obj->renderer.get())) {
            t->setPosition(obj->position - cameraPos);
            t->setRotation(sf::degrees(obj->rotation));
            window.draw(*obj->renderer);
        }
        // VertexArray path
        else {
            states.transform.translate(obj->position - cameraPos);
            states.transform.rotate(sf::degrees(obj->rotation));
            window.draw(*obj->renderer, states);
        }

        if (obj->t == Type::Bullet) {
            obj->trail.setCameraPos(cameraPos / 2.f);
            window.draw(obj->trail);
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
