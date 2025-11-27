#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <string>

class Player;

namespace SaveGame {
    bool save(const Player &p, int currentWeek, int scenario, const std::string &path = "savegame.txt");
    bool load(Player &p, int &currentWeek, int &scenario, const std::string &path = "savegame.txt");
    bool clear(const std::string &path = "savegame.txt");
}

#endif // SAVEGAME_H
