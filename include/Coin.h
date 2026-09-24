#pragma once

// =============================================================
// Coin.h
// Header file for the Coin collectible class.
//
// Represents a collectible item in the game world.
// Responsibilities:
//   1. Track position, point value, and collected status
//   2. Animate a gentle bobbing/floating effect using delta time
//   3. Provide bounding box (sf::FloatRect) for pickup collision
//   4. Render visually as a golden coin
// =============================================================

#include <SFML/Graphics.hpp>

class Coin
{
public:
    // Constructor: sets coordinates, point value, and visual properties
    Coin(float x, float y, int value = 100);

    // Updates animation (floating/bobbing) using delta time
    void update(float dt);

    // Draws the coin if it has not been collected yet
    void render(sf::RenderWindow& window) const;

    // Returns the bounding box for collision detection
    sf::FloatRect getBounds() const;

    // Checks whether this coin was already picked up
    bool isCollected() const;

    // Marks this coin as collected (deactivates collision & rendering)
    void collect();

    // Returns point value (default: 100)
    int getValue() const;

    // Returns current center/top-left position
    sf::Vector2f getPosition() const;

private:
    sf::Vector2f m_basePosition;   // Original anchor position in the level
    sf::CircleShape m_shape;       // SFML circle representation
    int m_value;                   // Points awarded upon pickup
    bool m_isCollected;            // True if player has collected it

    float m_animationTimer;        // Tracks time for smooth floating animation
    const float m_radius;          // Radius in pixels
};
