// =============================================================
// Game.cpp
// Implementation of the Game class.
//
// Manages the application lifecycle:
//   - Window initialization
//   - Frame-rate independent game loop (using sf::Clock and dt)
//   - COMPLETE UI & GAME STATE SYSTEM:
//       * MENU       : Main menu (Play, Controls, Exit) + Controls screen
//       * PLAYING    : Level gameplay, physics, event queue, HUD
//       * PAUSED     : Translucent overlay, freeze gameplay, Resume/Menu
//       * GAME_OVER  : Respawn (Stack LIFO), Restart Level, Main Menu
//       * WIN        : Victory fanfare, Final Score, Play Again, Exit
//   - DATA STRUCTURES:
//       1. LinkedList<Enemy>   : Enemy management inside Level
//       2. Queue<GameEvent>    : Event queue processed in FIFO order
//       3. Stack<PlayerState>  : Checkpoint history restored in LIFO order
// =============================================================

#include "Game.h"
#include <iostream>

// -------------------------------------------------------------
// Constructor
// -------------------------------------------------------------
Game::Game()
    : m_window(sf::VideoMode({1280u, 720u}), "2D Platformer Adventure (C++ / SFML 3)")
    , m_deltaTime(0.0f)
    , m_state(GameState::MENU)        // Game launches directly into MAIN MENU
    , m_player(80.0f, 540.0f)
    , m_groundY(600.0f)
    , m_currentLevelNumber(1)
    , m_score(0)
    , m_coinsCollected(0)
    , m_menuIndex(0)
    , m_showControlsScreen(false)
    , m_pauseIndex(0)
    , m_gameOverIndex(0)
    , m_winIndex(0)
    , m_isTransitioning(false)
    , m_transitionTimer(0.0f)
    , m_fontLoaded(false)
{
    m_window.setFramerateLimit(60);

    // Set up the permanent safety ground floor
    m_groundShape.setSize({1280.0f, 120.0f});
    m_groundShape.setPosition({0.0f, m_groundY});
    m_groundShape.setFillColor(sf::Color(46, 125, 50));
    m_groundShape.setOutlineThickness(2.0f);
    m_groundShape.setOutlineColor(sf::Color(76, 175, 80));

    // Load font for UI and HUD
    m_fontLoaded = m_font.openFromFile("assets/fonts/arial.ttf");
    if (m_fontLoaded)
    {
        m_scoreText.emplace(m_font, "", 20);
        m_scoreText->setFillColor(sf::Color(255, 255, 255));
        m_scoreText->setOutlineThickness(1.5f);
        m_scoreText->setOutlineColor(sf::Color(0, 0, 0));
        m_scoreText->setPosition({20.0f, 15.0f});

        m_instructionText.emplace(m_font, "[A/D/Arrows]: Move  |  [Space]: Jump  |  [P]: Pause  |  [R]: Respawn Stack  |  [U]: Undo Stack", 15);
        m_instructionText->setFillColor(sf::Color(210, 230, 255));
        m_instructionText->setOutlineThickness(1.0f);
        m_instructionText->setOutlineColor(sf::Color(0, 0, 0));
        m_instructionText->setPosition({20.0f, 685.0f});
    }
    else
    {
        std::cerr << "[Game] Warning: Could not load assets/fonts/arial.ttf\n";
    }

    // Pre-initialize Level 1 data
    loadLevel(1);

    std::cout << "============================================================\n";
    std::cout << "  PHASE 9: UI & STATE SYSTEM INITIALIZED (LAUNCHED IN MENU)\n";
    std::cout << "============================================================\n";
}

// -------------------------------------------------------------
// loadLevel()
// Switches active level, resets player spawn, and initializes checkpoint stack.
// -------------------------------------------------------------
void Game::loadLevel(int levelNumber)
{
    m_currentLevelNumber = levelNumber;
    m_level.load(levelNumber);

    // Position player at level spawn
    m_player.setPosition(m_level.getSpawnPoint());
    m_player.setVelocity({0.0f, 0.0f});
    m_player.setGrounded(true);

    // Reset checkpoint stack with starting spawn point (LIFO)
    m_checkpointStack.clear();
    m_checkpointStack.push(PlayerState(m_level.getSpawnPoint(), m_player.getHealth(), m_score, "Level Spawn"));

    updateScoreUI();
}

