# DSA GAME — College Data Structures Game

**DSA GAME** is a 2D side-scrolling platformer built in **C++ with SFML 3**, showcasing the real-time application of three fundamental linear data structures: **Linked List**, **Queue**, and **Stack**.

## Play DSA GAME

### Windows desktop version

The repository includes the packaged Windows executable **`DSA-GAME.exe`**. To play DSA GAME on Windows:

1. Download `DSA-GAME.exe` from the [latest GitHub release](https://github.com/yashxboxones-del/DSA-Game/releases/latest).
2. Open the downloaded file in Windows Explorer.
3. If Windows SmartScreen appears, review the publisher warning and choose **More info → Run anyway** only if you trust the download.

The executable is a Windows desktop game and must be downloaded before it can be played; it is not browser-playable.

### Browser edition

The project also includes a separate HTML5 Canvas edition. Once GitHub Pages finishes deploying, play it here:

**https://yashxboxones-del.github.io/DSA-Game/**

The browser edition preserves the two levels, platform physics, coins, enemies, checkpoints, menus, pause screen, and victory flow.

---

## Environment

| Tool        | Version / Detail             |
|-------------|------------------------------|
| Language    | C++17                        |
| Game Library| SFML 3.0.2                   |
| Compiler    | g++ 16.1.0 (MSYS2 / UCRT64) |
| OS          | Windows                      |

---

## Project Structure

```
PlatformerGame/
├── include/
│   ├── Game.h
│   ├── Player.h
│   ├── Platform.h
│   ├── Coin.h
│   ├── Enemy.h
│   └── data_structures/
│       ├── LinkedList.h     <- Custom Singly Linked List template
│       ├── Queue.h          <- Custom FIFO Queue template & GameEvent
│       └── Stack.h          <- Custom LIFO Stack template & PlayerState
│
├── src/
│   ├── main.cpp
│   ├── Game.cpp
│   ├── Player.cpp
│   ├── Platform.cpp
│   ├── Coin.cpp
│   ├── Enemy.cpp
│   └── data_structures/
│       ├── LinkedList.cpp   <- Explicit template instantiations
│       ├── Queue.cpp        <- Explicit template instantiations
│       └── Stack.cpp        <- Explicit template instantiations
│
├── assets/
│   ├── fonts/arial.ttf      <- HUD font
│   ├── textures/
│   ├── sounds/
│   └── music/
│
├── DSA-GAME.exe       <- Packaged Windows desktop game
├── docs/              <- Browser edition
├── Makefile
└── README.md
```

---

## Build the Windows desktop version from source

Open PowerShell in the `PlatformerGame/` directory:

**Build:**
```powershell
mingw32-make
```

**Build and run:**
```powershell
mingw32-make run
```

**Clean:**
```powershell
mingw32-make clean
```

> Ensure `C:/msys64/ucrt64/bin` is in your `PATH` so SFML DLLs are found at runtime.

---

## 📚 Data Structures Demonstration

| Data Structure | Game Usage | Principle | Time Complexity |
|---|---|---|---|
| **Linked List** | Enemy management (patrol, collision, stomp deletion) | Sequential dynamic nodes | Insert: $O(1)$, Remove: $O(N)$, Traverse: $O(N)$ |
| **Queue** | Event handling (coin pickups, stomps, damage, checkpoints) | FIFO (First In, First Out) | Enqueue: $O(1)$, Dequeue: $O(1)$, Front: $O(1)$ |
| **Stack** | Checkpoint history & player state recovery | LIFO (Last In, First Out) | Push: $O(1)$, Pop: $O(1)$, Top: $O(1)$ |

---

### 1. Linked List (`LinkedList<Enemy>`)

* **What is it?** A linear data structure where elements are not stored in contiguous memory locations. Instead, each node contains a data payload and a pointer linking to the `next` node in sequence.
* **Why we use it in this game:**
  Enemies are constantly spawned at level initialization and defeated during gameplay. When a player stomps an enemy, that specific node is cleanly spliced out and deleted from memory using `removeIf()`.
  Unlike an array or `std::vector`, removing an element from a linked list does not require shifting subsequent elements in memory.
* **Operations Demonstrated:**
  - `insert(enemy)`: Appends an enemy node in $O(1)$ time using the tail pointer.
  - `forEach(lambda)`: Linearly traverses living enemies for physics updates and rendering ($O(N)$).
  - `removeIf(predicate)`: Safely deletes defeated enemy nodes and relinks pointers ($O(N)$).
  - `clear()`: Traverses and deletes all nodes upon level reset to prevent memory leaks.

---

### 2. Queue (`Queue<GameEvent>`)

* **What is it?** An abstract data type following the **FIFO (First In, First Out)** principle. The first item added to the rear of the queue is the first item extracted from the front.
* **Why we use it in this game:**
  Decouples physics and collision detection from gameplay state updates. When a collision occurs (e.g. coin collected, enemy stomped, damage taken), a `GameEvent` packet is enqueued. During the event phase of the game loop, the queue processes events in the exact sequence they occurred.
* **FIFO Explanation:**
  Suppose in a single frame the player simultaneously stomps an enemy and touches a coin:
  $$\text{Enqueue(Enemy Stomped)} \to \text{Enqueue(Coin Picked)}$$
  The Queue processes `Enemy Stomped` first, awarding 200 points, followed immediately by `Coin Picked`, awarding 100 points.
* **Operations Demonstrated:**
  - `enqueue(event)`: Inserts event at the rear in $O(1)$ time.
  - `dequeue()`: Removes the front event in $O(1)$ time.
  - `front()`: Inspects the front event in $O(1)$ time.
  - `isEmpty()`: Checks if all pending events are resolved.

---

### 3. Stack (`Stack<PlayerState>`)

* **What is it?** An abstract data type following the **LIFO (Last In, First Out)** principle. The last item pushed onto the top of the stack is the first item removed.
* **Why we use it in this game:**
  Provides a checkpoint history and recovery mechanism. When the player reaches a checkpoint banner, a snapshot of the player (`PlayerState` containing position, health, score, and name) is pushed onto the stack. If the player dies or presses <kbd>R</kbd>, the top checkpoint is restored. Pressing <kbd>U</kbd> pops the top checkpoint, allowing the player to fall back to an earlier checkpoint.
* **LIFO Explanation:**
  ```
  [Stack Top]    -> Checkpoint 2 (High Bridge)
  [Middle]       -> Checkpoint 1 (Plateau)
  [Stack Bottom] -> Level Spawn Point
  ```
  Upon death or recovery, Checkpoint 2 (the newest) is restored first. If popped (Undo), Checkpoint 1 becomes the new top.
* **Operations Demonstrated:**
  - `push(state)`: Adds a new checkpoint state to the top in $O(1)$ time.
  - `pop()`: Removes the top checkpoint state in $O(1)$ time.
  - `top()`: Peeks at the active checkpoint state in $O(1)$ time.

---

## 🎮 Controls

| Key | Action | Data Structure Impact |
|---|---|---|
| <kbd>A</kbd> / <kbd>←</kbd> | Move Left | Updates physics |
| <kbd>D</kbd> / <kbd>→</kbd> | Move Right | Updates physics |
| <kbd>Space</kbd> | Jump | Grounded jump impulse |
| <kbd>R</kbd> | Respawn / Restore Checkpoint | Peeks top of **Stack** (LIFO recovery) |
| <kbd>U</kbd> | Undo Checkpoint | Pops top of **Stack** (falls back to previous checkpoint) |
| <kbd>Esc</kbd> | Close game | Cleans up resources |

---

## Development Phases

| Phase | Description                    | Status    |
|-------|-------------------------------|-----------|
| 1     | Project setup + window         | ✅ Done   |
| 2     | Game loop + Game class         | ✅ Done   |
| 3     | Player movement + gravity      | ✅ Done   |
| 4     | Platforms + collision          | ✅ Done   |
| 5     | Coins + score                  | ✅ Done   |
| 6     | Enemies + combat               | ✅ Done   |
| 7     | Data structures integration    | ✅ Done   |
| 8     | Levels + transitions           | ✅ Done   |
| 9     | UI (score, health, menus)      | ✅ Done   |
| 10    | Sound + visual polish          | 🔜 Next   |
| 11    | Testing + bug fixes            | ⏳        |
| 12    | Documentation                  | ⏳        |
