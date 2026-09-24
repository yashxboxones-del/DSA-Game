// =============================================================
// Platform.cpp
// Implementation of the Platform class.
//
// Manages platform visuals and bounding box calculations for
// AABB collision detection.
// =============================================================

#include "Platform.h"

// -------------------------------------------------------------
// Constructor
// -------------------------------------------------------------
Platform::Platform(float x, float y, float width, float height,
                   sf::Color fillColor, sf::Color outlineColor)
    : m_position({x, y})
    , m_size({width, height})
{
    m_shape.setSize(m_size);
    m_shape.setPosition(m_position);
    m_shape.setFillColor(fillColor);
    m_shape.setOutlineThickness(2.0f);
    m_shape.setOutlineColor(outlineColor);
}

// -------------------------------------------------------------
// render()
// -------------------------------------------------------------
void Platform::render(sf::RenderWindow& window) const
{
    window.draw(m_shape);
}

// -------------------------------------------------------------
// getBounds()
// In SFML 3, sf::FloatRect is constructed with position and size:
//   sf::FloatRect(position, size)
// -------------------------------------------------------------
sf::FloatRect Platform::getBounds() const
{
    return sf::FloatRect(m_position, m_size);
}

// -------------------------------------------------------------
// Getters
// -------------------------------------------------------------
sf::Vector2f Platform::getPosition() const
{
    return m_position;
}

sf::Vector2f Platform::getSize() const
{
    return m_size;
}