// -------------------------------------------------------------
// drawCenteredText()
// Helper to center and draw a text string at yPos.
// -------------------------------------------------------------
void Game::drawCenteredText(const std::string& str, unsigned int charSize, float yPos,
                            sf::Color fillColor, sf::Color outlineColor, float outlineThick)
{
    if (!m_fontLoaded)
    {
        return;
    }

    sf::Text text(m_font, str, charSize);
    text.setFillColor(fillColor);
    text.setOutlineThickness(outlineThick);
    text.setOutlineColor(outlineColor);

    // SFML 3: getLocalBounds().size.x
    const float textWidth = text.getLocalBounds().size.x;
    const float xPos = (1280.0f - textWidth) * 0.5f;
    text.setPosition({xPos, yPos});

    m_window.draw(text);
}

// -------------------------------------------------------------
// run()
// -------------------------------------------------------------
void Game::run()
{
    m_clock.restart();

    while (m_window.isOpen())
    {
        m_deltaTime = m_clock.restart().asSeconds();

        if (m_deltaTime > 0.1f)
        {
            m_deltaTime = 0.1f;
        }

        processEvents();
        update(m_deltaTime);
        render();
    }

    std::cout << "[Game] Loop ended. Window closed.\n";
}

// -------------------------------------------------------------
// processEvents()
// Directs input events strictly according to active GameState.
// -------------------------------------------------------------
void Game::processEvents()
{
    while (const std::optional<sf::Event> event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
            return;
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            const sf::Keyboard::Key key = keyPressed->code;

            switch (m_state)
            {
                case GameState::MENU:
                    handleMenuKey(key);
                    break;

                case GameState::PLAYING:
                    // P pauses gameplay
                    if (key == sf::Keyboard::Key::P)
                    {
                        m_pauseIndex = 0;
                        m_state = GameState::PAUSED;
                        std::cout << "[State] Switched to PAUSED\n";
                    }
                    // R: Restore checkpoint from Stack
                    else if (key == sf::Keyboard::Key::R)
                    {
                        restoreCheckpoint();
                    }
                    // U: Undo checkpoint on Stack
                    else if (key == sf::Keyboard::Key::U)
                    {
                        undoCheckpoint();
                    }
                    // Escape also pauses during active gameplay
                    else if (key == sf::Keyboard::Key::Escape)
                    {
                        m_pauseIndex = 0;
                        m_state = GameState::PAUSED;
                    }
                    break;

                case GameState::PAUSED:
                    handlePausedKey(key);
                    break;

                case GameState::GAME_OVER:
                    handleGameOverKey(key);
                    break;

                case GameState::WIN:
                    handleWinKey(key);
                    break;
            }
        }
    }
}

// -------------------------------------------------------------
// Menu Input Handlers
// -------------------------------------------------------------
void Game::handleMenuKey(sf::Keyboard::Key key)
{
    if (m_showControlsScreen)
    {
        if (key == sf::Keyboard::Key::Escape || key == sf::Keyboard::Key::Enter)
        {
            m_showControlsScreen = false;
        }
        return;
    }

    if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::W)
    {
        m_menuIndex = (m_menuIndex - 1 + 3) % 3;
    }
    else if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S)
    {
        m_menuIndex = (m_menuIndex + 1) % 3;
    }
    else if (key == sf::Keyboard::Key::Enter)
    {
        if (m_menuIndex == 0) // 1. PLAY GAME
        {
            m_score = 0;
            m_coinsCollected = 0;
            m_player.setHealth(3);
            loadLevel(1);
            m_state = GameState::PLAYING;
            m_clock.restart(); // Reset delta-time clock
            std::cout << "[State] Game Started! State: PLAYING (Level 1)\n";
        }
        else if (m_menuIndex == 1) // 2. CONTROLS
        {
            m_showControlsScreen = true;
        }
        else if (m_menuIndex == 2) // 3. EXIT
        {
            m_window.close();
        }
    }
    else if (key == sf::Keyboard::Key::Escape)
    {
        m_window.close();
    }
}

