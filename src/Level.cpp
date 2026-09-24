// =============================================================
// Level.cpp
// Implementation of the Level class.
//
// Configures level layouts, platforms, coins, checkpoints,
// patrolling enemies (LinkedList), and exit portals.
// =============================================================

#include "Level.h"
#include <iostream>

// -------------------------------------------------------------
// Constructor
// -------------------------------------------------------------
Level::Level()
    : m_levelNumber(1)
    , m_levelName("Level 1 - Meadow Rise")
    , m_spawnPoint({80.0f, 540.0f})
    , m_goalPosition({0.0f, 0.0f})
    , m_goalSize({38.0f, 52.0f})
{
    load(1);
}

// -------------------------------------------------------------
// load()
// Loads stage configurations for Level 1 or Level 2.
// -------------------------------------------------------------
void Level::load(int levelNumber)
{
    m_levelNumber = levelNumber;
    m_platforms.clear();
    m_coins.clear();
    m_enemies.clear();
    m_checkpoints.clear();

    if (m_levelNumber == 1)
    {
        buildLevel1();
    }
    else
    {
        buildLevel2();
    }

    // Configure the visual Exit Goal Door
    m_goalShape.setSize(m_goalSize);
    m_goalShape.setPosition(m_goalPosition);
    m_goalShape.setFillColor(sf::Color(255, 215, 80));      // Radiant golden portal
    m_goalShape.setOutlineThickness(3.0f);
    m_goalShape.setOutlineColor(sf::Color(180, 130, 20));    // Deep gold frame

    m_goalFrame.setSize({m_goalSize.x + 8.0f, m_goalSize.y + 4.0f});
    m_goalFrame.setPosition({m_goalPosition.x - 4.0f, m_goalPosition.y - 4.0f});
    m_goalFrame.setFillColor(sf::Color::Transparent);
    m_goalFrame.setOutlineThickness(2.0f);
    m_goalFrame.setOutlineColor(sf::Color(255, 240, 160));  // Outer glowing ring

    std::cout << "[Level] Loaded: " << m_levelName << "\n";
    std::cout << "[Level] Platforms: " << m_platforms.size()
              << " | Coins: " << m_coins.size()
              << " | Enemies (LinkedList): " << m_enemies.size()
              << " | Checkpoints: " << m_checkpoints.size() << "\n";
}

// -------------------------------------------------------------
// buildLevel1() — "Meadow Rise"
// -------------------------------------------------------------
void Level::buildLevel1()
{
    m_levelName = "Level 1 - Meadow Rise";
    m_spawnPoint = {80.0f, 540.0f};

    const sf::Color woodFill(130, 82, 45);
    const sf::Color woodEdge(215, 165, 100);

    // 7 Platforms
    m_platforms.emplace_back(120.0f, 510.0f, 160.0f, 20.0f, woodFill, woodEdge);
    m_platforms.emplace_back(340.0f, 430.0f, 160.0f, 20.0f, woodFill, woodEdge);
    m_platforms.emplace_back(560.0f, 350.0f, 180.0f, 20.0f, woodFill, woodEdge);
    m_platforms.emplace_back(800.0f, 275.0f, 160.0f, 20.0f, woodFill, woodEdge);
    m_platforms.emplace_back(560.0f, 200.0f, 160.0f, 20.0f, woodFill, woodEdge);
    m_platforms.emplace_back(320.0f, 135.0f, 180.0f, 20.0f, woodFill, woodEdge);
    m_platforms.emplace_back(80.0f, 85.0f, 160.0f, 20.0f, sf::Color(165, 110, 55), sf::Color(255, 215, 120));

    // 9 Coins
    m_coins.emplace_back(260.0f, 565.0f, 100);
    m_coins.emplace_back(190.0f, 475.0f, 100);
    m_coins.emplace_back(410.0f, 395.0f, 100);
    m_coins.emplace_back(640.0f, 315.0f, 100);
    m_coins.emplace_back(870.0f, 240.0f, 100);
    m_coins.emplace_back(630.0f, 165.0f, 100);
    m_coins.emplace_back(400.0f, 100.0f, 100);
    m_coins.emplace_back(115.0f, 50.0f, 100);
    m_coins.emplace_back(195.0f, 50.0f, 100);

    // 5 Patrolling Enemies (inserted into LINKED LIST)
    m_enemies.insert(Enemy(450.0f, 564.0f, 360.0f, 720.0f, 85.0f));
    m_enemies.insert(Enemy(360.0f, 394.0f, 345.0f, 495.0f, 70.0f));
    m_enemies.insert(Enemy(600.0f, 314.0f, 565.0f, 735.0f, 75.0f));
    m_enemies.insert(Enemy(830.0f, 239.0f, 805.0f, 955.0f, 80.0f));
    m_enemies.insert(Enemy(350.0f, 99.0f, 325.0f, 495.0f, 85.0f));

    // 2 Checkpoints
    m_checkpoints.emplace_back(640.0f, 350.0f, "Checkpoint 1 - Plateau");
    m_checkpoints.emplace_back(400.0f, 135.0f, "Checkpoint 2 - High Bridge");

    // Exit Goal Portal on Platform 7 (Summit)
    m_goalPosition = {100.0f, 33.0f};
}

