#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <string>

class Player;

namespace SaveGame {
    // save()
    // Writes the current player state and progression to a text file.
    // Input: Player by const reference, current week, scenario index, and optional file path.
    // Output: true on successful write, false if the file could not be opened.
    bool save(const Player &p, int currentWeek, int scenario, const std::string &path = "savegame.txt");

    // load()
    // Reads player state and progression from a text file into the given Player and indices.
    // Input: Player by reference to fill, week and scenario by reference, and optional file path.
    // Output: true on successful load, false if the file could not be opened.
    bool load(Player &p, int &currentWeek, int &scenario, const std::string &path = "savegame.txt");

    // clear()
    // Removes the save file from disk, treating a missing file as success.
    // Input: optional file path. Output: true if the file was removed or did not exist, false on other errors.
    bool clear(const std::string &path = "savegame.txt");
}

#endif // SAVEGAME_H
