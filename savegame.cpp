#include "savegame.h"
#include "player.h"
#include <fstream>

bool SaveGame::save(const Player &p, int currentWeek, int scenario, const std::string &path)
{
    std::ofstream ofs(path);
    if (!ofs) return false;
    ofs << currentWeek << " " << scenario << "\n";
    ofs << p.energy << " " << p.health << " " << p.social << " " << p.academic << " " << p.fitness << " " << p.money << "\n";
    return true;
}

bool SaveGame::load(Player &p, int &currentWeek, int &scenario, const std::string &path)
{
    std::ifstream ifs(path);
    if (!ifs) return false;
    ifs >> currentWeek >> scenario;
    ifs >> p.energy >> p.health >> p.social >> p.academic >> p.fitness >> p.money;
    return true;
}