void Game::handlePausedKey(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Key::Escape)
    {
        // ESC resumes gameplay
        m_state = GameState::PLAYING;
        m_clock.restart();
        std::cout << "[State] Resumed gameplay via ESC\n";
        return;
    }

    if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::W)
    {
        m_pauseIndex = (m_pauseIndex - 1 + 2) % 2;
    }
    else if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S)
    {
        m_pauseIndex = (m_pauseIndex + 1) % 2;
    }
    else if (key == sf::Keyboard::Key::Enter)
    {
        if (m_pauseIndex == 0) // RESUME
        {
            m_state = GameState::PLAYING;
            m_clock.restart();
            std::cout << "[State] Resumed gameplay\n";
        }
        else if (m_pauseIndex == 1) // MAIN MENU
        {
            m_state = GameState::MENU;
            std::cout << "[State] Returned to MAIN MENU\n";
        }
    }
}

void Game::handleGameOverKey(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::W)
    {
        m_gameOverIndex = (m_gameOverIndex - 1 + 3) % 3;
    }
    else if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S)
    {
        m_gameOverIndex = (m_gameOverIndex + 1) % 3;
    }
    else if (key == sf::Keyboard::Key::Enter)
    {
        if (m_gameOverIndex == 0) // 1. RESPAWN (Stack LIFO)
        {
            restoreCheckpoint();
            m_state = GameState::PLAYING;
            m_clock.restart();
            std::cout << "[Game Over] Respawned via Checkpoint Stack!\n";
        }
        else if (m_gameOverIndex == 1) // 2. RESTART LEVEL
        {
            m_player.setHealth(3);
            loadLevel(m_currentLevelNumber);
            m_state = GameState::PLAYING;
            m_clock.restart();
            std::cout << "[Game Over] Restarted Level " << m_currentLevelNumber << "\n";
        }
        else if (m_gameOverIndex == 2) // 3. MAIN MENU
        {
            m_state = GameState::MENU;
        }
    }
}

void Game::handleWinKey(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::W)
    {
        m_winIndex = (m_winIndex - 1 + 3) % 3;
    }
    else if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S)
    {
        m_winIndex = (m_winIndex + 1) % 3;
    }
    else if (key == sf::Keyboard::Key::Enter)
    {
        if (m_winIndex == 0) // 1. PLAY AGAIN
        {
            m_score = 0;
            m_coinsCollected = 0;
            m_player.setHealth(3);
            loadLevel(1);
            m_state = GameState::PLAYING;
            m_clock.restart();
            std::cout << "[Victory] Playing again from Level 1!\n";
        }
        else if (m_winIndex == 1) // 2. MAIN MENU
        {
            m_state = GameState::MENU;
        }
        else if (m_winIndex == 2) // 3. EXIT
        {
            m_window.close();
        }
    }
}

// -------------------------------------------------------------
// update()
// -------------------------------------------------------------
void Game::update(float dt)
{
    switch (m_state)
    {
        case GameState::MENU:
            updateMenu(dt);
            break;

        case GameState::PLAYING:
            updatePlaying(dt);
            break;

        case GameState::PAUSED:
            updatePaused(dt); // Gameplay frozen
            break;

        case GameState::GAME_OVER:
            updateGameOver(dt); // Gameplay frozen
            break;

        case GameState::WIN:
            updateWin(dt); // Gameplay frozen
            break;
    }
}

void Game::updateMenu(float /*dt*/)
{
}

void Game::updatePaused(float /*dt*/)
{
    // Deliberately empty: gameplay completely freezes while paused
}

void Game::updateGameOver(float /*dt*/)
{
    // Gameplay frozen on Game Over screen
}

