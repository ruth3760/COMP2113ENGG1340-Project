# Summer Maxxing (Text-Based Mini-Game)

A small, terminal-based life-sim for COMP2113/ENGG1340. You play through a 12-week summer break, balancing Energy, Health, Social, Academic, Fitness, and Money across 4 scenarios per week (48 scenarios total). Your choices affect stats, relationships, money flow, and the final ending you receive.

## What's Here (current state)
- Stats system: Energy, Health, Social, Academic, Fitness, Money
- Time system: 12 weeks - 4 scenarios per week (48 scenarios)
- Interactive scenarios with choices at each time slot
- Location-based activities: sleep, workouts, meals, study sessions, side hustle, social events
- Random weekly and special events: small pool of positive/negative/path-based events
- Relationships container: simple NPC list with affinity (Alex, Sam, Riley, Jordan, etc.)
- Save/Load: plain text file (`savegame.txt`)
- Difficulty selection: Easy / Medium / Hard with different starting stats and decay
- Multi-file C++ project: headers and sources split by module

## How to Build & Run

Linux (Academy server / recommended):
```
make
./summer_maxxing
```

Windows (PowerShell, MSYS2 g++ installed):
```
g++ -std=c++17 main.cpp player.cpp relationships.cpp events.cpp activities.cpp savegame.cpp -o summer_maxxing.exe
./summer_maxxing.exe
```

If `make` isn't available on your system, just use the direct `g++` command above.

On the Academy server (without `make`), you can also run:
```
g++ -std=c++17 main.cpp player.cpp relationships.cpp events.cpp activities.cpp savegame.cpp -o summer_maxxing
./summer_maxxing
```

## Files & Structure
- `main.cpp`: game loop (12 weeks - 4 scenarios), difficulty selection, endings
- `player.h/.cpp`: player stats, adjust/clamp/print
- `events.h/.cpp`: random weekly/special events + roll logic
- `activities.h/.cpp`: activities and their stat effects
- `relationships.h/.cpp`: NPC storage + affinity tracking
- `savegame.h/.cpp`: save/load to text file
- `Makefile`: Linux build convenience (optional)

## Features & Coding Elements
This project is designed to satisfy the COMP2113/ENGG1340 coding requirements:

1. **Generation of random events**
   - Weekly and special events are defined in `events.h/.cpp` and triggered from `main.cpp`.
   - Each event has a probability and an effect on player stats/relationships.

2. **Data structures for storing data**
   - `Player` class (`player.*`) stores core stats and money.
   - `Relationships` and `NPC` (`relationships.*`) use `std::vector` to track multiple characters and affinities.
   - Scenario flow uses `Scenario` and `Choice` structs in `main.cpp` with `std::vector` containers.

3. **Dynamic memory management**
   - Dynamic storage is handled via STL containers (`std::vector`, `std::string`, `std::map`) instead of manual `new/delete`.
   - These are used for NPC lists, scenarios, choices, and event collections.

4. **File input/output**
   - `savegame.h/.cpp` implement saving and loading of player stats, current week, and scenario index to/from `savegame.txt` using file streams.
   - A clear function is provided to remove the save file when starting fresh.

5. **Program codes in multiple files**
   - Logic is split into multiple modules: `main.cpp`, `player.*`, `events.*`, `activities.*`, `relationships.*`, `savegame.*`, and `Makefile`.

6. **Multiple difficulty levels**
   - Difficulty selection (Easy / Medium / Hard) is implemented in `main.cpp` via `DifficultySettings` and `chooseDifficulty`.
   - Each mode changes starting stats, money, weekly decay, allowances, and penalties (e.g. weekly remittance in Hard).

## Roadmap (short)
- Add more UI polish and additional menu shortcuts
- Expand event pool and location-specific activities
- Add more end-of-summer summaries/endings

## Game Description (for report)
- **Game type**: Text-based life-sim / RPG set during a university summer break.
- **Core loop**: For each of 12 weeks, play through 4 scenarios and choose one action per scenario (study, work out, socialize, rest, etc.).
- **Goal**: Finish summer with the best possible combination of stats (Energy/Health/Social/Academic/Fitness/Money) and get a good ending.
- **Systems**: Time management, random events, simple relationships, and difficulty-based stat decay.

## Team
- TODO: Add team member names, student numbers, and GitHub IDs here before submission.

## Notes
- No non-standard runtime libraries are required. Only the C++17 standard library is used.
- The project should compile on the Academy server with the provided `g++` command.
