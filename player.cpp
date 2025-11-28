#include "player.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

// Constructor for the Player class, initializes the player's stats with the given values.
Player::Player(int energy_, int health_, int social_, int academic_, int fitness_, int money_)
    : energy(energy_), health(health_), social(social_), academic(academic_), fitness(fitness_), money(money_)
{
}

// Adjusts the player's energy by the given delta value.
// Input: delta (int) - the amount to modify energy by.
// Output: Updates the `energy` member variable.
void Player::adjustEnergy(int delta) { energy += delta; }

// Adjusts the player's health by the given delta value.
// Input: delta (int) - the amount to modify health by.
// Output: Updates the `health` member variable.
void Player::adjustHealth(int delta) { health += delta; }

// Adjusts the player's social stat by the given delta value.
// Input: delta (int) - the amount to modify social by.
// Output: Updates the `social` member variable.
void Player::adjustSocial(int delta) { social += delta; }

// Adjusts the player's academic stat by the given delta value.
// Input: delta (int) - the amount to modify academic by.
// Output: Updates the `academic` member variable.
void Player::adjustAcademic(int delta) { academic += delta; }

// Adjusts the player's fitness stat by the given delta value.
// Input: delta (int) - the amount to modify fitness by.
// Output: Updates the `fitness` member variable.
void Player::adjustFitness(int delta) { fitness += delta; }

// Adjusts the player's money by the given delta value.
// Input: delta (int) - the amount to modify money by.
// Output: Updates the `money` member variable.
void Player::adjustMoney(int delta) { money += delta; }

// Ensures that all stats are clamped within their valid ranges.
// Energy, health, social, academic, and fitness are clamped between 0 and 100.
// Money is clamped to a minimum of 0 (no negative money).
// Input: None.
// Output: Updates the player's stats to ensure they remain within valid bounds.
void Player::clampStats()
{
    energy = std::max(0, std::min(100, energy));       // Clamp energy to [0, 100]
    health = std::max(0, std::min(100, health));       // Clamp health to [0, 100]
    social = std::max(0, std::min(100, social));       // Clamp social to [0, 100]
    academic = std::max(0, std::min(100, academic));   // Clamp academic to [0, 100]
    fitness = std::max(0, std::min(100, fitness));     // Clamp fitness to [0, 100]
    if (money < 0) money = 0;                          // Ensure money is not negative
}

// Prints the player's current stats in a formatted way.
// Displays each stat as a bar (10 segments) and the corresponding numeric value.
// Money is displayed as a dollar amount.
// Input: None.
// Output: Prints the player's stats to the console.
void Player::printStats() const
{
    // Lambda function to generate a visual representation of a stat as a bar.
    // Input: value (int) - the stat value to represent.
    // Output: A string representing the stat as a bar (e.g., "#####-----").
    auto barFor = [](int value) {
        int clamped = std::max(0, std::min(100, value)); // Clamp the value to [0, 100]
        int filled = clamped / 10;                      // Determine the number of filled segments (0-10)
        std::string bar(10, '-');                       // Initialize the bar with 10 empty segments
        for (int i = 0; i < filled; ++i) {              // Fill the appropriate number of segments
            bar[i] = '#';
        }
        return bar;                                     // Return the bar as a string
    };

    // Simulate a delay for a better user experience
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Print the formatted stats
    std::cout << std::endl;
    std::cout << "==== CURRENT STATS ====\n";
    std::cout << "Energy   [" << barFor(energy)   << "] " << energy   << "\n";  // Print energy bar and value
    std::cout << "Health   [" << barFor(health)   << "] " << health   << "\n";  // Print health bar and value
    std::cout << "Social   [" << barFor(social)   << "] " << social   << "\n";  // Print social bar and value
    std::cout << "Academic [" << barFor(academic) << "] " << academic << "\n";  // Print academic bar and value
    std::cout << "Fitness  [" << barFor(fitness)  << "] " << fitness  << "\n";  // Print fitness bar and value
    std::cout << "Money    [$" << money << "]\n";                                // Print money value
    std::cout << std::endl;

    // Simulate another delay for better user experience
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}