void Game::updateWin(float /*dt*/)
{
    // Gameplay frozen on Victory screen
}

void Game::updatePlaying(float dt)
{
    // Handle short level transition delay
    if (m_isTransitioning)
    {
        m_transitionTimer -= dt;
        if (m_transitionTimer <= 0.0f)
        {
            m_isTransitioning = false;
            loadLevel(2);
        }
        return;
    }

    // 1. Player controls & physics
    m_player.handleInput();
    m_player.update(dt);
    m_player.setGrounded(false);

    // 2. Collision with platforms and ground
    checkPlatformCollisions();
    if (!m_player.isGrounded())
    {
        m_player.checkGroundCollision(m_groundY);
    }

    // 3. Level updates (enemy patrol AI & coin animations)
    m_level.update(dt);

    // 4. Combat & pickup collisions
    checkEnemyCollisions();
    checkCoinCollisions();
    checkCheckpointCollisions();
    checkGoalCollision();

    // 5. Event queue processing (FIFO)
    processEventQueue();
}

// -------------------------------------------------------------
// checkPlatformCollisions()
// -------------------------------------------------------------
void Game::checkPlatformCollisions()
{
    if (m_player.getVelocity().y < 0.0f)
    {
        return;
    }

    const sf::Vector2f prevPos = m_player.getPrevPosition();
    const sf::Vector2f currPos = m_player.getPosition();
    const sf::Vector2f playerSize = m_player.getSize();

    const float prevFeetY = prevPos.y + playerSize.y;
    const float currFeetY = currPos.y + playerSize.y;
    const float playerLeft = currPos.x;
    const float playerRight = currPos.x + playerSize.x;

    for (const auto& platform : m_level.getPlatforms())
    {
        const sf::FloatRect platBounds = platform.getBounds();
        const float platLeft = platBounds.position.x;
        const float platRight = platBounds.position.x + platBounds.size.x;
        const float platTop = platBounds.position.y;
        const float platBottom = platBounds.position.y + platBounds.size.y;

        const bool horizontalOverlap = (playerRight > platLeft) && (playerLeft < platRight);
        const bool crossedTop = (prevFeetY <= platTop + 3.0f) &&
                                (currFeetY >= platTop) &&
                                (prevFeetY <= platBottom);

        if (horizontalOverlap && crossedTop)
        {
            m_player.setPosition({currPos.x, platTop - playerSize.y});
            m_player.setVerticalVelocity(0.0f);
            m_player.setGrounded(true);
            return;
        }
    }
}

// -------------------------------------------------------------
// checkCoinCollisions()
// -------------------------------------------------------------
void Game::checkCoinCollisions()
{
    const sf::FloatRect playerBounds = m_player.getBounds();

    for (auto& coin : m_level.getCoins())
    {
        if (!coin.isCollected())
        {
            if (playerBounds.findIntersection(coin.getBounds()).has_value())
            {
                coin.collect();
                m_eventQueue.enqueue(GameEvent(EventType::COIN_COLLECTED, coin.getValue(), "Coin collected"));
            }
        }
    }
}

