# Summer Maxxing (Text-Based Mini-Game)

A small, terminal-based life-sim for COMP2113/ENGG1340. You play through a summer break, balancing Energy, Health, Social, Academic, Fitness, and Money across 12 weeks, with 4 scenarios each week (48 scenarios total).

## Features
- Stats system: Energy, Health, Social, Academic, Fitness, Money
- Time system: 12 weeks × 4 scenarios per week (48 scenarios)
- Basic activities (placeholder): e.g. Sleep, Workout, Healthy Meal
- Random weekly events: small pool of positive/negative events
- Relationships container: simple NPC list with affinity
- Save/Load: plain text file (`savegame.txt`)
- Multi-file C++ project: headers and sources split by module

Note: This is a working skeleton meant to be extended. No interactive menus yet; the loop runs and prints progress so we can verify pacing and stats.

## How to Build & Run

Linux (recommended):
```
make
./summer_maxxing
```

Windows (PowerShell, MSYS2 g++ installed):
```
g++ -std=c++17 main.cpp player.cpp relationships.cpp events.cpp activities.cpp savegame.cpp -o summer_maxxing.exe
./summer_maxxing.exe
```

If `make` isn’t available on your system, just use the direct `g++` command above.

## Files & Structure
- `main.cpp`: game loop (12 weeks × 4 scenarios), basic flow
- `player.h/.cpp`: player stats, adjust/clamp/print
- `events.h/.cpp`: simple weekly random events + roll logic
- `activities.h/.cpp`: activities and their stat effects (placeholders)
- `relationships.h/.cpp`: minimal NPC storage + affinity tweak
- `savegame.h/.cpp`: save/load to text file
- `Makefile`: Linux build convenience (optional)

## Mapping to Project Requirements
- Random events: yes (`events.*`)
- Data structures: yes (classes, `std::vector` for NPCs/events)
- Dynamic memory management: via STL dynamic containers (no manual `new/delete` needed here)
- File I/O: yes (`savegame.*`)
- Multiple files: yes (separated headers/sources)
- Multiple difficulty levels: not implemented yet (planned next)

## Roadmap (short)
- Add an input-driven menu to choose activities per scenario
- Implement difficulty selection at start (Easy/Normal/Hard)
- Expand event pool and location-specific activities
- End-of-summer summary/outcomes

## Team
- [Add team member names and GitHub IDs here!]

## Notes
- No non-standard runtime libraries are required. Standard C++17 only.
- The code should be able to compile on the Academy server with `g++`.


