#include "Trail.h"
#include <cmath>

Trail::Trail(std::size_t maxPoints)
: maxPoints(maxPoints),
  mesh(sf::PrimitiveType::TriangleStrip) {
    points.reserve(maxPoints);
}

void Trail::addPoint(sf::Vector2f pos, float width, sf::Color color) {
    TrailPoint p{pos, width, color};

    if (points.size() < maxPoints) {
        points.push_back(p);
    } else {
        points[head] = p;
        head = (head + 1) % maxPoints;
        full = true;
    }

    dirty = true;
}

void Trail::update(float dt) {
    bool changed = false;

    const std::size_t count = full ? maxPoints : points.size();
    for (std::size_t i = 0; i < count; ++i) {
        auto& p = points[i];
        if (p.color.a > 0) {
            p.color.a = static_cast<uint8_t>(
                std::max(0.f, p.color.a - 200.f * dt));
            changed = true;
        }
    }

    if (changed)
        dirty = true;
}

void Trail::clear() {
    points.clear();
    head = 0;
    full = false;
    dirty = true;
}

void Trail::setCameraPos(sf::Vector2f cam) {
    cameraPos = cam;
}

void Trail::draw(sf::RenderTarget& target,
                 sf::RenderStates states) const {
    if (points.size() < 2)
        return;

    rebuildMesh(cameraPos);
    states.transform.translate(-cameraPos);
    target.draw(mesh, states);
}


void Trail::rebuildMesh(const sf::Vector2f& cameraPos) const {
    if (!dirty)
        return;

    mesh.clear();

    const std::size_t count = full ? maxPoints : points.size();
    if (count < 2)
        return;

    auto getPoint = [&](std::size_t i) -> const TrailPoint& {
        return points[(head + i) % count];
    };

    for (std::size_t i = 1; i < count; ++i) {
        const auto& p0 = getPoint(i - 1);
        const auto& p1 = getPoint(i);

        sf::Vector2f dir = p1.pos - p0.pos;
        float len = std::hypot(dir.x, dir.y);
        if (len == 0)
            continue;

        dir /= len;
        sf::Vector2f n(-dir.y, dir.x);

        sf::Vector2f a = p0.pos - cameraPos;
        sf::Vector2f b = p1.pos - cameraPos;

        mesh.append({a + n * (p0.width * 0.5f), p0.color});
        mesh.append({a - n * (p0.width * 0.5f), p0.color});
        mesh.append({b + n * (p1.width * 0.5f), p1.color});
        mesh.append({b - n * (p1.width * 0.5f), p1.color});
    }

    dirty = false;
}