// -------------------------------------------------------------
// checkEnemyCollisions()
// -------------------------------------------------------------
void Game::checkEnemyCollisions()
{
    const sf::FloatRect playerBounds = m_player.getBounds();
    const sf::Vector2f playerVel = m_player.getVelocity();
    const sf::Vector2f prevPos = m_player.getPrevPosition();
    const sf::Vector2f playerSize = m_player.getSize();
    const float prevFeetY = prevPos.y + playerSize.y;

    bool enemyDefeated = false;

    // Traverse custom Linked List
    for (auto* node = m_level.getEnemies().getHead(); node != nullptr; node = node->next)
    {
        Enemy& enemy = node->data;
        if (!enemy.isAlive())
        {
            continue;
        }

        const sf::FloatRect enemyBounds = enemy.getBounds();

        if (playerBounds.findIntersection(enemyBounds).has_value())
        {
            const float enemyTop = enemyBounds.position.y;
            const float enemyCenterX = enemyBounds.position.x + enemyBounds.size.x * 0.5f;

            // Stomp
            if (playerVel.y > 0.0f && prevFeetY <= enemyTop + 14.0f)
            {
                enemy.defeat();
                m_player.setPosition({m_player.getPosition().x, enemyTop - playerSize.y});
                m_player.bounce(-380.0f);

                m_eventQueue.enqueue(GameEvent(EventType::ENEMY_DEFEATED, enemy.getPointValue(), "Enemy stomped"));
                enemyDefeated = true;
            }
            // Side hit
            else
            {
                if (!m_player.isInvulnerable())
                {
                    m_player.takeDamage(1, enemyCenterX);
                    m_eventQueue.enqueue(GameEvent(EventType::PLAYER_DAMAGED, 1, "Player took damage"));
                }
            }
        }
    }

    if (enemyDefeated)
    {
        m_level.getEnemies().removeIf([](const Enemy& e) { return !e.isAlive(); });
    }
}

// -------------------------------------------------------------
// checkCheckpointCollisions()
// -------------------------------------------------------------
void Game::checkCheckpointCollisions()
{
    const sf::FloatRect playerBounds = m_player.getBounds();

    for (size_t i = 0; i < m_level.getCheckpoints().size(); ++i)
    {
        auto& cp = m_level.getCheckpoints()[i];
        if (!cp.reached)
        {
            if (playerBounds.findIntersection(cp.triggerBounds).has_value())
            {
                cp.reached = true;
                m_eventQueue.enqueue(GameEvent(EventType::CHECKPOINT_REACHED, static_cast<int>(i), cp.name));
            }
        }
    }
}

// -------------------------------------------------------------
// checkGoalCollision()
// -------------------------------------------------------------
void Game::checkGoalCollision()
{
    if (m_isTransitioning)
    {
        return;
    }

    if (m_level.checkGoalReached(m_player.getBounds()))
    {
        m_eventQueue.enqueue(GameEvent(EventType::LEVEL_COMPLETED, m_currentLevelNumber, "Level Completed"));
    }
}

// -------------------------------------------------------------
// processEventQueue()
// -------------------------------------------------------------
void Game::processEventQueue()
{
    while (!m_eventQueue.isEmpty())
    {
        const GameEvent event = m_eventQueue.front();
        m_eventQueue.dequeue();

        switch (event.type)
        {
            case EventType::COIN_COLLECTED:
                m_score += event.value;
                m_coinsCollected++;
                break;

            case EventType::ENEMY_DEFEATED:
                m_score += event.value;
                break;

            case EventType::PLAYER_DAMAGED:
                if (m_player.getHealth() <= 0)
                {
                    m_gameOverIndex = 0;
                    m_state = GameState::GAME_OVER;
                    std::cout << "[Game Over] Player health is 0! State: GAME_OVER\n";
                }
                break;

            case EventType::CHECKPOINT_REACHED:
            {
                PlayerState newState(m_player.getPosition(), m_player.getHealth(), m_score, event.message);
                m_checkpointStack.push(newState);
                std::cout << "[Stack LIFO] Checkpoint saved: '" << event.message
                          << "' | Stack Depth: " << m_checkpointStack.size() << "\n";
                break;
            }

            case EventType::LEVEL_COMPLETED:
            {
                if (event.value == 1)
                {
                    // Trigger level transition
                    m_isTransitioning = true;
                    m_transitionTimer = 1.2f;
                    m_transitionText = "LEVEL 1 COMPLETE!\nLoading Level 2: Cavern Summit...";
                    std::cout << "[Transition] Level 1 Complete! Loading Level 2 in 1.2s...\n";
                }
                else
                {
                    m_winIndex = 0;
                    m_state = GameState::WIN;
                    std::cout << "[Victory] Level 2 Complete! State: WIN\n";
                }
                break;
            }
        }
    }

    updateScoreUI();
}

