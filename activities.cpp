#include "activities.h"
#include <iostream>

Activity::Activity(const std::string &n, int dur)
    : name(n), durationScenarios(dur)
{
}

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

Activity sleep()
{
    Activity a("Sleep", 1);
    a.energyChange = +50;
    return a;
}

Activity workOut()
{
    Activity a("Work Out", 1);
    a.energyChange = -30;
    a.fitnessChange = +6; // example
    a.moneyChange = -5;
    return a;
}

Activity healthyMeal()
{
    Activity a("Healthy Meal", 1);
    a.moneyChange = -12;
    a.healthChange = +15;
    return a;
}

} // namespace Activities

