# ==============================================================
# Makefile for PlatformerGame
#
# This Makefile tells the compiler (g++) how to build the game.
# It handles:
#   - Which source files to compile
#   - Where header files and SFML libraries are located
#   - Which SFML DLLs need to be linked
#
# HOW TO USE:
#   Open a terminal (PowerShell or CMD) in this folder and run:
#
#   Option A - if C:/msys64/ucrt64/bin is in PATH:
#       mingw32-make
#       mingw32-make clean
#       mingw32-make run
#
#   Option B - full path:
#       C:/msys64/ucrt64/bin/mingw32-make.exe
#
#   Option C - compile directly with g++:
#       g++ -std=c++17 -Wall -Wextra -g -Iinclude -IC:/msys64/ucrt64/include src/main.cpp src/Game.cpp -o game.exe -LC:/msys64/ucrt64/lib -lsfml-graphics -lsfml-window -lsfml-system
# ==============================================================

# --- Compiler & Flags -------------------------------------------
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# --- Include & Library Paths -----------------------------------
INCLUDES = -Iinclude -IC:/msys64/ucrt64/include
SFML_LIB = -LC:/msys64/ucrt64/lib

# --- SFML Libraries to link ------------------------------------
# Dynamic (.dll) versions of the SFML modules:
# -lsfml-graphics : window rendering, shapes, sprites, text
# -lsfml-window   : OS window creation, input/events
# -lsfml-system   : clocks, time, vectors
SFML_LINK = -lsfml-graphics -lsfml-window -lsfml-system

# --- Source files & Output ------------------------------------
SRC_DIR = src
SRCS    = $(SRC_DIR)/main.cpp $(SRC_DIR)/Game.cpp $(SRC_DIR)/Player.cpp $(SRC_DIR)/Platform.cpp $(SRC_DIR)/Coin.cpp $(SRC_DIR)/Enemy.cpp $(SRC_DIR)/Level.cpp \
          $(SRC_DIR)/data_structures/LinkedList.cpp $(SRC_DIR)/data_structures/Queue.cpp $(SRC_DIR)/data_structures/Stack.cpp
OUTPUT  = game.exe

# --- Build rules ----------------------------------------------

# Default target: build the game
all: $(OUTPUT)

$(OUTPUT): $(SRCS)
	@echo "Building $(OUTPUT) ..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(OUTPUT) $(SFML_LIB) $(SFML_LINK)
	@echo "Build successful! Run with:  ./$(OUTPUT)"

# Run the game after building
run: all
	./$(OUTPUT)

# Remove the compiled executable
clean:
	@echo "Cleaning build files..."
	del /Q $(OUTPUT) 2>nul || true
	@echo "Done."

.PHONY: all run clean
