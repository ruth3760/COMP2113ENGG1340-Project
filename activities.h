#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include <string>
#include "player.h"

class Activity {
public:
    std::string name;
    int durationScenarios; // how many scenarios it takes

    // Activity()
    // Constructs a basic activity with a name and time cost in scenarios.
    // Input: activity name and duration in scenarios. Output: Activity instance with default zero stat changes.
    Activity(const std::string &n = "", int dur = 1);

    // apply()
    // Applies this activity's stat and money changes to the given Player.
    // Input: Player to modify by reference. Output: Player stats updated according to the change fields below.
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
    // sleep()
    // Creates an Activity representing sleeping to restore energy.
    // Input: none. Output: Activity with a large positive energyChange.
    Activity sleep();

    // workOut()
    // Creates an Activity representing working out at the gym.
    // Input: none. Output: Activity that costs energy and money but increases fitness.
    Activity workOut();

    // healthyMeal()
    // Creates an Activity representing eating a healthy meal.
    // Input: none. Output: Activity that costs money but improves health.
    Activity healthyMeal();
}

#endif // ACTIVITIES_H
