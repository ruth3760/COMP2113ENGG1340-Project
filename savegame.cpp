#include "savegame.h"
#include "player.h"
#include <fstream>
#include <cstdio>
#include <cerrno>

bool SaveGame::save(const Player &p, int currentWeek, int scenario, const std::string &path)
{
    // Purpose:
    // --------
    // Saves the current game state (week number, scenario ID, and player stats)
    // into a text file at the specified path.
    // Returns true if saving succeeds, false otherwise.

    // Inputs:
    // -------
    // - p           : A reference to the Player object whose stats will be saved.
    // - currentWeek : The current week number in the game.
    // - scenario    : The current scenario identifier (int).
    // - path        : The file path where the save data should be written.

    // Outputs:
    // --------
    // - Returns a boolean:
    //   * true  → Save succeeded (file opened and data written).
    //   * false → Save failed (file could not be opened).
    // - Side effect: Creates or overwrites a text file at 'path' containing
    //   the serialized game state.
    
    std::ofstream ofs(path);
    if (!ofs) return false;
    ofs << currentWeek << " " << scenario << "\n";
    ofs << p.energy << " " << p.health << " " << p.social << " " << p.academic << " " << p.fitness << " " << p.money << "\n";
    return true;
}

bool SaveGame::load(Player &p, int &currentWeek, int &scenario, const std::string &path)
{
    // Purpose:
    // --------
    // Loads a previously saved game state from a text file.
    // Reads the current week, scenario ID, and player stats from the file
    // and restores them into the program.
    // Returns true if loading succeeds, false otherwise.

    // Inputs:
    // -------
    // - p           : Reference to a Player object whose stats will be updated.
    // - currentWeek : Reference to an integer that will store the loaded week number.
    // - scenario    : Reference to an integer that will store the loaded scenario ID.
    // - path        : The file path where the save data is stored.

    // Outputs:
    // --------
    // - Returns a boolean:
    //   * true  → Load succeeded (file opened and data read correctly).
    //   * false → Load failed (file could not be opened).
    // - Side effect: Updates the Player object and the referenced integers
    //   with values read from the file.
    
    std::ifstream ifs(path);
    if (!ifs) return false;
    ifs >> currentWeek >> scenario;
    ifs >> p.energy >> p.health >> p.social >> p.academic >> p.fitness >> p.money;
    return true;
}

bool SaveGame::clear(const std::string &path)
{
    // Purpose:
    // --------
    // Deletes the save file at the given path.
    // If the file does not exist, it is treated as success so the game flow
    // is not interrupted by a missing file.

    // Inputs:
    // -------
    // - path : The file path (string) of the save file to be deleted.

    // Outputs:
    // --------
    // - Returns a boolean:
    //   * true  → File was successfully deleted OR file did not exist.
    //   * false → File deletion failed for another reason (e.g., permission issues).
    // - Side effect: Removes the file from the filesystem if it exists.
    
    // Treat "file does not exist" as success so we don't block game flow.
    int rc = std::remove(path.c_str());
    return (rc == 0) || (errno == ENOENT);
}
