#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
public:
    int energy;    // 0-100
    int health;    // 0-100
    int social;    // 0-100
    int academic;  // 0-100
    int fitness;   // 0-100
    int money;     // 0 - 5000 (integer dollars)

    Player(int energy = 50, int health = 50, int social = 50,
           int academic = 50, int fitness = 50, int money = 300);

    void adjustEnergy(int delta);
    void adjustHealth(int delta);
    void adjustSocial(int delta);
    void adjustAcademic(int delta);
    void adjustFitness(int delta);
    void adjustMoney(int delta);

    void clampStats();

    void printStats() const;
};

#endif // PLAYER_H