// -------------------------------------------------------------
// buildLevel2() — "Cavern Summit"
// -------------------------------------------------------------
void Level::buildLevel2()
{
    m_levelName = "Level 2 - Cavern Summit";
    m_spawnPoint = {60.0f, 540.0f};

    // Dark slate stone with bright crystal blue outline
    const sf::Color stoneFill(65, 75, 95);
    const sf::Color stoneEdge(140, 175, 235);

    // 8 Challenging Platforms
    m_platforms.emplace_back(140.0f, 515.0f, 150.0f, 20.0f, stoneFill, stoneEdge);
    m_platforms.emplace_back(360.0f, 440.0f, 150.0f, 20.0f, stoneFill, stoneEdge);
    m_platforms.emplace_back(180.0f, 365.0f, 150.0f, 20.0f, stoneFill, stoneEdge);
    m_platforms.emplace_back(390.0f, 290.0f, 160.0f, 20.0f, stoneFill, stoneEdge);
    m_platforms.emplace_back(630.0f, 330.0f, 150.0f, 20.0f, stoneFill, stoneEdge);
    m_platforms.emplace_back(850.0f, 260.0f, 150.0f, 20.0f, stoneFill, stoneEdge);
    m_platforms.emplace_back(1060.0f, 190.0f, 160.0f, 20.0f, stoneFill, stoneEdge);
    m_platforms.emplace_back(820.0f, 110.0f, 180.0f, 20.0f, sf::Color(90, 80, 125), sf::Color(255, 215, 120));

    // 10 Coins
    m_coins.emplace_back(270.0f, 565.0f, 100);
    m_coins.emplace_back(210.0f, 480.0f, 100);
    m_coins.emplace_back(430.0f, 405.0f, 100);
    m_coins.emplace_back(250.0f, 330.0f, 100);
    m_coins.emplace_back(465.0f, 255.0f, 100);
    m_coins.emplace_back(700.0f, 295.0f, 100);
    m_coins.emplace_back(920.0f, 225.0f, 100);
    m_coins.emplace_back(1135.0f, 155.0f, 100);
    m_coins.emplace_back(860.0f, 75.0f, 100);
    m_coins.emplace_back(940.0f, 75.0f, 100);

    // 6 Faster Patrolling Enemies in LINKED LIST
    m_enemies.insert(Enemy(500.0f, 564.0f, 350.0f, 780.0f, 95.0f));
    m_enemies.insert(Enemy(390.0f, 404.0f, 365.0f, 500.0f, 75.0f));
    m_enemies.insert(Enemy(210.0f, 329.0f, 185.0f, 320.0f, 80.0f));
    m_enemies.insert(Enemy(660.0f, 294.0f, 635.0f, 770.0f, 85.0f));
    m_enemies.insert(Enemy(880.0f, 224.0f, 855.0f, 990.0f, 90.0f));
    m_enemies.insert(Enemy(1090.0f, 154.0f, 1065.0f, 1210.0f, 95.0f));

    // 2 Checkpoints
    m_checkpoints.emplace_back(465.0f, 290.0f, "Level 2 CP1 - Center Cavern");
    m_checkpoints.emplace_back(1135.0f, 190.0f, "Level 2 CP2 - Outer Spire");

    // Exit Goal Portal on Platform 8 (Grand Summit)
    m_goalPosition = {890.0f, 58.0f};
}

