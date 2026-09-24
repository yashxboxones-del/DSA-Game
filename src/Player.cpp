// =============================================================
// Player.cpp
// Implementation of the Player class.
//
// Handles player physics, delta-time based movement,
// keyboard inputs, ground collision, health, and combat reaction.
// =============================================================

#include "Player.h"

// -------------------------------------------------------------
// Constructor
// -------------------------------------------------------------
Player::Player(float startX, float startY)
    : m_size({40.0f, 60.0f})
    , m_moveSpeed(300.0f)        // 300 pixels per second
    , m_gravity(1200.0f)         // 1200 pixels per second squared
    , m_jumpVelocity(-520.0f)    // Upward velocity (negative Y is up in SFML)
    , m_position({startX, startY})
    , m_prevPosition({startX, startY})
    , m_velocity({0.0f, 0.0f})
    , m_isGrounded(false)
    , m_health(3)                // Start with 3 hit points
    , m_maxHealth(3)
    , m_invulnerabilityTimer(0.0f)
{
    // Configure visual shape for the player
    m_shape.setSize(m_size);
    m_shape.setFillColor(sf::Color(65, 180, 245));      // Vibrant light blue
    m_shape.setOutlineThickness(2.0f);
    m_shape.setOutlineColor(sf::Color::White);          // White border
    m_shape.setPosition(m_position);
}

// -------------------------------------------------------------
// handleInput()
// Reads direct keyboard state for responsive movement.
// -------------------------------------------------------------
void Player::handleInput()
{
    // 1. Horizontal movement: A/D or Arrow keys
    const bool moveLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
                          sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);

    const bool moveRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
                           sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);

    if (moveLeft && !moveRight)
    {
        m_velocity.x = -m_moveSpeed; // Moving left: negative X
    }
    else if (moveRight && !moveLeft)
    {
        m_velocity.x = m_moveSpeed;  // Moving right: positive X
    }
    else
    {
        // Stop moving horizontally when neither or both keys are pressed
        m_velocity.x = 0.0f;
    }

    // 2. Jumping: Space key
    // Crucial rule: jumping is only allowed if the player is currently grounded!
    const bool jumpPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
    if (jumpPressed && m_isGrounded)
    {
        m_velocity.y = m_jumpVelocity; // Apply upward impulse
        m_isGrounded = false;          // Instantly become airborne
    }
}

// -------------------------------------------------------------
// update()
// Simulates one tick of physics and updates damage cooldown.
// -------------------------------------------------------------
void Player::update(float dt)
{
    // Update invulnerability timer
    if (m_invulnerabilityTimer > 0.0f)
    {
        m_invulnerabilityTimer -= dt;
        if (m_invulnerabilityTimer < 0.0f)
        {
            m_invulnerabilityTimer = 0.0f;
        }
    }

    // Save current position as previous position BEFORE updating physics
    m_prevPosition = m_position;

    // 1. Apply gravity to vertical velocity
    m_velocity.y += m_gravity * dt;

    // 2. Update position using velocity
    m_position.x += m_velocity.x * dt;
    m_position.y += m_velocity.y * dt;

    // 3. Screen horizontal boundaries clamp
    if (m_position.x < 0.0f)
    {
        m_position.x = 0.0f;
    }
    else if (m_position.x + m_size.x > 1280.0f)
    {
        m_position.x = 1280.0f - m_size.x;
    }

    // 4. Update the visual shape to match the new position
    m_shape.setPosition(m_position);
}

// -------------------------------------------------------------
// checkGroundCollision()
// Checks if the player's feet have penetrated the ground line.
// -------------------------------------------------------------
void Player::checkGroundCollision(float groundY)
{
    if (m_position.y + m_size.y >= groundY)
    {
        m_position.y = groundY - m_size.y;
        m_velocity.y = 0.0f;
        m_isGrounded = true;
        m_shape.setPosition(m_position);
    }
}

// -------------------------------------------------------------
// render()
// Flashes the player sprite during the invulnerability period.
// -------------------------------------------------------------
void Player::render(sf::RenderWindow& window) const
{
    if (isInvulnerable())
    {
        // Rapid 10Hz blinking to communicate invulnerability
        const int blinkPhase = static_cast<int>(m_invulnerabilityTimer * 10.0f);
        if (blinkPhase % 2 == 0)
        {
            window.draw(m_shape);
        }
    }
    else
    {
        window.draw(m_shape);
    }
}

// -------------------------------------------------------------
// Health & Combat
// -------------------------------------------------------------
int Player::getHealth() const
{
    return m_health;
}

void Player::setHealth(int health)
{
    m_health = health;
}

bool Player::isInvulnerable() const
{
    return m_invulnerabilityTimer > 0.0f;
}

void Player::takeDamage(int amount, float enemyCenterX)
{
    if (isInvulnerable())
    {
        return; // Immune while damage cooldown is active
    }

    m_health -= amount;
    if (m_health < 0)
    {
        m_health = 0;
    }

    // 1.0 second invulnerability grace period
    m_invulnerabilityTimer = 1.0f;

    // Knockback flinch: push horizontally away from the enemy
    const float playerCenterX = m_position.x + m_size.x * 0.5f;
    if (playerCenterX < enemyCenterX)
    {
        m_velocity.x = -240.0f; // Push left
    }
    else
    {
        m_velocity.x = 240.0f;  // Push right
    }
    m_velocity.y = -180.0f;     // Small upward flinch hop
    m_isGrounded = false;
}

void Player::bounce(float upwardVelocity)
{
    m_velocity.y = upwardVelocity;
    m_isGrounded = false;
}

// -------------------------------------------------------------
// Getters and Setters
// -------------------------------------------------------------
sf::FloatRect Player::getBounds() const
{
    return sf::FloatRect(m_position, m_size);
}

sf::Vector2f Player::getPosition() const
{
    return m_position;
}

sf::Vector2f Player::getPrevPosition() const
{
    return m_prevPosition;
}

sf::Vector2f Player::getSize() const
{
    return m_size;
}

void Player::setPosition(const sf::Vector2f& position)
{
    m_position = position;
    m_shape.setPosition(m_position);
}

sf::Vector2f Player::getVelocity() const
{
    return m_velocity;
}

void Player::setVelocity(const sf::Vector2f& velocity)
{
    m_velocity = velocity;
}

void Player::setVerticalVelocity(float vy)
{
    m_velocity.y = vy;
}

void Player::setHorizontalVelocity(float vx)
{
    m_velocity.x = vx;
}

bool Player::isGrounded() const
{
    return m_isGrounded;
}

void Player::setGrounded(bool grounded)
{
    m_isGrounded = grounded;
}
