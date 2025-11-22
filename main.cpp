#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <chrono>

#include "activities.h"
#include "events.h"
#include "player.h"
#include "relationships.h"
#include "savegame.h"

struct Effect {
    int energy = 0;
    int health = 0;
    int social = 0;
    int academic = 0;
    int fitness = 0;
    int money = 0;
    std::map<std::string, int> affinityDeltas;

    Effect(int energy_ = 0, int health_ = 0, int social_ = 0, int academic_ = 0,
           int fitness_ = 0, int money_ = 0)
        : energy(energy_), health(health_), social(social_),
          academic(academic_), fitness(fitness_), money(money_)
    {
    }
};

struct GameState {
    int currentWeek = 1;
    bool skipNextScenario = false;
    bool visitedGym = false;
    bool metSam = false;
    bool metRiley = false;
    bool employed = false;
    std::string relationshipPath = "open"; // open, partner, single, drama, avoidance
    std::string partnerName;
    bool gameOver = false;
};

struct Choice {
    std::string title;
    std::string detail;
    Effect effect;
    std::function<void(GameState&, Player&, Relationships&)> special;
    std::function<bool(const GameState&, const Player&, const Relationships&)> available;
    std::string lockReason;

    Choice(const std::string& title_ = std::string(),
           const std::string& detail_ = std::string(),
           const Effect& effect_ = Effect(),
           std::function<void(GameState&, Player&, Relationships&)> special_ = nullptr,
           std::function<bool(const GameState&, const Player&, const Relationships&)> available_ = nullptr,
           const std::string& lockReason_ = std::string())
        : title(title_), detail(detail_), effect(effect_),
          special(special_), available(available_), lockReason(lockReason_)
    {
    }
};

struct Scenario {
    int week;
    int index; // 1..4 per week
    std::string location;
    std::string prompt;
    std::vector<Choice> choices;
};

struct WeekDecay {
    int energy = 0;
    int health = 0;
    int social = 0;
    int academic = 0;
    int fitness = 0;
};

struct DifficultySettings {
    std::string name;
    std::string description;
    double energyCostMultiplier = 1.0;
    double decayMultiplier = 1.0;
    int weeklyAllowance = 0;
    bool allowanceNeedsGrades = false;
    int weeklyDeduction = 0;
};

// rng()
// Returns a shared random number generator used for all probabilistic rolls.
// Input: none. Output: reference to std::mt19937 engine.
std::mt19937& rng()
{
    static std::mt19937 gen(std::random_device{}());
    return gen;
}

// roll()
// Rolls a random chance in [0,1) and compares against the given probability.
// Input: probability in [0.0, 1.0]. Output: true if the event occurs.
bool roll(double probability)
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng()) < probability;
}

