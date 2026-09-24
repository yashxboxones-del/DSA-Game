#pragma once

// =============================================================
// Platform.h
// Header file for the Platform class.
//
// Represents a stationary platform in the 2D game world.
// Responsibilities:
//   1. Store position and dimensions (width, height)
//   2. Provide bounding box (sf::FloatRect) for collision detection
//   3. Render visual shape on screen
// =============================================================

#include <SFML/Graphics.hpp>

class Platform
{
public:
    // Constructor: defines coordinates, size, and optional fill color
    Platform(float x, float y, float width, float height,
             sf::Color fillColor = sf::Color(130, 82, 45),
             sf::Color outlineColor = sf::Color(210, 160, 95));

    // Renders the platform rectangle onto the provided window
    void render(sf::RenderWindow& window) const;

    // Returns the Axis-Aligned Bounding Box (AABB) in SFML 3 format
    sf::FloatRect getBounds() const;

    // Getters for position and size
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;

private:
    sf::Vector2f m_position;        // Top-left coordinate of the platform
    sf::Vector2f m_size;            // Width and Height of the platform
    sf::RectangleShape m_shape;     // SFML shape used to draw the platform
};
