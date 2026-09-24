#pragma once

// =============================================================
// Game.h
// Header file for the main Game class.
//
// In object-oriented game development, the Game class acts as
// the "conductor" of the entire application. It owns:
//   1. The application window (sf::RenderWindow)
//   2. The clock that tracks time between frames (delta time)
//   3. The current state of the game (GameState enum)
//   4. The main game loop: processEvents(), update(), render()
//   5. Active level management (Level 1 & Level 2 transitions)
//   6. CUSTOM DATA STRUCTURES:
//        - LinkedList<Enemy>     : Inside Level (traversal, removal)
//        - Queue<GameEvent>      : Event system (FIFO processing)
//        - Stack<PlayerState>    : Checkpoint history (LIFO recovery)
//   7. COMPLETE UI & MENU SYSTEM:
//        - Main Menu (Play, Controls, Exit)
//        - Controls overlay
//        - Pause Menu (Resume, Main Menu)
//        - Game Over Screen (Respawn via Stack, Restart, Main Menu)
//        - Victory Screen (Play Again, Main Menu, Exit)
//        - Polished gameplay HUD & transition banners
// =============================================================

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>
#include <optional>
#include <string>

#include "Player.h"
#include "Level.h"

// --- Custom Data Structures ---
#include "data_structures/LinkedList.h"
#include "data_structures/Queue.h"
#include "data_structures/Stack.h"

// -------------------------------------------------------------
// GameState Enum
// -------------------------------------------------------------
enum class GameState
{
    MENU,       // Main menu screen
    PLAYING,    // Active gameplay
    PAUSED,     // Gameplay suspended
    GAME_OVER,  // Player lost all lives/health
    WIN         // Player completed all levels
};

class Game
{
public:
    // Constructor: initializes window, subsystems, menus, and UI
    Game();

    // Destructor: cleans up resources
    ~Game() = default;

    // Starts and runs the main game loop until the window is closed
    void run();

    // State management getters and setters
    GameState getState() const;
    void setState(GameState newState);

    // Level management
    void loadLevel(int levelNumber);

    // Gameplay statistics accessors
    int getScore() const;
    int getCoinsCollected() const;

    // Checkpoint operations using the Stack
    void restoreCheckpoint();
    void undoCheckpoint();

private:
    // Core game loop steps
    void processEvents();
    void update(float dt);
    void render();

    // State-specific keyboard input handlers
    void handleMenuKey(sf::Keyboard::Key key);
    void handlePausedKey(sf::Keyboard::Key key);
    void handleGameOverKey(sf::Keyboard::Key key);
    void handleWinKey(sf::Keyboard::Key key);

    // Helper functions for state-specific updates and rendering
    void updateMenu(float dt);
    void updatePlaying(float dt);
    void updatePaused(float dt);
    void updateGameOver(float dt);
    void updateWin(float dt);

    void renderMenu();
    void renderControlsScreen();
    void renderPlaying();
    void renderPaused();
    void renderGameOver();
    void renderWin();

    // Collision & gameplay helpers
    void checkPlatformCollisions();
    void checkCoinCollisions();
    void checkEnemyCollisions();
    void checkCheckpointCollisions();
    void checkGoalCollision();

    // Event & Queue processing
    void processEventQueue();
    void updateScoreUI();

    // Helper to draw formatted text
    void drawCenteredText(const std::string& str, unsigned int charSize, float yPos,
                          sf::Color fillColor, sf::Color outlineColor = sf::Color::Black,
                          float outlineThick = 2.0f);

private:
    // SFML RenderWindow manages graphical window and OS events
    sf::RenderWindow m_window;

    // Clock used to measure time passed between frames (Delta Time)
    sf::Clock m_clock;

    // Delta time in seconds
    float m_deltaTime;

    // Current state of the game (starts in MENU)
    GameState m_state;

    // Player character
    Player m_player;

    // Permanent safety ground floor
    sf::RectangleShape m_groundShape;
    float m_groundY;

    // Active level manager
    Level m_level;
    int m_currentLevelNumber;

    // =========================================================
    // DATA STRUCTURE 2: Custom FIFO Queue for Game Events
    // =========================================================
    Queue<GameEvent> m_eventQueue;

    // =========================================================
    // DATA STRUCTURE 3: Custom LIFO Stack for Checkpoint History
    // =========================================================
    Stack<PlayerState> m_checkpointStack;

    // Score & statistics
    int m_score;
    int m_coinsCollected;

    // --- UI State & Menu Navigation Indices ---
    int m_menuIndex;             // 0: Play Game, 1: Controls, 2: Exit
    bool m_showControlsScreen;   // True when displaying controls guide
    int m_pauseIndex;            // 0: Resume, 1: Main Menu
    int m_gameOverIndex;         // 0: Respawn, 1: Restart Level, 2: Main Menu
    int m_winIndex;              // 0: Play Again, 1: Main Menu, 2: Exit

    // Level transition interstitial
    bool m_isTransitioning;
    float m_transitionTimer;
    std::string m_transitionText;

    // HUD / Font for displaying stats on screen
    sf::Font m_font;
    bool m_fontLoaded;
    std::optional<sf::Text> m_scoreText;
    std::optional<sf::Text> m_instructionText;
};
