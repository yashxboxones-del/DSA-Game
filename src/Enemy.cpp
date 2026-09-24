// =============================================================
// Enemy.cpp
// Implementation of the Enemy class.
//
// Manages horizontal platform patrol AI, boundary turning,
// and stomp defeat state.
// =============================================================

#include "Enemy.h"

// -------------------------------------------------------------
// Constructor
// -------------------------------------------------------------
Enemy::Enemy(float startX, float startY, float patrolLeft, float patrolRight, float speed)
    : m_position({startX, startY})
    , m_size({36.0f, 36.0f})
    , m_speed(speed)
    , m_direction(1)              // Start moving right
    , m_patrolLeft(patrolLeft)
    , m_patrolRight(patrolRight)
    , m_isAlive(true)
    , m_pointValue(200)           // 200 points awarded upon defeat
{
    // Body shape: crimson red rectangle with dark border
    m_bodyShape.setSize(m_size);
    m_bodyShape.setFillColor(sf::Color(220, 48, 48));
    m_bodyShape.setOutlineThickness(2.0f);
    m_bodyShape.setOutlineColor(sf::Color(135, 18, 18));
    m_bodyShape.setPosition(m_position);

    // Expressive glowing eyes (visual polish)
    const sf::Vector2f eyeSize({6.0f, 6.0f});
    m_eyeLeft.setSize(eyeSize);
    m_eyeLeft.setFillColor(sf::Color(255, 230, 80)); // Amber yellow eyes
    m_eyeRight.setSize(eyeSize);
    m_eyeRight.setFillColor(sf::Color(255, 230, 80));

    // Position eyes near the upper part of the enemy face
    m_eyeLeft.setPosition({m_position.x + 6.0f, m_position.y + 8.0f});
    m_eyeRight.setPosition({m_position.x + 22.0f, m_position.y + 8.0f});
}

// -------------------------------------------------------------
// update()
// Advances patrol motion and reverses direction at boundaries.
// -------------------------------------------------------------
void Enemy::update(float dt)
{
    if (!m_isAlive)
    {
        return; // Defeated enemies are inert
    }

    // 1. Move horizontally based on direction and delta time
    m_position.x += m_direction * m_speed * dt;

    // 2. Patrol AI: Check left boundary
    if (m_position.x <= m_patrolLeft)
    {
        m_position.x = m_patrolLeft;
        m_direction = 1; // Turn right
    }
    // 3. Patrol AI: Check right boundary
    else if (m_position.x + m_size.x >= m_patrolRight)
    {
        m_position.x = m_patrolRight - m_size.x;
        m_direction = -1; // Turn left
    }

    // 4. Synchronize body and eyes with new position
    m_bodyShape.setPosition(m_position);
    m_eyeLeft.setPosition({m_position.x + 6.0f, m_position.y + 8.0f});
    m_eyeRight.setPosition({m_position.x + 22.0f, m_position.y + 8.0f});
}

// -------------------------------------------------------------
// render()
// -------------------------------------------------------------
void Enemy::render(sf::RenderWindow& window) const
{
    if (m_isAlive)
    {
        window.draw(m_bodyShape);
        window.draw(m_eyeLeft);
        window.draw(m_eyeRight);
    }
}

// -------------------------------------------------------------
// getBounds()
// -------------------------------------------------------------
sf::FloatRect Enemy::getBounds() const
{
    return sf::FloatRect(m_position, m_size);
}

// -------------------------------------------------------------
// State accessors
// -------------------------------------------------------------
bool Enemy::isAlive() const
{
    return m_isAlive;
}

void Enemy::defeat()
{
    m_isAlive = false;
}

sf::Vector2f Enemy::getPosition() const
{
    return m_position;
}

sf::Vector2f Enemy::getSize() const
{
    return m_size;
}

int Enemy::getPointValue() const
{
    return m_pointValue;
}
