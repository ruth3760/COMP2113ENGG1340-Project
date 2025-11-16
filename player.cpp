#include "player.h"
#include <algorithm>
#include <iostream>

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
    std::cout << "[Stats] Energy:" << energy
              << " Health:" << health
              << " Social:" << social
              << " Academic:" << academic
              << " Fitness:" << fitness
              << " Money:$" << money << "\n";
}
