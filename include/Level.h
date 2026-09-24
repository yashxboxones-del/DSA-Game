#pragma once

// =============================================================
// Level.h
// Header file for the Level management class.
//
// Encapsulates all world elements for a given stage:
//   1. Platform layout
//   2. Collectible coins
//   3. Patrolling enemies (stored in custom LinkedList<Enemy>)
//   4. Checkpoints
//   5. Goal / Exit portal
//   6. Player spawn coordinates
// =============================================================

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "Platform.h"
#include "Coin.h"
#include "Enemy.h"
#include "data_structures/LinkedList.h"

// -------------------------------------------------------------
// Checkpoint Definition
// -------------------------------------------------------------
struct LevelCheckpoint
{
    sf::Vector2f position;
    sf::FloatRect triggerBounds;
    bool reached;
    std::string name;

    LevelCheckpoint(float x, float y, const std::string& n = "")
        : position({x, y})
        , triggerBounds(sf::Vector2f{x - 10.0f, y - 50.0f}, sf::Vector2f{40.0f, 60.0f})
        , reached(false)
        , name(n)
    {
    }
};

class Level
{
public:
    // Constructor
    Level();

    // Loads layout, platforms, coins, checkpoints, and enemies for level 1 or 2
    void load(int levelNumber);

    // Updates level objects (coin animations, enemy patrol AI)
    void update(float dt);

    // Draws platforms, coins, enemies, checkpoints, and exit door
    void render(sf::RenderWindow& window) const;

    // Checks if player bounding box touches the level exit goal
    bool checkGoalReached(const sf::FloatRect& playerBounds) const;

    // Accessors
    int getLevelNumber() const;
    const std::string& getLevelName() const;
    sf::Vector2f getSpawnPoint() const;
    sf::FloatRect getGoalBounds() const;

    std::vector<Platform>& getPlatforms();
    const std::vector<Platform>& getPlatforms() const;

    std::vector<Coin>& getCoins();
    const std::vector<Coin>& getCoins() const;

    LinkedList<Enemy>& getEnemies();
    const LinkedList<Enemy>& getEnemies() const;

    std::vector<LevelCheckpoint>& getCheckpoints();
    const std::vector<LevelCheckpoint>& getCheckpoints() const;

private:
    void buildLevel1();
    void buildLevel2();

private:
    int m_levelNumber;
    std::string m_levelName;
    sf::Vector2f m_spawnPoint;

    // Exit portal / goal door
    sf::Vector2f m_goalPosition;
    sf::Vector2f m_goalSize;
    sf::RectangleShape m_goalShape;
    sf::RectangleShape m_goalFrame;

    // Level collections
    std::vector<Platform> m_platforms;
    std::vector<Coin> m_coins;
    LinkedList<Enemy> m_enemies;
    std::vector<LevelCheckpoint> m_checkpoints;
};
