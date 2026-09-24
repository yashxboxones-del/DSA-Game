// =============================================================
// main.cpp
// Entry point of the 2D Platformer Game.
//
// In a clean Object-Oriented architecture, main() has one single
// responsibility: instantiate the Game object and hand over control
// to the game loop by calling game.run().
// =============================================================

#include "Game.h"
#include <iostream>

int main()
{
    try
    {
        // 1. Create our Game instance
        // This sets up the window and initializes subsystems.
        Game game;

        // 2. Start the game loop
        // Execution will stay inside game.run() until the player exits.
        game.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Error] An unexpected exception occurred: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "[Error] An unknown error occurred.\n";
        return 1;
    }

    return 0; // Return success
}
