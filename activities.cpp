#include "activities.h"
#include <iostream>

// Constructor for the Activity class.
// Initializes the activity with a name and duration.
// Input:
// - n (const std::string&): The name of the activity.
// - dur (int): The duration of the activity in arbitrary units.
// Output: Initializes the `name` and `durationScenarios` member variables.
Activity::Activity(const std::string &n, int dur)
    : name(n), durationScenarios(dur)
{
}

// Applies the effects of the activity to the given player.
// Adjusts the player's stats (energy, health, social, academic, fitness, money) based on the activity's effects.
// Input:
// - p (Player&): The player object whose stats will be modified.
// Output: Updates the player's stats and displays the activity name.
void Activity::apply(Player &p) const
{
    p.adjustEnergy(energyChange);      // Adjusts player's energy based on the activity.
    p.adjustHealth(healthChange);      // Adjusts player's health based on the activity.
    p.adjustSocial(socialChange);      // Adjusts player's social stat based on the activity.
    p.adjustAcademic(academicChange);  // Adjusts player's academic stat based on the activity.
    p.adjustFitness(fitnessChange);    // Adjusts player's fitness based on the activity.
    p.adjustMoney(moneyChange);        // Adjusts player's money based on the activity.
    std::cout << "Performed activity: " << name << "\n"; // Prints the performed activity.
}

namespace Activities {

// Creates and returns an activity representing "Sleep".
// Sleep restores energy.
// Input: None.
// Output: Returns an `Activity` object with predefined effects (increases energy by 50).
Activity sleep()
{
    Activity a("Sleep", 1); // Create an activity named "Sleep" with a duration of 1.
    a.energyChange = +50;   // Increases energy by 50.
    return a;
}

// Creates and returns an activity representing "Work Out".
// Working out decreases energy, increases fitness, and costs money.
// Input: None.
// Output: Returns an `Activity` object with predefined effects (decreases energy by 30, increases fitness by 6, and costs 5 money).
Activity workOut()
{
    Activity a("Work Out", 1); // Create an activity named "Work Out" with a duration of 1.
    a.energyChange = -30;      // Decreases energy by 30.
    a.fitnessChange = +6;      // Increases fitness by 6.
    a.moneyChange = -5;        // Costs 5 money.
    return a;
}

// Creates and returns an activity representing "Healthy Meal".
// Eating a healthy meal improves health and costs money.
// Input: None.
// Output: Returns an `Activity` object with predefined effects (decreases money by 12 and increases health by 15).
Activity healthyMeal()
{
    Activity a("Healthy Meal", 1); // Create an activity named "Healthy Meal" with a duration of 1.
    a.moneyChange = -12;          // Costs 12 money.
    a.healthChange = +15;         // Increases health by 15.
    return a;
}

} // namespace Activities


