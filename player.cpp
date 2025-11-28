#include "player.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>


Player::Player(int energy_, int health_, int social_, int academic_, int fitness_, int money_)
    : energy(energy_), health(health_), social(social_), academic(academic_), fitness(fitness_), money(money_)
{
}

void Player::adjustEnergy(int delta) { energy += delta; }
void Player::adjustHealth(int delta) { health += delta; }
void Player::adjustSocial(int delta) { social += delta; }
void Player::adjustAcademic(int delta) { academic += delta; }
void Player::adjustFitness(int delta) { fitness += delta; }
void Player::adjustMoney(int delta) { money += delta; }

void Player::clampStats()
{
    energy = std::max(0, std::min(100, energy));
    health = std::max(0, std::min(100, health));
    social = std::max(0, std::min(100, social));
    academic = std::max(0, std::min(100, academic));
    fitness = std::max(0, std::min(100, fitness));
    if (money < 0) money = 0;
}

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

