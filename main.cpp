#include <iostream>
#include "player.h"
#include "relationships.h"
#include "events.h"
#include "activities.h"
#include "savegame.h"

constexpr int WEEKS = 12;
constexpr int SCENARIOS_PER_WEEK = 4;
constexpr int TOTAL_SCENARIOS = WEEKS * SCENARIOS_PER_WEEK; // 48

class Game {
public:
    Game()
    {
    }

    void run()
    {
        Player player;
        Relationships rels;

        int currentWeek = 1;
        int scenario = 1;

        std::cout << "Welcome to Summer Maxxing (skeleton)\n";
        std::cout << "Total weeks: " << WEEKS << ", scenarios per week: " << SCENARIOS_PER_WEEK << " (" << TOTAL_SCENARIOS << " scenarios)\n";

        // Minimal loop placeholder
        for (currentWeek = 1; currentWeek <= WEEKS; ++currentWeek) {
            std::cout << "--- Week " << currentWeek << " ---\n";
            for (scenario = 1; scenario <= SCENARIOS_PER_WEEK; ++scenario) {
                std::cout << "Scenario " << scenario << ": choose an activity (not implemented)\n";

                // Placeholder: apply a sample activity
                Activity act = Activities::sleep();
                act.apply(player);

                // Generate weekly events only at scenario 1 for example
                if (scenario == 1) {
                    auto events = Events::generateWeeklyEvents();
                    for (auto &e : events) {
                        if (Events::rollEvent(e)) {
                            std::cout << "Event triggered: " << e.name << " - " << e.description << "\n";
                            e.apply(player);
                        }
                    }
                }

                player.clampStats();
                player.printStats();
            }
        }

        std::cout << "Game loop complete (skeleton). Consider adding input and activity choices.\n";
    }
};

int main()
{
    Game g;
    g.run();
    return 0;
}
