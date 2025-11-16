#include "events.h"
#include "player.h"
#include <cstdlib>
#include <ctime>

using namespace std;

static bool seed_done = [](){ srand((unsigned)time(nullptr)); return true; }();

namespace Events {

std::vector<Event> generateWeeklyEvents()
{
    std::vector<Event> out;

    out.push_back({"Inheritance", "A distant relative leaves you some money.", 0.05,
                   [](Player &p){ p.adjustMoney(500); }});

    out.push_back({"Gym Sale", "Gym equipment is cheaper this week.", 0.10,
                   [](Player &p){ /* could set a flag or reduce costs */ p.adjustSocial(0); }});

    out.push_back({"Study Breakthrough", "You finally understand a tough concept.", 0.15,
                   [](Player &p){ p.adjustAcademic(20); }});

    out.push_back({"Sports Injury", "You injure yourself during exercise.", 0.05,
                   [](Player &p){ p.adjustHealth(-20); p.adjustFitness(-10); p.adjustMoney(-100); }});

    return out;
}

bool rollEvent(const Event &e)
{
    double r = (double)rand() / RAND_MAX;
    return r < e.probability;
}

} // namespace Events
