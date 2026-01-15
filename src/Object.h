//
// Created by dj on 1/15/26.
//

#ifndef CMAKESFMLPROJECT_OBJECT_H
#define CMAKESFMLPROJECT_OBJECT_H
#include <list>

#include "SFML/Graphics.hpp"

class Object {
public:
    Object() = default;

    void setShape(std::unique_ptr<sf::Shape> newShape) {
        shape = std::move(newShape);
    }

    void draw(sf::RenderWindow& window) {
        if (shape)
            window.draw(*shape);
    }

    sf::Shape* getShape() {
        return shape.get();
    }

private:
    std::unique_ptr<sf::Shape> shape;
};


#endif //CMAKESFMLPROJECT_OBJECT_H