// -------------------------------------------------------------
// update()
// -------------------------------------------------------------
void Level::update(float dt)
{
    // Update coin bobbing animations
    for (auto& coin : m_coins)
    {
        coin.update(dt);
    }

    // Update enemy patrol AI via LINKED LIST traversal
    m_enemies.forEach([dt](Enemy& enemy) {
        enemy.update(dt);
    });
}

// -------------------------------------------------------------
// render()
// -------------------------------------------------------------
void Level::render(sf::RenderWindow& window) const
{
    // 1. Draw all platforms
    for (const auto& platform : m_platforms)
    {
        platform.render(window);
    }

    // 2. Draw checkpoints
    for (const auto& cp : m_checkpoints)
    {
        sf::RectangleShape pole({4.0f, 35.0f});
        pole.setPosition({cp.position.x, cp.position.y - 35.0f});
        pole.setFillColor(sf::Color(200, 200, 200));
        window.draw(pole);

        sf::RectangleShape flag({16.0f, 12.0f});
        flag.setPosition({cp.position.x + 4.0f, cp.position.y - 35.0f});
        if (cp.reached)
        {
            flag.setFillColor(sf::Color(40, 220, 180));
        }
        else
        {
            flag.setFillColor(sf::Color(120, 120, 140));
        }
        window.draw(flag);
    }

    // 3. Draw Exit Goal Portal
    window.draw(m_goalFrame);
    window.draw(m_goalShape);

    // 4. Draw all uncollected coins
    for (const auto& coin : m_coins)
    {
        coin.render(window);
    }

    // 5. Draw all alive enemies via LINKED LIST traversal
    m_enemies.forEach([&window](const Enemy& enemy) {
        enemy.render(window);
    });
}

// -------------------------------------------------------------
// checkGoalReached()
// -------------------------------------------------------------
bool Level::checkGoalReached(const sf::FloatRect& playerBounds) const
{
    return playerBounds.findIntersection(getGoalBounds()).has_value();
}

// -------------------------------------------------------------
// Accessors
// -------------------------------------------------------------
int Level::getLevelNumber() const
{
    return m_levelNumber;
}

const std::string& Level::getLevelName() const
{
    return m_levelName;
}

sf::Vector2f Level::getSpawnPoint() const
{
    return m_spawnPoint;
}

sf::FloatRect Level::getGoalBounds() const
{
    return sf::FloatRect(m_goalPosition, m_goalSize);
}

std::vector<Platform>& Level::getPlatforms()
{
    return m_platforms;
}

const std::vector<Platform>& Level::getPlatforms() const
{
    return m_platforms;
}

std::vector<Coin>& Level::getCoins()
{
    return m_coins;
}

const std::vector<Coin>& Level::getCoins() const
{
    return m_coins;
}

LinkedList<Enemy>& Level::getEnemies()
{
    return m_enemies;
}

const LinkedList<Enemy>& Level::getEnemies() const
{
    return m_enemies;
}

std::vector<LevelCheckpoint>& Level::getCheckpoints()
{
    return m_checkpoints;
}

const std::vector<LevelCheckpoint>& Level::getCheckpoints() const
{
    return m_checkpoints;
}
