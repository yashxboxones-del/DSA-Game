#pragma once

// =============================================================
// Enemy.h
// Header file for the Enemy class.
//
// Represents a patrolling hazard in the 2D platformer.
// Responsibilities:
//   1. Patrol back and forth horizontally between two boundaries
//   2. Turn around smoothly upon reaching patrol limits
//   3. Track alive/defeated state
//   4. Provide collision bounding box (sf::FloatRect)
//   5. Render visual appearance (distinguishable from player)
// =============================================================

#include <SFML/Graphics.hpp>

class Enemy
{
public:
    // Constructor: defines starting position, patrol range, speed, and size
    Enemy(float startX, float startY, float patrolLeft, float patrolRight, float speed = 90.0f);

    // Advances enemy patrol movement by delta time (dt)
    void update(float dt);

    // Draws the enemy if alive
    void render(sf::RenderWindow& window) const;

    // Returns bounding box for collision detection
    sf::FloatRect getBounds() const;

    // State accessors
    bool isAlive() const;
    void defeat();

    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
    int getPointValue() const;

private:
    sf::Vector2f m_position;     // Current top-left coordinate
    sf::Vector2f m_size;         // Width & Height (approx 36x36)
    float m_speed;               // Movement speed in pixels/sec
    int m_direction;             // +1 for right, -1 for left
    float m_patrolLeft;          // Leftmost turnaround X coordinate
    float m_patrolRight;         // Rightmost turnaround X coordinate

    bool m_isAlive;              // False once stomped
    int m_pointValue;            // Score awarded upon defeat (200)

    // Visual shapes
    sf::RectangleShape m_bodyShape;
    sf::RectangleShape m_eyeLeft;
    sf::RectangleShape m_eyeRight;
};
