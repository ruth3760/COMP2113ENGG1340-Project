#include "player.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>


// Player::Player()
// Initializes a Player instance with the provided starting stats and money.
// Input: initial values for energy, health, social, academic, fitness, and money.
// Output: Player object constructed with these values stored in member fields.
Player::Player(int energy_, int health_, int social_, int academic_, int fitness_, int money_)
    : energy(energy_), health(health_), social(social_), academic(academic_), fitness(fitness_), money(money_)
{
}

// adjustEnergy()
// Adjusts the current energy level by the given delta (can be positive or negative).
// Input: delta to add to energy. Output: energy field updated, may exceed [0,100] until clampStats() is called.
void Player::adjustEnergy(int delta) { energy += delta; }

// adjustHealth()
// Adjusts the current health level by the given delta (can be positive or negative).
// Input: delta to add to health. Output: health field updated, may exceed [0,100] until clampStats() is called.
void Player::adjustHealth(int delta) { health += delta; }

// adjustSocial()
// Adjusts the current social stat by the given delta (can be positive or negative).
// Input: delta to add to social. Output: social field updated, may exceed [0,100] until clampStats() is called.
void Player::adjustSocial(int delta) { social += delta; }

// adjustAcademic()
// Adjusts the current academic stat by the given delta (can be positive or negative).
// Input: delta to add to academic. Output: academic field updated, may exceed [0,100] until clampStats() is called.
void Player::adjustAcademic(int delta) { academic += delta; }

// adjustFitness()
// Adjusts the current fitness stat by the given delta (can be positive or negative).
// Input: delta to add to fitness. Output: fitness field updated, may exceed [0,100] until clampStats() is called.
void Player::adjustFitness(int delta) { fitness += delta; }

// adjustMoney()
// Adjusts the current money balance by the given delta (can be positive or negative).
// Input: delta to add to money. Output: money field updated; may be negative until clampStats() is called.
void Player::adjustMoney(int delta) { money += delta; }

// clampStats()
// Ensures that all stats stay within their valid ranges (0-100) and money is non-negative.
// Input: none. Output: this Player's fields mutated to fit their clamped ranges.
void Player::clampStats()
{
    energy = std::max(0, std::min(100, energy));
    health = std::max(0, std::min(100, health));
    social = std::max(0, std::min(100, social));
    academic = std::max(0, std::min(100, academic));
    fitness = std::max(0, std::min(100, fitness));
    if (money < 0) money = 0;
}

// printStats()
// Outputs the player's current stats and simple progress bars to the console.
// Input: none. Output: human-readable stats printed using std::cout.
void Player::printStats() const
{
    auto barFor = [](int value) {
        int clamped = std::max(0, std::min(100, value));
        int filled = clamped / 10; // 0-10 segments
        std::string bar(10, '-');
        for (int i = 0; i < filled; ++i) {
            bar[i] = '#';
        }
        return bar;
    };

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
    std::cout << "==== CURRENT STATS ====\n";
    std::cout << "Energy   [" << barFor(energy)   << "] " << energy   << "\n";
    std::cout << "Health   [" << barFor(health)   << "] " << health   << "\n";
    std::cout << "Social   [" << barFor(social)   << "] " << social   << "\n";
    std::cout << "Academic [" << barFor(academic) << "] " << academic << "\n";
    std::cout << "Fitness  [" << barFor(fitness)  << "] " << fitness  << "\n";
    std::cout << "Money    [$" << money << "]\n";
    std::cout << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

}