// -------------------------------------------------------------
// restoreCheckpoint()
// -------------------------------------------------------------
void Game::restoreCheckpoint()
{
    if (m_checkpointStack.isEmpty())
    {
        return;
    }

    const PlayerState latest = m_checkpointStack.top();

    m_player.setPosition(latest.position);
    m_player.setHealth(3);
    m_player.setVelocity({0.0f, 0.0f});
    m_player.setGrounded(true);

    if (m_state == GameState::GAME_OVER)
    {
        m_state = GameState::PLAYING;
    }

    updateScoreUI();

    std::cout << "[Stack LIFO] Restored player to TOP checkpoint: '" << latest.checkpointName
              << "' at (" << latest.position.x << ", " << latest.position.y << ")\n";
}

// -------------------------------------------------------------
// undoCheckpoint()
// -------------------------------------------------------------
void Game::undoCheckpoint()
{
    if (m_checkpointStack.size() <= 1)
    {
        std::cout << "[Stack] Already at spawn checkpoint. Cannot undo further.\n";
        return;
    }

    m_checkpointStack.pop();
    restoreCheckpoint();
}

// -------------------------------------------------------------
// updateScoreUI()
// -------------------------------------------------------------
void Game::updateScoreUI()
{
    if (m_scoreText)
    {
        m_scoreText->setString("LEVEL: " + std::to_string(m_currentLevelNumber) + "/2   |   " +
                               "SCORE: " + std::to_string(m_score) + "   |   " +
                               "COINS: " + std::to_string(m_coinsCollected) + "   |   " +
                               "HEALTH: " + std::to_string(m_player.getHealth()) + "/3   |   " +
                               "Enemies Left (LinkedList): " + std::to_string(m_level.getEnemies().size()) + "   |   " +
                               "Checkpoints (Stack): " + std::to_string(m_checkpointStack.size()));
    }
}

// -------------------------------------------------------------
// render()
// -------------------------------------------------------------
void Game::render()
{
    switch (m_state)
    {
        case GameState::MENU:
            renderMenu();
            break;

        case GameState::PLAYING:
            renderPlaying();
            break;

        case GameState::PAUSED:
            renderPaused();
            break;

        case GameState::GAME_OVER:
            renderGameOver();
            break;

        case GameState::WIN:
            renderWin();
            break;
    }

    m_window.display();
}

// -------------------------------------------------------------
// renderMenu()
// -------------------------------------------------------------
void Game::renderMenu()
{
    m_window.clear(sf::Color(24, 28, 48)); // Rich dark slate

    if (m_showControlsScreen)
    {
        renderControlsScreen();
        return;
    }

    // Title banner
    drawCenteredText("PLATFORMER ADVENTURE", 46, 120.0f, sf::Color(255, 220, 80), sf::Color(0, 0, 0), 3.0f);
    drawCenteredText("A 2D College Project Demonstrating C++ & Data Structures", 18, 185.0f, sf::Color(180, 210, 255), sf::Color(0, 0, 0), 1.5f);

    // Menu options: 1. PLAY GAME, 2. CONTROLS, 3. EXIT
    const std::string options[3] = {"PLAY GAME", "CONTROLS", "EXIT"};
    const float startY = 280.0f;
    const float stepY = 70.0f;

    for (int i = 0; i < 3; ++i)
    {
        const bool selected = (i == m_menuIndex);
        const std::string label = selected ? ">  " + options[i] + "  <" : options[i];
        const sf::Color color = selected ? sf::Color(255, 235, 90) : sf::Color(200, 205, 220);
        const unsigned int size = selected ? 32 : 28;

        drawCenteredText(label, size, startY + i * stepY, color, sf::Color(0, 0, 0), selected ? 2.5f : 1.5f);
    }

    // Bottom instruction hints
    drawCenteredText("[Up / Down / W / S] : Navigate   |   [Enter] : Select Option", 16, 620.0f, sf::Color(150, 170, 200));
}

