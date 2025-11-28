#include "savegame.h"
#include "player.h"
#include <fstream>
#include <cstdio>
#include <cerrno>

// SaveGame::save()
// Serializes the current week, scenario, and player's core stats into a plain text save file.
// Input: Player by const reference, currentWeek and scenario indices, and a file path.
// Output: true if the file was opened and written successfully; false on failure to open.
bool SaveGame::save(const Player &p, int currentWeek, int scenario, const std::string &path)
{
    std::ofstream ofs(path);
    if (!ofs) return false;
    ofs << currentWeek << " " << scenario << "\n";
    ofs << p.energy << " " << p.health << " " << p.social << " " << p.academic << " " << p.fitness << " " << p.money << "\n";
    return true;
}

// SaveGame::load()
// Deserializes the save file into the provided Player and progression indices.
// Input: Player by reference to populate, currentWeek and scenario by reference, and file path.
// Output: true if all values were read successfully; false if the file cannot be opened.
bool SaveGame::load(Player &p, int &currentWeek, int &scenario, const std::string &path)
{
    std::ifstream ifs(path);
    if (!ifs) return false;
    ifs >> currentWeek >> scenario;
    ifs >> p.energy >> p.health >> p.social >> p.academic >> p.fitness >> p.money;
    return true;
}

// SaveGame::clear()
// Attempts to remove the save file from disk, ignoring the case where it does not exist.
// Input: file path to remove. Output: true on successful deletion or if the file is already absent.
bool SaveGame::clear(const std::string &path)
{
    // Treat "file does not exist" as success so we don't block game flow.
    int rc = std::remove(path.c_str());
    return (rc == 0) || (errno == ENOENT);
}
