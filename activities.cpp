#include "activities.h"
#include <iostream>

// Activity::Activity()
// Initializes an Activity with a name and duration in scenarios.
// Input: activity name and duration. Output: Activity instance with all stat deltas initially zero.
Activity::Activity(const std::string &n, int dur)
    : name(n), durationScenarios(dur)
{
}

// Activity::apply()
// Applies this activity's stat/money changes to the given Player and prints a short message.
// Input: Player to modify by reference. Output: Player has all relevant stats adjusted, not clamped.
void Activity::apply(Player &p) const
{
    p.adjustEnergy(energyChange);
    p.adjustHealth(healthChange);
    p.adjustSocial(socialChange);
    p.adjustAcademic(academicChange);
    p.adjustFitness(fitnessChange);
    p.adjustMoney(moneyChange);
    std::cout << "Performed activity: " << name << "\n";
}

namespace Activities {

// Activities::sleep()
// Returns an Activity that represents sleeping for one scenario.
// Input: none. Output: Activity with a large positive energyChange.
Activity sleep()
{
    Activity a("Sleep", 1);
    a.energyChange = +50;
    return a;
}

// Activities::workOut()
// Returns an Activity that represents a gym workout.
// Input: none. Output: Activity that reduces energy, costs money, and improves fitness.
Activity workOut()
{
    Activity a("Work Out", 1);
    a.energyChange = -30;
    a.fitnessChange = +6; // example
    a.moneyChange = -5;
    return a;
}

// Activities::healthyMeal()
// Returns an Activity that represents eating a healthy meal.
// Input: none. Output: Activity that costs money and increases health.
Activity healthyMeal()
{
    Activity a("Healthy Meal", 1);
    a.moneyChange = -12;
    a.healthChange = +15;
    return a;
}

} // namespace Activities
