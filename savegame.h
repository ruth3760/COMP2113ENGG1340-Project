#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <string>

class Player;


namespace SaveGame {
    // Purpose:
    // --------
    // Provides a set of functions for saving, loading, and clearing
    // the game state. Encapsulated in a namespace to avoid naming conflicts
    // and to group related functionality together.

    // Function: save
    // --------------
    // Saves the current game state (week, scenario, and player stats) to a file.
    //
    // Inputs:
    // - p           : Reference to a Player object whose stats will be saved.
    // - currentWeek : The current week number in the game.
    // - scenario    : The current scenario identifier.
    // - path        : Optional file path (default = "savegame.txt").
    //
    // Outputs:
    // - Returns true if saving succeeded, false otherwise.

    bool save(const Player &p, int currentWeek, int scenario, const std::string &path = "savegame.txt");
    bool load(Player &p, int &currentWeek, int &scenario, const std::string &path = "savegame.txt");
    bool clear(const std::string &path = "savegame.txt");
}

#endif // SAVEGAME_H