// promptInt()
// Reads an integer from stdin between [min, max], re-prompting on invalid input.
// Inputs: min/max accepted values. Output: validated integer choice.
int promptInt(int min, int max)
{
    int value;
    while (true) {
        std::cout << "> ";
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Please enter a number between " << min << " and " << max << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// waitForEnter()
// Waits for the user to press Enter to continue.
// Input/Output: uses stdin, no return value.
void waitForEnter()
{
    std::cout << "(Press Enter to continue...)\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// applyEffect()
// Applies a simple stat/affinity Effect to the player and relationships, with difficulty-based energy scaling.
// Inputs: player and relationships to modify, effect description, difficulty settings.
// Output: modifies Player and Relationships in place.
void applyEffect(Player& player, Relationships& rels, const Effect& effect, const DifficultySettings& diff)
{
    int energyDelta = effect.energy;
    if (energyDelta < 0) {
        energyDelta = static_cast<int>(std::lround(energyDelta * diff.energyCostMultiplier));
    }
    player.adjustEnergy(energyDelta);
    player.adjustHealth(effect.health);
    player.adjustSocial(effect.social);
    player.adjustAcademic(effect.academic);
    player.adjustFitness(effect.fitness);
    player.adjustMoney(effect.money);

    for (const auto& entry : effect.affinityDeltas) {
        rels.interactWith(entry.first, entry.second);
    }
}

// enforceCriticalRules()
// Enforces global health/energy/money rules (hospital visit, collapse, game over).
// Inputs: current GameState and Player. Output: may set gameOver/skipNextScenario and adjust stats.
void enforceCriticalRules(GameState& state, Player& player)
{
    if (player.money <= 0) {
        std::cout << "[CRITICAL] FINANCIAL CRISIS! You ran out of money. GAME OVER.\n";
        state.gameOver = true;
        return;
    }

    if (player.health <= 10) {
        std::cout << "[WARNING] MEDICAL EMERGENCY! Forced hospital visit (-$100, +30 health, +20 energy).\n";
        player.adjustMoney(-100);
        player.adjustHealth(30);
        player.adjustEnergy(20);
    }

    if (player.energy <= 10) {
        std::cout << "[WARNING] COLLAPSE FROM EXHAUSTION! Forced sleep, skipping next scenario.\n";
        player.adjustEnergy(40);
        state.skipNextScenario = true;
    }

    player.clampStats();
}

// printRelationships()
// Prints the current affinity values for all known NPCs.
// Input: Relationships container. Output: formatted text to stdout.
void printRelationships(const Relationships& rels)
{
    std::cout << "==== RELATIONSHIPS ====\n";
    for (const auto& npc : rels.npcs) {
        std::cout << " - " << npc.name << ": " << npc.affinity << "\n";
    }
}

// chooseDifficulty()
// Prompts the user to select a difficulty and initializes starting stats accordingly.
// Input: Player to overwrite with starting values, output difficultyIndex and DifficultySettings.
DifficultySettings chooseDifficulty(Player& player, int& difficultyIndex)
{
    std::cout << "Choose your difficulty:\n";
    std::cout << "1. Trust Fund Kid (Easy) - Life on easy mode, just don't screw up\n";
    std::cout << "2. Average Student (Medium) - Typical college student struggles\n";
    std::cout << "3. Struggling Immigrant (Hard) - Provide for family, higher costs\n";

    int choice = promptInt(1, 3);
    difficultyIndex = choice;
    DifficultySettings settings;

    switch (choice) {
    case 1:
        player = Player(80, 75, 70, 40, 65, 1500);
        settings = {"Easy", "Lower decay and cheaper actions", 0.85, 0.8, 50, false, 0};
        break;
    case 2:
        player = Player(60, 65, 50, 50, 45, 300);
        settings = {"Medium", "Standard campus grind", 1.0, 1.0, 30, true, 0};
        break;
    default:
        player = Player(40, 60, 30, 60, 20, 100);
        settings = {"Hard", "Higher decay and weekly remittance", 1.1, 1.25, 0, false, 50};
        break;
    }

    std::cout << "Starting stats set for " << settings.name << " mode.\n";
    player.printStats();
    waitForEnter();
    return settings;
}

// difficultyFromIndex()
// Returns DifficultySettings corresponding to a stored difficulty index (1=Easy, 2=Medium, 3=Hard).
// Input: difficulty index. Output: DifficultySettings for that mode.
DifficultySettings difficultyFromIndex(int index)
{
    switch (index) {
    case 1:
        return {"Easy", "Lower decay and cheaper actions", 0.85, 0.8, 50, false, 0};
    case 2:
        return {"Medium", "Standard campus grind", 1.0, 1.0, 30, true, 0};
    default:
        return {"Hard", "Higher decay and weekly remittance", 1.1, 1.25, 0, false, 50};
    }
}

// buildScenarios()
// Constructs the full list of scripted scenarios (week, time-slot, choices) for the summer.
// Input: none. Output: vector of Scenario definitions.
std::vector<Scenario> buildScenarios()
{
    std::vector<Scenario> s;

    // Week 1
    s.push_back({1, 1, "Dorm Room",
                 "Summer break begins! Your alarm blares at 8 AM. You choose to:",
                 {
                     {"Sleep in", "Energy +40, Academic -3", {40, 0, 0, -3, 0, 0}, nullptr},
                     {"Go for a run", "Energy -25, Fitness +8, Health +5", {-25, 5, 0, 0, 8, 0}, nullptr},
                     {"Plan your summer", "Energy -5, All stats +2", {-5, 2, 2, 2, 2, 0}, nullptr},
                 }});

    s.push_back({1, 2, "Hall",
                 "Everyone seems to be done with exams. Where to first?",
                 {
                     {"Check out the gym", "Energy -20, Fitness +5, meet Alex", {-20, 0, 0, 0, 5, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.visitedGym = true;
                          rels.interactWith("Alex (Gym Crush)", 5);
                      }},
                     {"Visit the library", "Energy -10, Academic +12, meet Sam", {-10, 0, 0, 12, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.metSam = true;
                          rels.interactWith("Sam (Study Buddy)", 3);
                      }},
                     {"Hang in the common area", "Energy -10, Social +8, Money -5, meet Riley", {-10, 0, 8, 0, 0, -5},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.metRiley = true;
                          rels.interactWith("Riley (Barista)", 4);
                      }},
                 }});

    s.push_back({1, 3, "Cafeteria",
                 "Lunch time. Your stomach rumbles.",
                 {
                     {"Salad bar", "Money -12, Health +15 (Riley discount if met)", {0, 15, 0, 0, 0, -12},
                      [](GameState& st, Player& p, Relationships& rels) {
                          if (st.metRiley) {
                              p.adjustMoney(4); // discount from -12 to -8
                              rels.interactWith("Riley (Barista)", 3);
                          } else {
                              st.metRiley = true;
                              rels.interactWith("Riley (Barista)", 5);
                          }
                      }},
                     {"Pizza slice", "Money -6, Health +3 (Social -2 if never hit the gym)", {0, 3, 0, 0, 0, -6},
                      [](GameState& st, Player& p, Relationships&) {
                          if (!st.visitedGym) {
                              p.adjustSocial(-2);
                          }
                      }},
                     {"Skip lunch", "Health -5, save money", {0, -5, 0, 0, 0, 0}, nullptr},
                 }});

    s.push_back({1, 4, "Dorm Room",
                 "Evening approaches. How to wind down?",
                 {
                     {"Video games", "Energy -10, Social +3 (online), Academic -2", {-10, 0, 3, -2, 0, 0}, nullptr},
                     {"Study for summer course", "Energy -25, Academic +15", {-25, 0, 0, 15, 0, 0}, nullptr},
                     {"Early bedtime", "Energy +35", {35, 0, 0, 0, 0, 0}, nullptr},
                 }});

    // Week 2
    s.push_back({2, 1, "Dorm Room",
                 "Monday again. The summer sun is bright. You wake up feeling...",
                 {
                     {"Hit the gym early", "Energy -25, Fitness +8, Money -2, Alex appreciates the spot", {-25, 0, 0, 0, 8, -2},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.visitedGym = true;
                          rels.interactWith("Alex (Gym Crush)", 8);
                      }},
                     {"Review summer course materials", "Energy -20, Academic +12, help Sam with the printer", {-20, 0, 0, 12, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.metSam = true;
                          rels.interactWith("Sam (Study Buddy)", 7);
                      }},
                     {"Grab coffee and socialize", "Energy -10, Social +8, Money -5, Riley banter", {-10, 0, 8, 0, 0, -5},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.metRiley = true;
                          rels.interactWith("Riley (Barista)", 9);
                      }},
                 }});

    s.push_back({2, 2, "Various",
                 "Midday energy slump hits. Time to decide your afternoon focus.",
                 {
                     {"Intense workout session", "Energy -20, Fitness +10, Health +2, Alex notices you", {-20, 2, 0, 0, 10, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.visitedGym = true;
                          rels.interactWith("Alex (Gym Crush)", 10);
                      }},
                     {"Library deep dive", "Energy -25, Academic +15 (crowded risk)", {-25, 0, 0, 15, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          if (roll(0.25)) {
                              std::cout << "The library is packed. Progress slows.\n";
                              p.adjustAcademic(-7);
                          }
                          st.metSam = true;
                      }},
                     {"Take a nap", "Energy +35 (may doomscroll)", {35, 0, 0, 0, 0, 0},
                      [](GameState&, Player& p, Relationships&) {
                          if (roll(0.35)) {
                              std::cout << "YOU DOOMSCROLLED INSTEAD OF SLEEPING!!!.\n";
                              p.adjustEnergy(-15);
                          }
                      }},
                 }});

    s.push_back({2, 3, "Various",
                 "Late afternoon. The day is winding down but there's still time for...",
                 {
                     {"Evening gym session", "Energy -20, Fitness +6, Health +3", {-20, 3, 0, 0, 6, 0}, nullptr},
                     {"Quick study review", "Energy -15, Academic +8, Sam nods if present", {-15, 0, 0, 8, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          if (st.metSam) {
                              rels.interactWith("Sam (Study Buddy)", 2);
                          }
                      }},
                     {"Wind down socially", "Energy -10, Social +6, Money -8, time with Riley", {-10, 0, 6, 0, 0, -8},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.metRiley = true;
                          rels.interactWith("Riley (Barista)", 7);
                          if (roll(0.3)) {
                              std::cout << "You join alumni for drinks.\n";
                              p.adjustMoney(-20);
                              p.adjustSocial(14);
                              p.adjustHealth(-3);
                          }
                      }},
                 }});

    s.push_back({2, 4, "Dorm Room",
                 "Evening settles in. Your phone shows various notifications. Time to...",
                 {
                     {"Gaming session", "Energy -5, Social +5 (online), Academic -3, Jordan queue", {-5, 0, 5, -3, 0, 0},
                      [](GameState&, Player& p, Relationships& rels) {
                          rels.interactWith("Jordan (Gamer)", 8);
                          if (roll(0.2)) {
                              std::cout << "Ultra-rare drop! Small energy boost.\n";
                              p.adjustEnergy(5);
                          }
                      }},
                     {"Light studying", "Energy -10, Academic +10 (party risk)", {-10, 0, 0, 10, 0, 0},
                      [](GameState&, Player& p, Relationships&) {
                          if (roll(0.2)) {
                              std::cout << "Neighbors are loud. You lose focus.\n";
                              p.adjustAcademic(-5);
                              p.adjustEnergy(-5);
                          }
                      }},
                     {"Early rest", "Energy +30, Health +5", {30, 5, 0, 0, 0, 0},
                      [](GameState&, Player& p, Relationships&) {
                          if (roll(0.3)) {
                              std::cout << "Perfect sleep bonus.\n";
                              p.adjustEnergy(10);
                          p.adjustHealth(3);
                      }
                  }},
             }});

    // Week 3
    s.push_back({3, 1, "Dorm Room",
                 "Wednesday morning. You're finding a rhythm. Time to...",
                 {
                     {"Gym workout", "Energy -25, Fitness +8, hiking invite from Alex", {-25, 0, 0, 0, 8, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.visitedGym = true;
                          rels.interactWith("Alex (Gym Crush)", 6);
                          if (roll(0.5)) {
                              std::cout << "You accept a hiking invite. Extra fitness, slight fatigue.\n";
                              p.adjustEnergy(-15);
                              p.adjustFitness(5);
                              rels.interactWith("Alex (Gym Crush)", 6);
                          }
                      }},
                     {"Library session", "Energy -20, Academic +12, more time with Sam", {-20, 0, 0, 12, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.metSam = true;
                          if (p.academic > 70) {
                              rels.interactWith("Sam (Study Buddy)", 5);
                          } else {
                              rels.interactWith("Sam (Study Buddy)", 2);
                          }
                      }},
                     {"Look for part-time work", "Energy -15, chance to secure a job", {-15, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          if (roll(0.55)) {
                              std::cout << "You land a campus job. Weekly pay secured.\n";
                              st.employed = true;
                              p.adjustMoney(25); // signing bonus
                          } else {
                              std::cout << "No offers yet.\n";
                          }
                      }},
                 }});

    s.push_back({3, 2, "Various",
                 "Midday approaches. Your energy is holding up. Focus on...",
                 {
                     {"Serious training", "Energy -30, Fitness +10, Health +2", {-30, 2, 0, 0, 10, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.visitedGym = true;
                          rels.interactWith("Alex (Gym Crush)", 8);
                          if (roll(0.2)) {
                              std::cout << "Minor strain from overdoing it.\n";
                              p.adjustFitness(-8);
                              p.adjustHealth(-5);
                              p.adjustEnergy(-10);
                          }
                      }},
                     {"Academic push", "Energy -25, Academic +15 with Sam", {-25, 0, 0, 15, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.metSam = true;
                          rels.interactWith("Sam (Study Buddy)", 4);
                      }},
                     {"Networking lunch", "Energy -10, Social +12, Money -15, time with Riley", {-10, 0, 12, 0, 0, -15},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.metRiley = true;
                          rels.interactWith("Riley (Barista)", 5);
                          if (roll(0.4)) {
                              std::cout << "You impress the manager. Part-time offer secured.\n";
                              st.employed = true;
                              p.adjustMoney(30);
                          }
                      }},
                 }});

    s.push_back({3, 3, "Various",
                 "Late afternoon. The summer heat is peaking. Time for...",
                 {
                     {"Cool gym workout", "Energy -20, Fitness +6", {-20, 0, 0, 0, 6, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.visitedGym = true;
                          rels.interactWith("Alex (Gym Crush)", 6);
                          if (roll(0.25)) {
                              std::cout << "AC breaks! Workout is brutal.\n";
                              p.adjustEnergy(-15);
                              p.adjustHealth(-3);
                              p.adjustFitness(-3);
                          }
                      }},
                     {"Focused study", "Energy -20, Academic +10, calm space with Sam", {-20, 0, 0, 10, 0, 0},
                      [](GameState&, Player&, Relationships& rels) {
                          if (roll(0.5)) {
                              rels.interactWith("Sam (Study Buddy)", 7);
                          }
                      }},
                     {"Pool time", "Energy -15, Fitness +2, Money -8, social fun", {-15, 0, 5, 0, 2, -8},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.metRiley = true;
                          rels.interactWith("Riley (Barista)", 10);
                          if (roll(0.35)) {
                              std::cout << "Perfect summer afternoon at the pool.\n";
                              p.adjustSocial(8);
                              p.adjustHealth(5);
                              p.adjustFitness(3);
                          }
                      }},
                 }});

    s.push_back({3, 4, "Dorm Room",
                 "Evening brings relief from the heat. Choose your wind down.",
                 {
                     {"Online gaming", "Energy -10, Social +6 (online), Academic -2", {-10, 0, 6, -2, 0, 0},
                      [](GameState&, Player&, Relationships& rels) { rels.interactWith("Jordan (Gamer)", 9); }},
                     {"Evening study", "Energy -20, Academic +12", {-20, 0, 0, 12, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          if (st.metSam) {
                              rels.interactWith("Sam (Study Buddy)", 5);
                          }
                      }},
                     {"Rest and plan", "Energy +25, All stats +1", {25, 1, 1, 1, 1, 0}, nullptr},
                 }});

    // Week 4
    s.push_back({4, 1, "Dorm Room",
                 "Monday morning. Pressure mounts. Time to...",
                 {
                     {"Gym - push through fatigue", "Energy -30, Fitness +10", {-30, 0, 0, 0, 10, 0},
                      [](GameState&, Player& p, Relationships& rels) {
                          if (rels.npcs.size() && rels.npcs[0].affinity > 25) {
                              std::cout << "Alex advises caution. You ease up slightly.\n";
                              p.adjustEnergy(5);
                              rels.interactWith("Alex (Gym Crush)", 4);
                          }
                      }},
                     {"Library - midterm prep", "Energy -25, Academic +15", {-25, 0, 0, 15, 0, 0}, nullptr},
                     {"Side hustle search", "Energy -20, Money opportunity", {-20, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          if (roll(0.4)) {
                              std::cout << "Found a paying gig for the week.\n";
                              st.employed = true;
                              p.adjustMoney(50);
                          }
                      }},
                 }});

    s.push_back({4, 2, "Various",
                 "Midweek energy crash. The grind is real.",
                 {
                     {"Forced workout", "Energy -35, Fitness +12, Health -5", {-35, -5, 0, 0, 12, 0},
                      [](GameState&, Player& p, Relationships& rels) {
                          rels.interactWith("Alex (Gym Crush)", 4);
                          if (roll(0.2)) {
                              std::cout << "Major injury! Medical bill hits.\n";
                              p.adjustHealth(-25);
                              p.adjustFitness(-15);
                              p.adjustEnergy(-30);
                              p.adjustMoney(-140);
                          }
                      }},
                     {"Academic emergency", "Energy -30, Academic +18, Social -5", {-30, 0, -5, 18, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.metSam = true;
                          rels.interactWith("Sam (Study Buddy)", 5);
                      }},
                     {"Money desperation", "Energy -25, Money +50, All other stats -3", {-25, -3, -3, -3, -3, 50},
                      [](GameState&, Player& p, Relationships&) {
                          if (roll(0.5)) {
                              std::cout << "Sketchy gig backfires. You get robbed.\n";
                              p.adjustMoney(-120);
                          }
                      }},
                 }});

    s.push_back({4, 3, "Various",
                 "The week is breaking you. One more push before weekend...",
                 {
                     {"Final workout", "Energy -25, Fitness +2 (or loss if low fitness)", {-25, 0, 0, 0, 2, 0},
                      [](GameState&, Player& p, Relationships&) {
                          if (p.fitness < 50) {
                              std::cout << "Overtrained. You backslide.\n";
                              p.adjustFitness(-8);
                              p.adjustHealth(-10);
                          }
                      }},
                     {"Last-minute studying", "Energy -25, Academic +12", {-25, 0, 0, 12, 0, 0},
                      [](GameState&, Player& p, Relationships&) {
                          if (p.academic < 50) {
                              std::cout << "Burnout hits. Progress slips.\n";
                              p.adjustAcademic(-10);
                          }
                          if (roll(0.35)) {
                              std::cout << "Mental burnout.\n";
                              p.adjustAcademic(-10);
                              p.adjustEnergy(-20);
                              p.adjustHealth(-5);
                          }
                      }},
                     {"Social recovery", "Energy -15, Social +10, Money -12", {-15, 0, 10, 0, 0, -12},
                      [](GameState& st, Player&, Relationships& rels) {
                          if (st.metRiley) {
                              rels.interactWith("Riley (Barista)", 10);
                          }
                      }},
                 }});

    s.push_back({4, 4, "Dorm Room",
                 "Friday night. You're drained. Choose recovery.",
                 {
                     {"Complete collapse", "Energy +40, Health +10", {40, 10, 0, 0, 0, 0},
                      [](GameState&, Player& p, Relationships&) {
                          if (p.health < 40) {
                              p.adjustHealth(15);
                              p.adjustEnergy(20);
                          }
                      }},
                     {"Weekend prep", "Energy -20, All stats +3", {-20, 3, 3, 3, 3, 0}, nullptr},
                     {"Emergency measures", "Money -30, Energy +25, Health +15", {25, 15, 0, 0, 0, -30}, nullptr},
                 }});

    // Week 5
    s.push_back({5, 1, "Dorm Room",
                 "Monday morning. The pressure lingers, but there's hope.",
                 {
                     {"Recovery workout", "Energy -15, Fitness +6, Health +3", {-15, 3, 0, 0, 6, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.visitedGym = true;
                          rels.interactWith("Alex (Gym Crush)", 4);
                      }},
                     {"Study planning", "Energy -10, Academic +8", {-10, 0, 0, 8, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.metSam = true;
                          rels.interactWith("Sam (Study Buddy)", 4);
                      }},
                     {"Financial planning", "Energy -10, Money +20 (savings)", {-10, 0, 0, 0, 0, 20},
                      [](GameState&, Player& p, Relationships&) {
                          if (p.money < 100) {
                              p.adjustMoney(30);
                          }
                      }},
                 }});

    s.push_back({5, 2, "Various",
                 "Midweek balance. Time to address your weakest area.",
                 {
                     {"Fitness focus", "Energy -20, Fitness +10", {-20, 0, 0, 0, 10, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.visitedGym = true;
                          if (p.fitness < 50) {
                              p.adjustFitness(5);
                          }
                          if (roll(0.2)) {
                              std::cout << "Old injury flares up.\n";
                              p.adjustFitness(-10);
                              p.adjustHealth(-8);
                              p.adjustEnergy(-15);
                          }
                          rels.interactWith("Alex (Gym Crush)", 6);
                      }},
                     {"Academic catch-up", "Energy -25, Academic +15", {-25, 0, 0, 15, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.metSam = true;
                          rels.interactWith("Sam (Study Buddy)", 5);
                      }},
                     {"Social rebuilding", "Energy -15, Social +12, Money -10", {-15, 0, 12, 0, 0, -10},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.metRiley = true;
                          rels.interactWith("Riley (Barista)", 8);
                      }},
                 }});

    s.push_back({5, 3, "Various",
                 "Late afternoon. The summer rhythm feels more natural.",
                 {
                     {"Sustainable workout", "Energy -15, Fitness +7, Health +2", {-15, 2, 0, 0, 7, 0}, nullptr},
                     {"Effective studying", "Energy -18, Academic +13", {-18, 0, 0, 13, 0, 0}, nullptr},
                     {"Meaningful socializing", "Energy -12, Social +10, Money -8", {-12, 0, 10, 0, 0, -8},
                      [](GameState& st, Player&, Relationships& rels) {
                          if (st.metRiley) {
                              rels.interactWith("Riley (Barista)", 10);
                          }
                      }},
                 }});

    s.push_back({5, 4, "Dorm Room",
                 "Friday evening. You've found better balance this week.",
                 {
                     {"Restorative activities", "Energy +25, Health +8", {25, 8, 0, 0, 0, 0},
                      [](GameState&, Player& p, Relationships&) {
                          if (p.health < 60) {
                              p.adjustHealth(12);
                              p.adjustEnergy(15);
                          }
                      }},
                     {"Weekend preparation", "Energy -15, All stats +4", {-15, 4, 4, 4, 4, 0}, nullptr},
                     {"Stock investing", "Energy -10, Social +8, Money swing", {-10, 0, 8, 0, 0, -5},
                      [](GameState&, Player& p, Relationships&) {
                          if (roll(0.2)) {
                              p.adjustMoney(200);
                          } else if (roll(0.5)) {
                              p.adjustMoney(-50);
                          } else {
                              p.adjustMoney(20);
                          }
                      }},
                 }});

    // Week 6
    s.push_back({6, 1, "Dorm Room",
                 "Emotions are high. Afternoon decisions feel heavier.",
                 {
                     {"Fitness competition with Alex", "Energy -25, Fitness +12, show off gains", {-25, 0, 0, 0, 12, 0},
                      [](GameState&, Player&, Relationships& rels) { rels.interactWith("Alex (Gym Crush)", 12); }},
                     {"Hackathon with Sam", "Energy -22, Academic +15, brain battle", {-22, 0, 0, 15, 0, 0},
                      [](GameState&, Player&, Relationships& rels) { rels.interactWith("Sam (Study Buddy)", 12); }},
                     {"Double date situation", "Energy -18, Social +15, Money -15", {-18, 0, 15, 0, 0, -15},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.relationshipPath = "drama";
                          rels.interactWith("Alex (Gym Crush)", 2);
                          rels.interactWith("Sam (Study Buddy)", 2);
                          rels.interactWith("Riley (Barista)", 2);
                      }},
                 }});

    s.push_back({6, 2, "Dorm Room",
                 "Friday night. Make relationship decisions before the weekend.",
                 {
                     {"Confront the love triangle", "Energy -20, choose a partner or settle drama", {-20, 0, 0, 0, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          std::cout << "Who do you commit to? (1 Alex, 2 Sam, 3 Riley, 4 Stay open)\n";
                          int c = promptInt(1, 4);
                          if (c == 1) {
                              st.relationshipPath = "partner";
                              st.partnerName = "Alex (Gym Crush)";
                              rels.interactWith("Alex (Gym Crush)", 25);
                              rels.interactWith("Sam (Study Buddy)", -30);
                              rels.interactWith("Riley (Barista)", -30);
                          } else if (c == 2) {
                              st.relationshipPath = "partner";
                              st.partnerName = "Sam (Study Buddy)";
                              rels.interactWith("Sam (Study Buddy)", 25);
                              rels.interactWith("Alex (Gym Crush)", -30);
                              rels.interactWith("Riley (Barista)", -30);
                          } else if (c == 3) {
                              st.relationshipPath = "partner";
                              st.partnerName = "Riley (Barista)";
                              rels.interactWith("Riley (Barista)", 25);
                              rels.interactWith("Alex (Gym Crush)", -30);
                              rels.interactWith("Sam (Study Buddy)", -30);
                          } else {
                              st.relationshipPath = "single";
                          }
                      }},
                     {"Avoid everyone and game", "Energy -10, Social +5 (online)", {-10, 0, 5, 0, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.relationshipPath = "avoidance";
                          rels.interactWith("Jordan (Gamer)", 10);
                      }},
                 }});

    s.push_back({6, 3, "Various",
                 "Late week follow-up on relationships.",
                 {
                     {"Talk things through with everyone", "Energy -20, Social +10", {-20, 0, 10, 0, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.relationshipPath = "drama";
                          rels.interactWith("Alex (Gym Crush)", -10);
                          rels.interactWith("Sam (Study Buddy)", -10);
                          rels.interactWith("Riley (Barista)", -10);
                      }},
                     {"Lean into your choice", "Energy -10, Social +8", {-10, 0, 8, 0, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          if (!st.partnerName.empty()) {
                              rels.interactWith(st.partnerName, 10);
                          }
                      }},
                     {"Quiet weekend planning", "Energy +20, Health +5", {20, 5, 0, 0, 0, 0},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "avoidance"; }},
                 }});

    s.push_back({6, 4, "Dorm Room",
                 "End of the dramatic week.",
                 {
                     {"Reflect and rest", "Energy +25, Health +5", {25, 5, 0, 0, 0, 0}, nullptr},
                     {"Go out with friends", "Energy -15, Social +12, Money -15", {-15, 0, 12, 0, 0, -15},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.relationshipPath = st.relationshipPath == "open" ? "single" : st.relationshipPath;
                          rels.interactWith("Riley (Barista)", 6);
                      }},
                     {"Deep talk with Jordan", "Energy -5, Social +6", {-5, 0, 6, 0, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.relationshipPath = st.relationshipPath == "open" ? st.relationshipPath : "avoidance";
                          rels.interactWith("Jordan (Gamer)", 12);
                      }},
                 }});

    // Week 7
    s.push_back({7, 1, "Dorm Room",
                 "Monday morning aftermath.",
                 {
                     {"Spend time with your partner", "Energy -20, Affinity +15, Social +10", {-20, 0, 10, 0, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          if (!st.partnerName.empty()) {
                              rels.interactWith(st.partnerName, 15);
                              st.relationshipPath = "partner";
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "No partner chosen"},
                     {"Enjoy single life", "Energy -15, Social +12, Money -10", {-15, 0, 12, 0, 0, -10},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "single"; },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath != "partner"; },
                      "Locked to non-partner paths"},
                     {"Deal with campus gossip", "Energy -25, Social -5", {-25, 0, -5, 0, 0, 0},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "drama"; }},
                     {"Recharge alone", "Energy +25, Health +10", {25, 10, 0, 0, 0, 0},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "avoidance"; }},
                 }});

    s.push_back({7, 2, "Campus Cafe",
                 "Attempted polyamory conversation.",
                 {
                     {"Propose polyamory", "Energy -25, risky conversation", {-25, 0, 0, 0, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          if (roll(0.2)) {
                              std::cout << "Surprisingly, it works.\n";
                              rels.interactWith("Alex (Gym Crush)", 20);
                              rels.interactWith("Sam (Study Buddy)", 20);
                              rels.interactWith("Riley (Barista)", 20);
                              st.relationshipPath = "partner";
                          } else {
                              std::cout << "It goes poorly.\n";
                              rels.interactWith("Alex (Gym Crush)", -15);
                              rels.interactWith("Sam (Study Buddy)", -15);
                              rels.interactWith("Riley (Barista)", -15);
                              st.relationshipPath = "drama";
                          }
                      }},
                     {"Juggle conversations", "Energy -20, Social +10", {-20, 0, 10, 0, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          rels.interactWith("Alex (Gym Crush)", 5);
                          rels.interactWith("Sam (Study Buddy)", 5);
                          rels.interactWith("Riley (Barista)", 5);
                          st.relationshipPath = "drama";
                      }},
                     {"Back out awkwardly", "Energy +15, Social -5", {15, 0, -5, 0, 0, 0},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "avoidance"; }},
                 }});

    s.push_back({7, 3, "Various",
                 "Afternoon choices reflect your trajectory.",
                 {
                     {"Couple workout/study session", "Fitness +8 or Academic +10, Affinity +12, Energy -20", {-20, 0, 0, 10, 8, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          if (!st.partnerName.empty()) {
                              rels.interactWith(st.partnerName, 12);
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "Partner path only"},
                     {"Party hopping", "Social +15, Money -30, Energy -20", {-20, 0, 15, 0, 0, -30},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "single"; },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath != "partner"; },
                      "Single/drama paths only"},
                     {"Damage control with friends", "Social +10, Energy -15", {-15, 0, 10, 0, 0, 0},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "drama"; }},
                     {"Solo meditation", "Health +12, Energy +25", {25, 12, 0, 0, 0, 0},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "avoidance"; }},
                 }});

    s.push_back({7, 4, "Dorm Room",
                 "Friday night reflection.",
                 {
                     {"Deep talk with partner", "Affinity +20, Social +10", {0, 0, 10, 0, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          if (!st.partnerName.empty()) {
                              rels.interactWith(st.partnerName, 20);
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "Partner path only"},
                     {"Casual gaming with Jordan", "Social +8, Energy -10", {-10, 0, 8, 0, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.relationshipPath = st.relationshipPath == "partner" ? st.relationshipPath : "single";
                          rels.interactWith("Jordan (Gamer)", 10);
                      }},
                     {"Distract yourself with work", "Academic +12, Energy -20", {-20, 0, 0, 12, 0, 0}, nullptr},
                     {"Early sleep", "Energy +30, Health +10", {30, 10, 0, 0, 0, 0}, nullptr},
                 }});

    // Week 8
    s.push_back({8, 1, "Library",
                 "Final exams week. Focus depends on your path.",
                 {
                     {"Study with partner", "Academic +20, Affinity +10, Energy -25", {-25, 0, 0, 20, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          if (!st.partnerName.empty()) {
                              rels.interactWith(st.partnerName, 10);
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "Partner path only"},
                     {"Solo cram session", "Academic +25, Energy -30", {-30, 0, 0, 25, 0, 0},
                      [](GameState& st, Player&, Relationships&) {
                          if (st.relationshipPath == "open") st.relationshipPath = "avoidance";
                      }},
                     {"Distracted by drama", "Academic +10, Social -5, Energy -20", {-20, 0, -5, 10, 0, 0},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "drama"; }},
                 }});

    s.push_back({8, 2, "Campus Quad",
                 "Big end-of-summer party.",
                 {
                     {"Attend with partner", "Social +20, Affinity +15, Money -40", {0, 0, 20, 0, 0, -40},
                      [](GameState& st, Player&, Relationships& rels) {
                          if (!st.partnerName.empty()) {
                              rels.interactWith(st.partnerName, 15);
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "Partner path only"},
                     {"Go wild single", "Social +25, Money -50, Energy -20", {-20, 0, 25, 0, 0, -50},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "single"; },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath != "partner"; },
                      "Single/drama paths only"},
                     {"Show up despite drama", "Social +10, Money -20", {0, 0, 10, 0, 0, -20},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "drama"; }},
                     {"Skip party", "Energy +30, Social -10", {30, 0, -10, 0, 0, 0},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "avoidance"; }},
                 }});

    s.push_back({8, 3, "Dorm Room",
                 "Final weekend. Time to reflect on your summer.",
                 {
                     {"Future planning with partner", "All stats +5, Affinity +10", {5, 5, 5, 5, 5, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          if (!st.partnerName.empty()) {
                              rels.interactWith(st.partnerName, 10);
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "Partner path only"},
                     {"Set solo goals", "All stats +8, Energy -15", {-15, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          p.adjustHealth(8);
                          p.adjustSocial(8);
                          p.adjustAcademic(8);
                          p.adjustFitness(8);
                          st.relationshipPath = st.relationshipPath == "partner" ? st.relationshipPath : "single";
                      }},
                     {"Repair drama damage", "Social +10, Energy -20", {-20, 0, 10, 0, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.relationshipPath = "drama";
                          rels.interactWith("Alex (Gym Crush)", 5);
                          rels.interactWith("Sam (Study Buddy)", 5);
                          rels.interactWith("Riley (Barista)", 5);
                      }},
                     {"Rest deeply", "Energy +40, Health +15", {40, 15, 0, 0, 0, 0},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "avoidance"; }},
                 }});

    s.push_back({8, 4, "Campus Quad",
                 "Sunday evening. Summer ends.",
                 {
                     {"Farewell with partner", "Affinity +20, Social +10", {0, 0, 10, 0, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          if (!st.partnerName.empty()) {
                              rels.interactWith(st.partnerName, 20);
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "Partner path only"},
                     {"Farewell with friends", "Social +15, Money -30", {0, 0, 15, 0, 0, -30},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "single"; }},
                     {"Farewell awkwardly", "Social -5, Energy +10", {10, 0, -5, 0, 0, 0},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "drama"; }},
                     {"Farewell quietly", "Energy +20, Social -5", {20, 0, -5, 0, 0, 0},
                      [](GameState& st, Player&, Relationships&) { st.relationshipPath = "avoidance"; }},
                 }});

    return s;
}

// buildWeekDecay()
// Returns per-week stat decay values (before difficulty scaling) for Energy/Health/Social/Academic/Fitness.
// Input: none. Output: vector indexed by week number.
std::vector<WeekDecay> buildWeekDecay()
{
    return {
        {}, // index 0 unused
        {0, -3, -4, -5, -6},
        {-8, -4, -5, -6, -7},
        {-9, -5, -6, -7, -8},
        {-12, -8, -7, -9, -8},
        {-8, -4, -5, -6, -5},
        {-20, -6, -8, -7, -6},
        {-12, -8, -10, -10, -9},
        {-20, -12, -15, -15, -15},
    };
}

// applyWeekEnd()
// Applies weekly allowance/deductions, stat decay, and flavor effects at the end of each week.
// Inputs: current week index, GameState, Player, Relationships, and DifficultySettings.
// Output: modifies Player and Relationships; may clear skipNextScenario.
void applyWeekEnd(int week, GameState& state, Player& player, Relationships& rels, const DifficultySettings& diff)
{
    // Small pause before the end-of-week summary block.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "\n[WEEK END] --- End of Week " << week << " ---\n";

    if (diff.weeklyAllowance > 0 && (!diff.allowanceNeedsGrades || player.academic > 60)) {
        player.adjustMoney(diff.weeklyAllowance);
        std::cout << "[MONEY] Allowance received: $" << diff.weeklyAllowance << "\n";
    }
    if (diff.weeklyDeduction > 0) {
        player.adjustMoney(-diff.weeklyDeduction);
        std::cout << "[MONEY] Sent money home: -$" << diff.weeklyDeduction << "\n";
    }
    if (state.employed) {
        player.adjustMoney(100);
        std::cout << "[MONEY] Paycheck from part-time work: +$100\n";
    }

    auto decayTable = buildWeekDecay();
    if (week < static_cast<int>(decayTable.size())) {
        WeekDecay d = decayTable[week];
        player.adjustEnergy(static_cast<int>(std::lround(d.energy * diff.decayMultiplier)));
        player.adjustHealth(static_cast<int>(std::lround(d.health * diff.decayMultiplier)));
        player.adjustSocial(static_cast<int>(std::lround(d.social * diff.decayMultiplier)));
        player.adjustAcademic(static_cast<int>(std::lround(d.academic * diff.decayMultiplier)));
        player.adjustFitness(static_cast<int>(std::lround(d.fitness * diff.decayMultiplier)));
        std::cout << "Stat decay applied for the week.\n";
    }

    if (week == 1) {
        if (player.academic > 40) {
            player.adjustMoney(30);
            std::cout << "[FAMILY] Parents proud of academics: +$30\n";
        } else if (player.academic < 20) {
            player.adjustSocial(-10);
            std::cout << "[FAMILY] Parents concerned about grades: Social -10\n";
        } else {
            player.adjustMoney(30);
            std::cout << "[FAMILY] Standard allowance: +$30\n";
        }
    }

    // Week-specific story events based on the design/game flow.
    if (week == 2) {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        std::cout << "\n========== SPECIAL EVENT ==========\n";
        std::cout << "      UNEXPECTED CAMPUS-WIDE EVENT!\n";
        std::cout << "===================================\n";
        double r = static_cast<double>(rand()) / RAND_MAX;
        if (r < 0.5) {
            std::cout << "Free campus movie night on the quad! Social +8.\n";
            player.adjustSocial(8);
        } else {
            std::cout << "Campus construction starts nearby. Noise and distractions all week.\n";
            player.adjustEnergy(-5);
            player.adjustAcademic(-2);
            player.adjustHealth(-2);
        }
    } else if (week == 3) {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        std::cout << "\n========== SPECIAL EVENT ==========\n";
        std::cout << "        GPA RESULTS CAME OUT!\n";
        std::cout << "===================================\n";
        double r = static_cast<double>(rand()) / RAND_MAX;
        if (r < 0.35) {
            std::cout << "STELLAR GRADES WOOHOO! All stats +3, Energy +10.\n";
            player.adjustEnergy(10);
            player.adjustHealth(3);
            player.adjustSocial(3);
            player.adjustAcademic(3);
            player.adjustFitness(3);
        } else if (r < 0.70) {
            std::cout << "You got a D and a C. Confidence shaken.\n";
            player.adjustEnergy(-5);
            player.adjustSocial(-2);
            player.adjustAcademic(-5);
        } else {
            std::cout << "Results were alright. You feel okay about it.\n";
        }
    } else if (week == 4) {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        std::cout << "\n========== SPECIAL EVENT ==========\n";
        std::cout << "        HEALTH CRISIS DECLARED!\n";
        std::cout << "===================================\n";
        double r = static_cast<double>(rand()) / RAND_MAX;
        if (r < 0.40) {
            std::cout << "SUMMER FLU EPIDEMIC HITS! Health -20, Energy -25.\n";
            player.adjustHealth(-20);
            player.adjustEnergy(-25);
        } else if (r < 0.70) {
            std::cout << "CAMPUS WELLNESS PROGRAM LAUNCHES! Health +15, Energy +20.\n";
            player.adjustHealth(15);
            player.adjustEnergy(20);
        } else {
            std::cout << "HEAT WAVE CONTINUES. You feel drained.\n";
            player.adjustEnergy(-10);
            player.adjustHealth(-5);
        }
    }

    if (week == 6 && state.relationshipPath == "partner" && !state.partnerName.empty()) {
        rels.interactWith(state.partnerName, 15);
        std::cout << "[RELATIONSHIP] Relationship solidifies with " << state.partnerName << ".\n";
    }

    // Additional weekly random events from the legacy event pool.
    auto weeklyEvents = Events::generateWeeklyEvents();
    for (const auto& e : weeklyEvents) {
        if (Events::rollEvent(e)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            std::cout << "\n********** RANDOM EVENT **********\n";
            std::cout << "[EVENT] " << e.name << " - " << e.description << "\n";
            std::cout << "**********************************\n";
            e.apply(player);
        }
    }

    player.clampStats();
    player.printStats();
    printRelationships(rels);
    state.skipNextScenario = false;
}

// starsFromStats()
// Derives a 0–5 "star" rating from final stats, with 0 meaning critical failure.
// Input: final Player stats. Output: integer rating in [0,5].
int starsFromStats(const Player& p)
{
    if (p.health <= 0 || p.energy <= 0 || p.money <= 0) return 0;
    int good = 0;
    good += p.health >= 80;
    good += p.energy >= 80;
    good += p.social >= 80;
    good += p.academic >= 80;
    good += p.fitness >= 80;
    good += p.money >= 2000;

    if (good >= 5) return 5;
    if (good >= 4) return 4;
    if (good >= 3) return 3;
    if (good >= 2) return 2;
    return 1;
}

// runGame()
// Main game loop: handles difficulty/load selection, iterates weeks/scenarios, and prints the final ending.
// Input: none. Output: runs the interactive session and prints results.
void runGame()
{
    Player player;
    Relationships rels;
    GameState state;

    std::cout << "WELCOME TO SUMMER MAXXING!\n";
    std::cout << "Your goal: Survive 8 weeks while balancing Energy, Health,\n";
    std::cout << "Social, Academic, Fitness, and Money while building relationships.\n";

    // Offer to load an existing save (player stats + week) before starting a new run.
    bool loaded = false;
    DifficultySettings diff;
    int difficultyIndex = 2;
    std::cout << "Load existing save from savegame.txt? (y/n)\n";
    char loadChoice = 'n';
    if (std::cin >> loadChoice) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (loadChoice == 'y' || loadChoice == 'Y') {
            int savedWeek = 1;
            int savedScenario = 0;
            if (SaveGame::load(player, savedWeek, savedScenario)) {
                std::cout << "Loaded save at week " << savedWeek << ".\n";
                state.currentWeek = savedWeek;
                difficultyIndex = savedScenario;
                if (difficultyIndex < 1 || difficultyIndex > 3) {
                    difficultyIndex = 2; // default to Medium if invalid
                }
                diff = difficultyFromIndex(difficultyIndex);
                loaded = true;
            } else {
                std::cout << "No valid save found. Starting a new game.\n";
            }
        }
    } else {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    if (!loaded) {
        diff = chooseDifficulty(player, difficultyIndex);
        std::cout << "TIP: Don't let any stat sit low for too long.\n";
        std::cout << "Work, rest and socialize in balance, and watch your Energy and Money.\n";
    }

    auto scenarios = buildScenarios();
    const int TOTAL_WEEKS = 8;

    for (int week = state.currentWeek; week <= TOTAL_WEEKS && !state.gameOver; ++week) {
        state.currentWeek = week;
        std::cout << "\n====================================\n";
        std::cout << "              WEEK " << week << "\n";
        std::cout << "====================================\n";
        for (int slot = 1; slot <= 4 && !state.gameOver; ++slot) {
            if (state.skipNextScenario) {
                std::cout << "You recover this scenario due to exhaustion.\n";
                state.skipNextScenario = false;
                continue;
            }

            auto it = std::find_if(scenarios.begin(), scenarios.end(), [&](const Scenario& sc) {
                return sc.week == week && sc.index == slot;
            });
            if (it == scenarios.end()) continue;
            const Scenario& sc = *it;

            // Show current stats and relationships before each decision.
            player.printStats();
            printRelationships(rels);

            // Brief pause before showing the scenario text block.
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            std::cout << "\n------------------------------------\n";
            std::cout << "Scenario " << slot << " - " << sc.location << "\n";
            std::cout << "------------------------------------\n";
            std::cout << sc.prompt << "\n";

            std::vector<int> availableIndices;
            for (size_t i = 0; i < sc.choices.size(); ++i) {
                const auto& c = sc.choices[i];
                bool ok = !c.available || c.available(state, player, rels);
                if (ok) {
                    availableIndices.push_back(static_cast<int>(i));
                    std::cout << availableIndices.size() << ". " << c.title << " (" << c.detail << ")\n";
                } else {
                    std::cout << "- " << c.title << " [locked: " << c.lockReason << "]\n";
                }
            }

            if (availableIndices.empty()) {
                std::cout << "No available actions. You rest instead.\n";
                player.adjustEnergy(15);
                continue;
            }

            int selection = promptInt(1, static_cast<int>(availableIndices.size()));
            const Choice& chosen = sc.choices[availableIndices[selection - 1]];
            applyEffect(player, rels, chosen.effect, diff);
            if (chosen.special) {
                chosen.special(state, player, rels);
            }

            enforceCriticalRules(state, player);
            player.clampStats();
        }

        if (!state.gameOver) {
            applyWeekEnd(week, state, player, rels, diff);

            // Simple save option at the end of each week.
            std::cout << "Save and quit? (y/n)\n";
            char saveChoice = 'n';
            if (std::cin >> saveChoice) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (saveChoice == 'y' || saveChoice == 'Y') {
                    if (SaveGame::save(player, week, difficultyIndex)) {
                        std::cout << "Game saved to savegame.txt.\n";
                    } else {
                        std::cout << "Failed to save game.\n";
                    }
                    return;
                }
            } else {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    std::cout << "\nCLOSING SEQUENCE\n";
    if (state.gameOver) {
        std::cout << "You could not survive the summer. Better luck next time.\n";
        return;
    }

    std::cout << "FINAL REPORT:\n";
    player.printStats();
    printRelationships(rels);

    int stars = starsFromStats(player);
    if (stars == 0) {
        std::cout << "GAME OVER due to critical failures.\n";
        return;
    }

    std::cout << "Rating: " << stars << "/5 ";
    if (stars == 5) std::cout << "Legendary Summer!\n";
    else if (stars == 4) std::cout << "Great Summer!\n";
    else if (stars == 3) std::cout << "Average Summer.\n";
    else if (stars == 2) std::cout << "Rough Summer.\n";
    else std::cout << "Disaster Summer.\n";
}

int main()
{
    runGame();
    return 0;
}