// -------------------------------------------------------------
// renderControlsScreen()
// -------------------------------------------------------------
void Game::renderControlsScreen()
{
    drawCenteredText("GAME CONTROLS", 40, 90.0f, sf::Color(255, 220, 80), sf::Color(0, 0, 0), 2.5f);

    const std::string controlsList[] = {
        "A  /  Left Arrow      :  Move Left",
        "D  /  Right Arrow     :  Move Right",
        "SPACE                 :  Jump (Grounded only)",
        "P                     :  Pause Gameplay",
        "R                     :  Restore Latest Checkpoint (Stack LIFO)",
        "U                     :  Undo / Pop Checkpoint (Stack LIFO)",
        "ENTER                 :  Select / Confirm Menu Option",
        "ESC                   :  Back to Menu / Resume Game"
    };

    const float startY = 190.0f;
    const float stepY = 46.0f;

    for (int i = 0; i < 8; ++i)
    {
        drawCenteredText(controlsList[i], 22, startY + i * stepY, sf::Color(230, 240, 255), sf::Color(0, 0, 0), 1.5f);
    }

    drawCenteredText("Press [ESC] or [ENTER] to return to Main Menu", 20, 620.0f, sf::Color(255, 225, 100), sf::Color(0, 0, 0), 2.0f);
}

// -------------------------------------------------------------
// renderPlaying()
// -------------------------------------------------------------
void Game::renderPlaying()
{
    // Stage background sky color
    if (m_currentLevelNumber == 1)
    {
        m_window.clear(sf::Color(35, 45, 75)); // Meadow blue
    }
    else
    {
        m_window.clear(sf::Color(25, 20, 42)); // Cavern purple
    }

    // Safety ground
    m_window.draw(m_groundShape);

    // Active level objects
    m_level.render(m_window);

    // Player
    m_player.render(m_window);

    // HUD Top Bar
    if (m_scoreText)
    {
        m_window.draw(*m_scoreText);
    }

    // HUD Bottom Controls Bar
    if (m_instructionText)
    {
        m_window.draw(*m_instructionText);
    }

    // Interstitial level transition banner
    if (m_isTransitioning)
    {
        sf::RectangleShape banner({700.0f, 160.0f});
        banner.setPosition({290.0f, 260.0f});
        banner.setFillColor(sf::Color(15, 20, 35, 235));
        banner.setOutlineThickness(3.0f);
        banner.setOutlineColor(sf::Color(255, 220, 80));
        m_window.draw(banner);

        drawCenteredText("LEVEL 1 COMPLETE!", 34, 280.0f, sf::Color(255, 235, 90), sf::Color(0, 0, 0), 2.5f);
        drawCenteredText("Loading Level 2: Cavern Summit...", 22, 340.0f, sf::Color(180, 220, 255), sf::Color(0, 0, 0), 1.5f);
    }
}

// -------------------------------------------------------------
// renderPaused()
// -------------------------------------------------------------
void Game::renderPaused()
{
    // 1. Draw gameplay state frozen in the background
    renderPlaying();

    // 2. Draw dark translucent overlay (160/255 opacity)
    sf::RectangleShape overlay({1280.0f, 720.0f});
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    m_window.draw(overlay);

    // 3. Pause Header
    drawCenteredText("PAUSED", 48, 200.0f, sf::Color(255, 225, 90), sf::Color(0, 0, 0), 3.0f);

    // 4. Options: RESUME, MAIN MENU
    const std::string options[2] = {"RESUME", "MAIN MENU"};
    const float startY = 320.0f;
    const float stepY = 70.0f;

    for (int i = 0; i < 2; ++i)
    {
        const bool selected = (i == m_pauseIndex);
        const std::string label = selected ? ">  " + options[i] + "  <" : options[i];
        const sf::Color color = selected ? sf::Color(255, 235, 90) : sf::Color(210, 215, 230);
        const unsigned int size = selected ? 32 : 28;

        drawCenteredText(label, size, startY + i * stepY, color, sf::Color(0, 0, 0), selected ? 2.5f : 1.5f);
    }

    drawCenteredText("[Up / Down / W / S] : Navigate   |   [Enter] : Select   |   [ESC] : Resume", 16, 520.0f, sf::Color(170, 190, 210));
}

