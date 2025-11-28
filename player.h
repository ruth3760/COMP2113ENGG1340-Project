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

    // Player()
    // Constructs a Player with the given starting stats and money.
    // Input: initial values for energy, health, social, academic, fitness, and money.
    // Output: Player instance with those values stored in the fields above.
    Player(int energy = 50, int health = 50, int social = 50,
           int academic = 50, int fitness = 50, int money = 300);

    // adjustEnergy()
    // Adjusts the player's energy by a signed delta.
    // Input: delta to add to energy. Output: energy field updated (not clamped).
    void adjustEnergy(int delta);
    // adjustHealth()
    // Adjusts the player's health by a signed delta.
    // Input: delta to add to health. Output: health field updated (not clamped).
    void adjustHealth(int delta);
    // adjustSocial()
    // Adjusts the player's social stat by a signed delta.
    // Input: delta to add to social. Output: social field updated (not clamped).
    void adjustSocial(int delta);
    // adjustAcademic()
    // Adjusts the player's academic stat by a signed delta.
    // Input: delta to add to academic. Output: academic field updated (not clamped).
    void adjustAcademic(int delta);
    // adjustFitness()
    // Adjusts the player's fitness stat by a signed delta.
    // Input: delta to add to fitness. Output: fitness field updated (not clamped).
    void adjustFitness(int delta);
    // adjustMoney()
    // Adjusts the player's money by a signed delta.
    // Input: delta to add to money. Output: money field updated (not clamped).
    void adjustMoney(int delta);

    // clampStats()
    // Clamps all core stats into the 0-100 range and ensures money is non-negative.
    // Input: none. Output: modifies this Player's fields in place.
    void clampStats();

    // printStats()
    // Prints the player's current stats to stdout with text-based bars.
    // Input: none. Output: formatted text for the current Player state.
    void printStats() const;
};

#endif // PLAYER_H
