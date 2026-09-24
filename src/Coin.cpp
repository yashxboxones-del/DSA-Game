// =============================================================
// Coin.cpp
// Implementation of the Coin collectible class.
//
// Handles coin visuals, floating bobbing animation, and pickup state.
// =============================================================

#include "Coin.h"
#include <cmath>

// -------------------------------------------------------------
// Constructor
// -------------------------------------------------------------
Coin::Coin(float x, float y, int value)
    : m_basePosition({x, y})
    , m_value(value)
    , m_isCollected(false)
    , m_animationTimer(0.0f)
    , m_radius(12.0f) // 24px diameter coin
{
    m_shape.setRadius(m_radius);
    m_shape.setPointCount(24); // Smooth round circle

    // Golden yellow fill with rich dark-amber rim
    m_shape.setFillColor(sf::Color(255, 215, 0));
    m_shape.setOutlineThickness(2.0f);
    m_shape.setOutlineColor(sf::Color(205, 133, 0));

    m_shape.setPosition(m_basePosition);
}

// -------------------------------------------------------------
// update()
// Animates a gentle floating up-and-down motion using sine wave.
// -------------------------------------------------------------
void Coin::update(float dt)
{
    if (m_isCollected)
    {
        return; // Inactive coins do not need animation updates
    }

    m_animationTimer += dt;

    // Smooth sinusoidal bobbing: 5 radians/sec, amplitude of 4 pixels
    const float bobbingOffset = std::sin(m_animationTimer * 5.0f) * 4.0f;
    m_shape.setPosition({m_basePosition.x, m_basePosition.y + bobbingOffset});
}

// -------------------------------------------------------------
// render()
// Only draws the coin if it is still uncollected.
// -------------------------------------------------------------
void Coin::render(sf::RenderWindow& window) const
{
    if (!m_isCollected)
    {
        window.draw(m_shape);
    }
}

// -------------------------------------------------------------
// getBounds()
// -------------------------------------------------------------
sf::FloatRect Coin::getBounds() const
{
    return m_shape.getGlobalBounds();
}

// -------------------------------------------------------------
// State accessors
// -------------------------------------------------------------
bool Coin::isCollected() const
{
    return m_isCollected;
}

void Coin::collect()
{
    m_isCollected = true;
}

int Coin::getValue() const
{
    return m_value;
}

sf::Vector2f Coin::getPosition() const
{
    return m_basePosition;
}