// -------------------------------------------------------------
// renderGameOver()
// -------------------------------------------------------------
void Game::renderGameOver()
{
    // 1. Draw gameplay state frozen in background
    renderPlaying();

    // 2. Draw dark crimson translucent overlay
    sf::RectangleShape overlay({1280.0f, 720.0f});
    overlay.setFillColor(sf::Color(45, 10, 10, 195));
    m_window.draw(overlay);

    // 3. Header & Stats
    drawCenteredText("GAME OVER", 52, 140.0f, sf::Color(255, 80, 80), sf::Color(0, 0, 0), 3.5f);

    const std::string statsStr = "Score: " + std::to_string(m_score) + "   |   Coins: " + std::to_string(m_coinsCollected);
    drawCenteredText(statsStr, 24, 220.0f, sf::Color(255, 230, 170), sf::Color(0, 0, 0), 2.0f);

    // 4. Options: RESPAWN (Stack), RESTART LEVEL, MAIN MENU
    const std::string options[3] = {"RESPAWN (Stack LIFO)", "RESTART LEVEL", "MAIN MENU"};
    const float startY = 300.0f;
    const float stepY = 65.0f;

    for (int i = 0; i < 3; ++i)
    {
        const bool selected = (i == m_gameOverIndex);
        const std::string label = selected ? ">  " + options[i] + "  <" : options[i];
        const sf::Color color = selected ? sf::Color(255, 235, 90) : sf::Color(220, 220, 230);
        const unsigned int size = selected ? 30 : 26;

        drawCenteredText(label, size, startY + i * stepY, color, sf::Color(0, 0, 0), selected ? 2.5f : 1.5f);
    }

    drawCenteredText("[Up / Down / W / S] : Navigate   |   [Enter] : Select Option", 16, 560.0f, sf::Color(190, 190, 200));
}

// -------------------------------------------------------------
// renderWin()
// -------------------------------------------------------------
void Game::renderWin()
{
    m_window.clear(sf::Color(18, 48, 28)); // Deep celebratory forest emerald

    drawCenteredText("VICTORY!", 56, 110.0f, sf::Color(255, 235, 90), sf::Color(0, 0, 0), 3.5f);
    drawCenteredText("CONGRATULATIONS! ALL LEVELS COMPLETED!", 24, 190.0f, sf::Color(160, 245, 180), sf::Color(0, 0, 0), 2.0f);

    const std::string statsStr = "FINAL SCORE: " + std::to_string(m_score) + "      TOTAL COINS: " + std::to_string(m_coinsCollected);
    drawCenteredText(statsStr, 26, 250.0f, sf::Color(255, 255, 255), sf::Color(0, 0, 0), 2.0f);

    // Options: PLAY AGAIN, MAIN MENU, EXIT
    const std::string options[3] = {"PLAY AGAIN", "MAIN MENU", "EXIT"};
    const float startY = 340.0f;
    const float stepY = 65.0f;

    for (int i = 0; i < 3; ++i)
    {
        const bool selected = (i == m_winIndex);
        const std::string label = selected ? ">  " + options[i] + "  <" : options[i];
        const sf::Color color = selected ? sf::Color(255, 235, 90) : sf::Color(210, 230, 215);
        const unsigned int size = selected ? 30 : 26;

        drawCenteredText(label, size, startY + i * stepY, color, sf::Color(0, 0, 0), selected ? 2.5f : 1.5f);
    }

    drawCenteredText("[Up / Down / W / S] : Navigate   |   [Enter] : Select Option", 16, 580.0f, sf::Color(160, 200, 175));
}

// -------------------------------------------------------------
// State Getters and Setters
// -------------------------------------------------------------
GameState Game::getState() const
{
    return m_state;
}

void Game::setState(GameState newState)
{
    m_state = newState;
}

int Game::getScore() const
{
    return m_score;
}

int Game::getCoinsCollected() const
{
    return m_coinsCollected;
}
