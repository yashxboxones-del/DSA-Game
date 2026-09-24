cmake_minimum_required(VERSION 3.20)

project(DSAGame LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

find_package(raylib 5.0 QUIET)

if (NOT raylib_FOUND)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(raylib)
endif()

add_executable(dsa_game WIN32
    src/street_rush.cpp
)

target_link_libraries(dsa_game PRIVATE raylib)

if (MSVC)
    target_compile_options(dsa_game PRIVATE /W4)
else()
    target_compile_options(dsa_game PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (WIN32)
    set_target_properties(dsa_game PROPERTIES OUTPUT_NAME "DSA-Game")
endif()
