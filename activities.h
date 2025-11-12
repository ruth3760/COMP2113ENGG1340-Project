#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include <string>
#include "player.h"

class Activity {
public:
    std::string name;
    int durationScenarios; // how many scenarios it takes
    Activity(const std::string &n = "", int dur = 1);
    void apply(Player &p) const;

    // simple effect fields (could be expanded)
    int energyChange = 0;
    int healthChange = 0;
    int socialChange = 0;
    int academicChange = 0;
    int fitnessChange = 0;
    int moneyChange = 0;
};

namespace Activities {
    Activity sleep();
    Activity workOut();
    Activity healthyMeal();
}

#endif // ACTIVITIES_H
