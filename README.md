# DSA Game

**Street Rush** is a playable Windows desktop game written in C++17 with
[raylib](https://www.raylib.com/). It demonstrates a FIFO queue by managing
traffic: newly spawned cars are enqueued, then moved onto the road from the
front of the queue when there is room.

The game is self-contained and uses raylib's drawing API only; it does not
require external textures, fonts, audio, or other runtime assets.

## Prerequisites

Install the following on Windows:

- Windows 10 or later
- Git (available on `PATH`, required for the automatic raylib download)
- CMake 3.20 or newer
- A C++17 compiler, such as MinGW-w64 from MSYS2 UCRT64

The build has been set up to download raylib 5.5 automatically with CMake.
An installed raylib package is also accepted when CMake can find it.

## Build

Open **MSYS2 UCRT64** or PowerShell in the repository root:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The first configure downloads and builds raylib, so it requires an internet
connection. The executable is written to:

```text
build/DSA-Game.exe
```

## Run

From PowerShell:

```powershell
.\build\DSA-Game.exe
```

Or configure a debugger/IDE to launch `build/DSA-Game.exe` from the repository
root. No asset-copy step is required.

## Controls

| Key | Action |
| --- | --- |
| `Enter` | Start the game from the menu |
| `A` / `Left Arrow` | Steer left |
| `D` / `Right Arrow` | Steer right |
| `Space` | Use nitro |
| `P` | Pause or resume |
| `R` | Restart after a collision |
| Close window | Exit |

## Project layout

```text
.
├── CMakeLists.txt
├── src/
│   └── street_rush.cpp
├── assets/                 # Assets for the separate browser/platformer edition
├── docs/                   # Browser edition
└── include/                # Earlier SFML platformer implementation
```

The CMake target intentionally builds only `src/street_rush.cpp`, the supplied
raylib game. The older SFML platformer files remain in the repository for
reference and are not part of this desktop target.
