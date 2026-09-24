#pragma once

// =============================================================
// Player.h
// Header file for the Player class.
//
// Represents the user-controlled character in our 2D platformer.
// The Player class is responsible for:
//   1. Handling keyboard input (A/D/Arrows for move, Space for jump)
//   2. Managing physical movement (velocity, gravity, position)
//   3. Tracking previous and current positions for continuous collision
//   4. Health and invulnerability system (damage cooldown, flashing)
//   5. Ground detection and jump validation (prevent double-jumping)
//   6. Rendering the character's visual representation on screen
// =============================================================

#include <SFML/Graphics.hpp>

class Player
{
public:
    // Constructor: sets initial position, health, and visual appearance
    Player(float startX = 100.0f, float startY = 300.0f);

    // Reads real-time keyboard state (A/D/Arrows, Space)
    void handleInput();

    // Advances physics and invulnerability timer by delta time (dt)
    void update(float dt);

    // Checks collision against the bottom ground floor line
    void checkGroundCollision(float groundY);

    // Draws the player (with flashing effect if invulnerable)
    void render(sf::RenderWindow& window) const;

    // --- Health & Combat ---
    int getHealth() const;
    void setHealth(int health);
    bool isInvulnerable() const;
    void takeDamage(int amount, float enemyCenterX);
    void bounce(float upwardVelocity = -380.0f);

    // --- Getters and Setters ---
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    sf::Vector2f getPrevPosition() const;
    sf::Vector2f getSize() const;
    void setPosition(const sf::Vector2f& position);

    sf::Vector2f getVelocity() const;
    void setVelocity(const sf::Vector2f& velocity);
    void setVerticalVelocity(float vy);
    void setHorizontalVelocity(float vx);

    bool isGrounded() const;
    void setGrounded(bool grounded);

private:
    // Dimensions of the player box
    const sf::Vector2f m_size;

    // Physics parameters
    const float m_moveSpeed;     // Horizontal speed in pixels/sec
    const float m_gravity;       // Downward acceleration in pixels/sec²
    const float m_jumpVelocity;  // Initial upward speed (negative Y) in pixels/sec

    // Physical state
    sf::Vector2f m_position;     // Top-left corner of the player (current frame)
    sf::Vector2f m_prevPosition; // Top-left corner before physics update (previous frame)
    sf::Vector2f m_velocity;     // Current speed in X and Y directions
    bool m_isGrounded;           // True if standing firmly on the ground/platform

    // Health & Invulnerability state
    int m_health;                // Current hit points (default: 3)
    const int m_maxHealth;       // Maximum hit points (3)
    float m_invulnerabilityTimer;// Seconds remaining of damage invulnerability (e.g. 1.0s)

    // Visual shape (placeholder until sprites in Phase 10)
    sf::RectangleShape m_shape;
};
