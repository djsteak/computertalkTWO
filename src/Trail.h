#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct TrailPoint {
    sf::Vector2f pos;
    float width;
    sf::Color color;
};

class Trail : public sf::Drawable {
public:
    explicit Trail(std::size_t maxPoints = 16);

    void addPoint(sf::Vector2f pos, float width, sf::Color color);
    void update(float dt);
    void clear();
    sf::Vector2f cameraPos;
    void draw(sf::RenderTarget& target,
              sf::RenderStates states) const override;
    void setCameraPos(sf::Vector2f cam);
private:
    void rebuildMesh(const sf::Vector2f& cameraPos) const;



    std::vector<TrailPoint> points;
    std::size_t maxPoints;
    std::size_t head = 0;
    bool full = false;

    mutable bool dirty = true;
    mutable sf::VertexArray mesh;
};
