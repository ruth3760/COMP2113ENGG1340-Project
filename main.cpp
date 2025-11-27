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
    // Week 1 random-event flags
    bool week1FriendEventDone = false;
    bool week1RouterEventDone = false;
    bool week1EveningEventDone = false;
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
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
    std::cout << "[Press any key to continue...]\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// printStatChanges()
// Prints how each stat and relationship affinity changed after a choice, in a compact before -> after format.
// Input: Player and Relationships state before and after an action.
void printStatChanges(const Player& beforePlayer,
                      const Player& afterPlayer,
                      const Relationships& beforeRels,
                      const Relationships& afterRels)
{
    std::cout << "\n--- RESULTING STAT CHANGES ---\n";
    auto line = [](const char* label, int beforeVal, int afterVal) {
        if (beforeVal == afterVal) return;
        int delta = afterVal - beforeVal;
        std::cout << label << ": " << beforeVal << " -> " << afterVal
                  << " (" << (delta > 0 ? "+" : "") << delta << ")\n";
    };

    line("Energy",   beforePlayer.energy,   afterPlayer.energy);
    line("Health",   beforePlayer.health,   afterPlayer.health);
    line("Social",   beforePlayer.social,   afterPlayer.social);
    line("Academic", beforePlayer.academic, afterPlayer.academic);
    line("Fitness",  beforePlayer.fitness,  afterPlayer.fitness);
    line("Money",    beforePlayer.money,    afterPlayer.money);

    // Relationship affinity changes
    bool anyAffinityChange = false;
    for (const auto& npcAfter : afterRels.npcs) {
        int beforeAffinity = 0;
        bool found = false;
        for (const auto& npcBefore : beforeRels.npcs) {
            if (npcBefore.name == npcAfter.name) {
                beforeAffinity = npcBefore.affinity;
                found = true;
                break;
            }
        }
        if (!found) {
            // New relationship created this step.
            beforeAffinity = 0;
        }
        if (beforeAffinity == npcAfter.affinity) continue;
        if (!anyAffinityChange) {
            std::cout << "\n--- RELATIONSHIP CHANGES ---\n";
            anyAffinityChange = true;
        }
        int delta = npcAfter.affinity - beforeAffinity;
        std::cout << npcAfter.name << ": " << beforeAffinity << " -> " << npcAfter.affinity
                  << " (" << (delta > 0 ? "+" : "") << delta << ")\n";
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
                 "Summer break begins! Your alarm blares at 8 AM cus you forgot to change them from all the exam stress. You choose to:",
                 {
                     {"Sleep in", "Energy +40, Academic -3", {40, 0, 0, -3, 0, 0},
                      [](GameState&, Player&, Relationships&) {
                          std::cout << "You hit snooze and roll over, letting yourself drift back to sleep.\n";
                          std::cout << "The stress of exams slowly fades as you catch up on much-needed rest.\n";
                      }},
                     {"Go for a run", "Energy -25, Fitness +8, Health +5", {-25, 5, 0, 0, 8, 0},
                      [](GameState&, Player&, Relationships&) {
                          std::cout << "You lace up your shoes and jog through the quiet campus.\n";
                          std::cout << "The cool morning air and empty paths make the run feel refreshing.\n";
                      }},
                     {"Plan your summer", "Energy -5, All stats +2", {-5, 2, 2, 2, 2, 0},
                      [](GameState&, Player&, Relationships&) {
                          std::cout << "You sit at your desk with a notebook and calendar.\n";
                          std::cout << "Listing goals for health, academics, finances, and relationships makes the summer feel full of possibility.\n";
                      }},
                 }});

    s.push_back({1, 2, "Hall",
                 "Everyone seems to be done with exams, so things are pretty quiet. Where to first?",
                 {
                     {"Check out the gym", "Energy -20, Fitness +5, meet Alex", {-20, 0, 0, 0, 5, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          std::cout << "You enter the gym near your hall – it's surprisingly modern and well-equipped.\n";
                          std::cout << "Only a handful of dedicated students are here this early in summer.\n";
                          std::cout << "You do a light workout to test the equipment.\n";
                          std::cout << "While adjusting the bench press, you notice an incredibly jacked person doing deadlifts with perfect form.\n";
                          std::cout << "They catch you looking and give a small smile before returning to their set.\n";
                          std::cout << "This must be Alex; you've heard from friends about their insane physique.\n";
                          st.visitedGym = true;
                          rels.interactWith("Alex (Gym Crush)", 5);
                      }},
                     {"Visit the library", "Energy -10, Academic +12, meet Sam", {-10, 0, 0, 12, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          std::cout << "You push through the heavy library doors into a calm, studious hush.\n";
                          std::cout << "Summer session means only the most dedicated students are here.\n";
                          std::cout << "You find a sunlit study carrel and spend two hours previewing next semester's courses.\n";
                          std::cout << "Nearby, a student with incredibly organized notes works quickly through the same material.\n";
                          std::cout << "They look up briefly – Sam – adjust their glasses, then dive back into the textbook.\n";
                          std::cout << "You leave feeling a little more prepared for the academic challenges ahead.\n";
                          st.metSam = true;
                          rels.interactWith("Sam (Study Buddy)", 3);
                      }},
                     {"Hang in the common area", "Energy -10, Social +8, Money -5, meet Riley", {-10, 0, 8, 0, 0, -5},
                      [](GameState& st, Player&, Relationships& rels) {
                          std::cout << "You head to the hall common area, where people are sprawled across sofas with laptops and board games.\n";
                          std::cout << "You grab an iced coffee and sink into a comfortable armchair.\n";
                          std::cout << "Riley, an acquaintance from earlier semesters, is at the counter ordering coffee for everyone.\n";
                          std::cout << "Riley: \"Iced Americano again like usual, right?\" They flash you a knowing smile.\n";
                          std::cout << "Summer might be the perfect time to turn casual acquaintances into real friends.\n";
                          st.metRiley = true;
                          rels.interactWith("Riley (Barista)", 4);
                      }},
                 }});

    s.push_back({1, 3, "Cafeteria",
                 "Lunch time. Your stomach rumbles.",
                 {
                     {"Salad bar", "Money -12, Health +15 (Riley discount if met)", {0, 15, 0, 0, 0, -12},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "You load up a plate with fresh greens, grilled chicken, and colorful vegetables.\n";
                          std::cout << "It looks healthy but definitely costs more than the cheaper options.\n";
                          std::cout << "At the register, you notice Riley working the cashier this shift.\n";

                          if (st.metRiley) {
                              std::cout << "Riley grins: \"Going for the healthy option again? I'll give you the staff discount.\"\n";
                              p.adjustMoney(4); // discount from -12 to -8
                              rels.interactWith("Riley (Barista)", 3);
                          } else {
                              std::cout << "Riley makes friendly small talk while ringing you up and seems impressed by your choice.\n";
                              st.metRiley = true;
                              rels.interactWith("Riley (Barista)", 5);
                          }
                     }},
                     {"Pizza slice", "Money -6, Health +3 (Social -2 if never hit the gym)", {0, 3, 0, 0, 0, -6},
                      [](GameState& st, Player& p, Relationships&) {
                          std::cout << "You grab a quick pepperoni slice from the counter.\n";
                          std::cout << "It's greasy but satisfying, the classic comfort option.\n";
                          std::cout << "At the next table, two gym bros are talking a bit too loudly.\n";
                          std::cout << "GYM BRO 1: \"Can you believe some people actually eat that during summer cut?\"\n";
                          std::cout << "GYM BRO 2: \"I know, right? No discipline.\"\n";

                          if (!st.visitedGym) {
                              std::cout << "You suddenly feel self-conscious about your meal choice and wonder if you should eat cleaner.\n";
                              p.adjustSocial(-2);
                          } else {
                              std::cout << "One of them recognizes you from the gym and gives you an approving nod despite your food choice.\n";
                              std::cout << "You shrug it off with a laugh — it's just one slice.\n";
                          }
                      }},
                     {"Skip lunch", "Health -5, save money", {0, -5, 0, 0, 0, 0},
                      [](GameState&, Player&, Relationships&) {
                          std::cout << "You decide to skip a proper meal and just sip water from the fountain instead.\n";
                          std::cout << "Your wallet is grateful, but your stomach growls in protest as the afternoon wears on.\n";
                      }},
                 }});

    s.push_back({1, 4, "Dorm Room",
                 "Evening approaches. How to wind down?",
                 {
                     {"Video games", "Energy -10, Social +3 (online), Academic -2", {-10, 0, 3, -2, 0, 0},
                      [](GameState&, Player&, Relationships&) {
                          std::cout << "You boot up your favorite game and queue into a few matches.\n";
                          std::cout << "Voice chat fills with familiar banter as you and online friends unwind from exam season.\n";
                      }},
                     {"Study for summer course", "Energy -25, Academic +15", {-25, 0, 0, 15, 0, 0},
                      [](GameState&, Player&, Relationships&) {
                          std::cout << "You clear off your desk, open your summer course materials, and start reviewing.\n";
                          std::cout << "The quiet evening gives you a focused head start on the weeks ahead.\n";
                      }},
                     {"Early bedtime", "Energy +35", {35, 0, 0, 0, 0, 0},
                      [](GameState&, Player&, Relationships&) {
                          std::cout << "You put your phone away, close your laptop, and climb into bed early.\n";
                          std::cout << "As you drift off, you feel grateful for the chance to reset before summer truly begins.\n";
                      }},
                 }});

    // Week 2
    s.push_back({2, 1, "Dorm Room",
                 "Monday again. The summer sun is already bright. You wake up feeling...",
                 {
                     {"Hit the gym early", "Energy -25, Fitness +8, Money -2, Alex appreciates the spot", {-25, 0, 0, 0, 8, -2},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.visitedGym = true;
                          rels.interactWith("Alex (Gym Crush)", 8);
                          // Positive random event: free personal training session giveaway.
                          if (roll(0.30)) {
                              std::cout << "Gym manager announces a free personal training session giveaway... and you win!\n";
                              p.adjustFitness(10);
                          }
                      }},
                     {"Review summer course materials", "Energy -20, Academic +12, help Sam with the printer", {-20, 0, 0, 12, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          st.metSam = true;
                          rels.interactWith("Sam (Study Buddy)", 7);
                      }},
                     {"Grab coffee and socialize", "Energy -10, Social +8, Money -5, Riley banter", {-10, 0, 8, 0, 0, -5},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.metRiley = true;
                          rels.interactWith("Riley (Barista)", 9);
                          // Additional social comfort bonus from consistent morning ritual.
                          p.adjustSocial(3);
                      }},
                 }});

    s.push_back({2, 2, "Various",
                 "Midday energy slump hits. Time to decide your afternoon focus.",
                 {
                     {"Intense workout session", "Energy -20, Fitness +10, Health +2, Alex notices you", {-20, 2, 0, 0, 10, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.visitedGym = true;
                          rels.interactWith("Alex (Gym Crush)", 10);
                          // Small positive random event: free protein shaker giveaway.
                          if (roll(0.20)) {
                              std::cout << "Gym is giving out free protein shakers today. Nice little boost!\n";
                              p.adjustFitness(1);
                          }
                      }},
                     {"Library deep dive", "Energy -25, Academic +15 (crowded risk)", {-25, 0, 0, 15, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          // 50% chance the library is too crowded to study effectively.
                          if (roll(0.50)) {
                              std::cout << "The library is surprisingly crowded. You end up going home instead.\n";
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
                     {"Evening gym session", "Energy -20, Fitness +6, Health +3", {-20, 3, 0, 0, 6, 0},
                      [](GameState&, Player& p, Relationships&) {
                          // Positive random event: empty gym lets you experiment with new equipment.
                          if (roll(0.20)) {
                              std::cout << "Gym is unexpectedly empty — perfect time to try new equipment.\n";
                              p.adjustFitness(4);
                              p.adjustHealth(2);
                          }
                      }},
                     {"Quick study review", "Energy -15, Academic +8, Sam nods if present", {-15, 0, 0, 8, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          if (st.metSam) {
                              rels.interactWith("Sam (Study Buddy)", 2);
                          }
                          // Negative random event: brain fog from over-studying.
                          if (roll(0.25)) {
                              std::cout << "Brain fog sets in — you've been studying too long.\n";
                              p.adjustAcademic(-4); // roughly halves the gain
                              p.adjustEnergy(-8);
                          }
                      }},
                     {"Wind down socially", "Energy -10, Social +6, Money -8, time with Riley", {-10, 0, 6, 0, 0, -8},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.metRiley = true;
                          rels.interactWith("Riley (Barista)", 7);
                          if (roll(0.3)) {
                              std::cout << "You join alumni for drinks.\n";
                              p.adjustMoney(-20);
                              p.adjustSocial(14); // total +20 including base
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
                              p.adjustEnergy(-10);
                              p.adjustSocial(2);
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
                 "Wednesday morning. You're starting to find your summer rhythm. Time to...",
                 {
                     {"Gym workout", "Energy -25, Fitness +8", {-25, 0, 0, 0, 8, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "\n[LOCATION: Gym]\n";
                          std::cout << "Alex is working on pull-ups when you arrive. They drop down and walk over.\n\n";
                          std::cout << "ALEX: \"Hey, I was hoping you'd come. Some of us are doing a weekend hiking trip - thought you might be interested?\"\n\n";
                          std::cout << "How do you respond?\n";
                          std::cout << "  1) \"Definitely! When?\"\n";
                          std::cout << "  2) \"Maybe another time.\"\n";
                          std::cout << "  3) \"Not really my thing.\"\n";
                          int sub = promptInt(1, 3);

                          st.visitedGym = true;
                          if (sub == 1) {
                              rels.interactWith("Alex (Gym Crush)", 12);
                              p.adjustEnergy(-15);
                              p.adjustFitness(5);
                              std::cout << "You lock in the hiking trip with Alex and their friends. It'll be a tough but rewarding weekend.\n";
                          } else if (sub == 2) {
                              rels.interactWith("Alex (Gym Crush)", 3);
                              p.adjustFitness(6);
                              std::cout << "You thank Alex for the invite but leave it open for next time. They seem a little disappointed, but understanding.\n";
                          } else {
                              rels.interactWith("Alex (Gym Crush)", -5);
                              std::cout << "You admit it's not really your thing. Alex nods, but the energy between you cools a bit.\n";
                          }
                      }},
                     {"Library session", "Energy -20, Academic +12", {-20, 0, 0, 12, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "\n[LOCATION: Library]\n";
                          std::cout << "Sam is here today, and you grab a seat next to them in a crowded row of desks.\n";
                          if (!st.metSam) {
                              std::cout << "SAM: \"Oh, are you also taking this course next semester?\"\n";
                              std::cout << "You chat for a bit and end up having a surprisingly productive study session together.\n";
                              st.metSam = true;
                              rels.interactWith("Sam (Study Buddy)", 3);
                          } else {
                              std::cout << "SAM: \"I've been stuck on this question for ages. Any idea how to approach it?\"\n";
                              if (p.academic > 70) {
                                  std::cout << "You walk Sam through the solution. Their eyes light up as it finally clicks.\n";
                                  rels.interactWith("Sam (Study Buddy)", 5);
                              } else {
                                  std::cout << "You puzzle over it together, but neither of you quite crack it.\n";
                                  rels.interactWith("Sam (Study Buddy)", 2);
                              }
                          }

                          // RANDOM EVENT - Positive: library assistant job posting.
                          if (roll(0.30)) {
                              std::cout << "\n[RANDOM EVENT] You spot a flyer: \"Library Assistant Needed for Summer\".\n";
                              if (p.academic > 60) {
                                  std::cout << "Your grades qualify you. You apply on the spot and get a callback.\n";
                                  p.adjustMoney(45);
                                  p.adjustAcademic(3);
                              } else {
                                  std::cout << "The posting requires stronger academics than you currently have. Maybe later in the summer.\n";
                              }
                          }
                      }},
                     {"Look for part-time work", "Energy -15, Money opportunity", {-15, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          std::cout << "\n[LOCATION: Campus Job Board]\n";
                          std::cout << "You browse summer job postings pinned to a crowded corkboard.\n";
                          std::cout << "A few options catch your eye:\n";
                          std::cout << "  1) Campus cafe barista (needs Social > 40)\n";
                          std::cout << "  2) Library assistant (needs Academic > 60)\n";
                          std::cout << "  3) Gym attendant (needs Fitness > 50)\n";
                          int job = promptInt(1, 3);

                          if (job == 1) {
                              if (p.social > 40) {
                                  std::cout << "You apply for the barista job. The manager appreciates your people skills.\n";
                                  p.adjustMoney(12);
                                  p.adjustSocial(2);
                              } else {
                                  std::cout << "You realize your social skills might not be strong enough for constant customer interaction yet.\n";
                              }
                          } else if (job == 2) {
                              if (p.academic > 60) {
                                  std::cout << "You talk to the head librarian about the assistant position. They seem impressed by your grades.\n";
                                  p.adjustMoney(15);
                                  p.adjustAcademic(2);
                              } else {
                                  std::cout << "The library role demands academic excellence you haven't quite reached yet.\n";
                              }
                          } else {
                              if (p.fitness > 50) {
                                  std::cout << "You chat with the gym staff about helping out. Your fitness level makes you a solid candidate.\n";
                                  p.adjustMoney(10);
                                  p.adjustFitness(2);
                              } else {
                                  std::cout << "You decide to build up your fitness a bit more before applying for a gym role.\n";
                              }
                          }

                          // RANDOM EVENT - Positive: quick online survey for extra cash.
                          if (roll(0.35)) {
                              std::cout << "\n[RANDOM EVENT] You stumble on a quick paid online survey while job hunting.\n";
                              std::cout << "You fill it out on your phone and get a small payout.\n";
                              p.adjustMoney(25);
                          }

                          st.employed = st.employed || false; // placeholder to preserve existing flag if used elsewhere
                      }},
                 }});

    s.push_back({3, 2, "Various",
                 "Midday approaches. Your energy is holding up. Focus on...",
                 {
                     {"Serious training", "Energy -30, Fitness +10, Health +2", {-30, 2, 0, 0, 10, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "\n[LOCATION: Gym]\n";
                          std::cout << "Alex leads you through an intense circuit training session.\n";
                          std::cout << "Between sets, they cheer you on and correct your form.\n";
                          st.visitedGym = true;
                          rels.interactWith("Alex (Gym Crush)", 8);
                          // RANDOM EVENT - Negative: minor muscle strain.
                          if (roll(0.20)) {
                              std::cout << "\n[RANDOM EVENT] Minor muscle strain from overexertion!\n";
                              std::cout << "Maybe you should have warmed up better...\n";
                              p.adjustFitness(-15);
                              p.adjustHealth(-5);
                              p.adjustEnergy(-10);
                          }
                      }},
                     {"Academic push", "Energy -25, Academic +15", {-25, 0, 0, 15, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "\n[LOCATION: Library Study Room]\n";
                          std::cout << "Sam has reserved a study room for a focused afternoon session.\n";
                          std::cout << "You work through problem sets together, trading tips and mnemonics.\n";
                          st.metSam = true;
                          rels.interactWith("Sam (Study Buddy)", 4);

                          // RANDOM EVENT - Positive: one-off tutor needed.
                          if (roll(0.30)) {
                              std::cout << "\n[RANDOM EVENT] A notice pops up: \"One-off tutor needed for an upcoming assignment\".\n";
                              if (p.academic > 70) {
                                  std::cout << "Your grades qualify you. You take the gig and earn some extra cash.\n";
                                  p.adjustMoney(20);
                              } else {
                                  std::cout << "The tutor role requires higher grades than you currently have.\n";
                              }
                          }
                      }},
                     {"Networking lunch", "Energy -10, Social +12, Money -15", {-10, 0, 12, 0, 0, -15},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "\n[LOCATION: Campus Cafe]\n";
                          std::cout << "You join Riley during their lunch break, chatting over sandwiches and coffee.\n";
                          st.metRiley = true;

                          // Neutral random event: manager interview opportunity.
                          std::cout << "RILEY: \"This is the friend I was telling you about – always productive and reliable.\"\n";
                          std::cout << "MANAGER: \"We're looking for summer help. Interested in an interview?\"\n";

                          if (p.social > 60) {
                              std::cout << "You handle the impromptu interview smoothly and make a strong impression.\n";
                              rels.interactWith("Riley (Barista)", 5);
                              st.employed = true;
                              p.adjustMoney(10);
                          } else if (roll(0.30)) {
                              std::cout << "\n[RANDOM EVENT] You arrive late and stumble through your answers.\n";
                              std::cout << "Later, Riley tells you their manager questioned why they recommended you.\n";
                              rels.interactWith("Riley (Barista)", -15);
                              p.adjustSocial(-20);
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
                 "Monday morning. You're feeling the summer pressure mounting. Time to...",
                 {
                     {"Gym - push through fatigue", "Energy -30, Fitness +10", {-30, 0, 0, 0, 10, 0},
                      [](GameState&, Player& p, Relationships& rels) {
                          std::cout << "\n[LOCATION: Gym]\n";
                          if (!rels.npcs.empty() && rels.npcs[0].affinity > 25) {
                              std::cout << "Alex looks concerned when they see you walk in.\n";
                              std::cout << "ALEX: \"You look exhausted, man. Maybe take it easy today?\"\n\n";
                          } else {
                              std::cout << "You drag yourself into the gym, muscles heavy from the week so far.\n";
                          }

                          std::cout << "How do you respond?\n";
                          std::cout << "  1) Push harder\n";
                          std::cout << "  2) Listen to Alex\n";
                          std::cout << "  3) Leave and rest\n";
                          int sub = promptInt(1, 3);

                          if (sub == 1) {
                              // Push harder: go beyond the base effect.
                              p.adjustEnergy(-5);   // net Energy -35
                              p.adjustFitness(2);   // net Fitness +12
                              p.adjustHealth(-8);
                              // Week 4 random event: overtraining only if you push harder.
                              Events::week4GymOvertraining(p);
                          } else if (sub == 2) {
                              // Listen to Alex: lighter session plus affinity.
                              p.adjustEnergy(15);   // net Energy -15
                              p.adjustFitness(-5);  // net Fitness +5
                              p.adjustHealth(3);
                              rels.interactWith("Alex (Gym Crush)", 8);
                              std::cout << "You scale back the workout and focus on light movement and stretching.\n";
                          } else {
                              // Leave and rest: abandon the workout.
                              p.adjustEnergy(50);   // net Energy +20
                              p.adjustFitness(-13); // net Fitness -3
                              std::cout << "You decide today isn't the day and head back to your room to rest instead.\n";
                          }
                      }},
                     {"Library - midterm prep", "Energy -25, Academic +15", {-25, 0, 0, 15, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "\n[LOCATION: Library]\n";
                          std::cout << "Sam looks stressed but determined, surrounded by stacks of notes.\n";
                          if (rels.npcs.size() > 1 && rels.npcs[1].affinity > 20) {
                              std::cout << "SAM: \"Tomorrow here, same time, and grab lunch afterwards?\"\n\n";
                          }

                          std::cout << "Midterm prep options:\n";
                          std::cout << "  1) Study marathon\n";
                          std::cout << "  2) Quick session\n";
                          std::cout << "  3) Too busy\n";
                          int sub = promptInt(1, 3);

                          st.metSam = true;
                          if (sub == 1) {
                              // Study marathon: heavier push.
                              p.adjustEnergy(-15);   // net Energy -40
                              p.adjustAcademic(5);   // net Academic +20
                              rels.interactWith("Sam (Study Buddy)", 10);
                              std::cout << "You and Sam grind through a long study block, trading strategies and mnemonics.\n";
                              // Week 4 random event: study breakthrough.
                              Events::week4StudyBreakthrough(p);
                          } else if (sub == 2) {
                              // Quick session: moderate gains.
                              // Base already gave Energy -25, Academic +15, so just adjust to +12 & affinity.
                              p.adjustAcademic(-3);  // net Academic +12
                              rels.interactWith("Sam (Study Buddy)", 3);
                              std::cout << "You review the key topics together for a while, then call it a night.\n";
                              Events::week4StudyBreakthrough(p);
                          } else {
                              // Too busy: smaller gains, Sam affinity hit.
                              p.adjustEnergy(5);     // net Energy -20
                              p.adjustAcademic(-7);  // net Academic +8
                              rels.interactWith("Sam (Study Buddy)", -10);
                              std::cout << "You apologize and cut the session short to handle other obligations.\n";
                          }
                      }},
                     {"Side hustle search", "Energy -20, Money opportunity", {-20, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          std::cout << "\n[LOCATION: Campus Job Center]\n";
                          std::cout << "More urgent financial pressure this week. You scan listings for side hustles.\n";
                          std::cout << "Approach?\n";
                          std::cout << "  1) Apply for multiple jobs\n";
                          std::cout << "  2) Focus on one good opportunity\n";
                          std::cout << "  3) Online freelancing\n";
                          int sub = promptInt(1, 3);

                          double baseChance = 0.40;
                          double jobChance = baseChance;
                          if (sub == 1) {
                              jobChance += 0.30;
                              std::cout << "You fire off applications to every reasonable posting you can find.\n";
                          } else if (sub == 2) {
                              jobChance += 0.10;
                              std::cout << "You tailor a careful application to the single most promising role.\n";
                          } else {
                              std::cout << "You spend the afternoon doing small online tasks and polishing your profile.\n";
                          }

                          if (roll(jobChance)) {
                              std::cout << "One of the employers gets back to you with a paid opportunity.\n";
                              st.employed = true;
                              p.adjustMoney(50);
                          } else {
                              std::cout << "Despite your efforts, nothing concrete lands this week.\n";
                          }
                      }},
                 }});

      s.push_back({4, 2, "Various",
                   "Midweek energy crash. The summer grind is real...",
                   {
                       {"Forced workout", "Energy -35, Fitness +12, Health -5", {-35, -5, 0, 0, 12, 0},
                       [](GameState&, Player& p, Relationships& rels) {
                           std::cout << "\n[LOCATION: Gym]\n";
                           std::cout << "Your body screams in protest with every rep.\n";

                           if (!rels.npcs.empty() && rels.npcs[0].affinity > 30) {
                               std::cout << "ALEX: \"Seriously, you look awful. Let me buy you a protein shake and we can talk.\"\n\n";
                               std::cout << "How do you respond?\n";
                               std::cout << "  1) Accept help\n";
                               std::cout << "  2) Push through pain\n";
                               int sub = promptInt(1, 2);

                               if (sub == 1) {
                                   // Accept help: offset some of the base damage and boost affinity.
                                   p.adjustHealth(10);
                                   p.adjustEnergy(15);
                                   rels.interactWith("Alex (Gym Crush)", 8);
                                   std::cout << "You sit with Alex, sip a protein shake, and talk about training smarter.\n";
                               } else {
                                   // Push through pain: extra gains but more health loss.
                                   p.adjustFitness(8);
                                   p.adjustHealth(-12);
                                   std::cout << "You wave Alex off and grind through the set anyway. It feels \"worth it\"... maybe.\n";
                                   // Week 4 random event: major injury only if you ignore the warning.
                                   Events::week4ForcedWorkoutInjury(p);
                               }
                           } else {
                               rels.interactWith("Alex (Gym Crush)", 4);
                               // Without the deeper relationship, you just push yourself — with risk.
                               Events::week4ForcedWorkoutInjury(p);
                           }
                      }},
                     {"Academic emergency", "Energy -30, Academic +18, Social -5", {-30, 0, -5, 18, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "\n[LOCATION: Library]\n";
                          std::cout << "Sam looks like they haven't slept in days, dark circles under their eyes.\n";

                          if (rels.npcs.size() > 1 && rels.npcs[1].affinity > 25) {
                              std::cout << "Sam opens up about the academic pressure they're under and how worried they are about the future.\n";
                              std::cout << "You listen, then decide to...\n";
                              std::cout << "  1) Share your experience in a long heart-to-heart\n";
                              std::cout << "  2) Offer short but sweet words of affirmation\n";
                              std::cout << "  3) Brush it off with a \"sounds like a you problem lol\"\n";
                              int sub = promptInt(1, 3);

                              if (sub == 1) {
                                  p.adjustEnergy(-15); // extra time/effort beyond base
                                  p.adjustHealth(-10);
                                  rels.interactWith("Sam (Study Buddy)", 15);
                                  std::cout << "You share your own struggles and the two of you talk about uni and life for a long time.\n";
                              } else if (sub == 2) {
                                  rels.interactWith("Sam (Study Buddy)", 1);
                                  std::cout << "You offer a few kind words and encouragement before both of you return to studying.\n";
                              } else {
                                  rels.interactWith("Sam (Study Buddy)", -50);
                                  std::cout << "You dismiss their worries with a joke. Sam goes quiet, clearly hurt.\n";
                              }
                          } else {
                              // Default behavior when the relationship isn't deep enough yet.
                              st.metSam = true;
                              rels.interactWith("Sam (Study Buddy)", 5);
                          }
                      }},
                     {"Money desperation", "Energy -25, Money +50, All other stats -3", {-25, -3, -3, -3, -3, 50},
                     [](GameState&, Player& p, Relationships&) {
                          // Week 4 random event: sketchy gig robbery.
                          Events::week4MoneyDesperationRobbery(p);
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
                          // Week 4 random event: magic potion offer.
                          Events::week4FinalWorkoutPotion(p);
                      }},
                     {"Last-minute studying", "Energy -25, Academic +12", {-25, 0, 0, 12, 0, 0},
                     [](GameState&, Player& p, Relationships&) {
                          if (p.academic < 50) {
                              std::cout << "Burnout hits. Progress slips.\n";
                              p.adjustAcademic(-10);
                          }
                          // Week 4 random event: mental burnout.
                          Events::week4LastMinuteStudyBurnout(p);
                      }},
                     {"Social recovery", "Energy -15, Social +10, Money -12", {-15, 0, 10, 0, 0, -12},
                     [](GameState& st, Player& p, Relationships& rels) {
                          if (st.metRiley) {
                              rels.interactWith("Riley (Barista)", 10);
                          }
                          // Week 4 random event: unexpected campus party.
                          Events::week4SocialRecoveryParty(p);
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
                          // Week 4 random event: wake up sick anyway.
                          Events::week4CompleteCollapseSick(p);
                      }},
                     {"Weekend prep", "Energy -20, All stats +3", {-20, 3, 3, 3, 3, 0}, nullptr},
                     {"Emergency measures", "Money -30, Energy +25, Health +15", {25, 15, 0, 0, 0, -30},
                      [](GameState&, Player& p, Relationships&) {
                          // Week 4 random event: buyer's remorse.
                          Events::week4EmergencyMeasuresRemorse(p);
                      }},
                 }});

    // Week 5
    s.push_back({5, 1, "Dorm Room",
                 "Monday morning. The pressure from last week lingers, but there's hope ahead.",
                 {
                     {"Recovery workout", "Energy -15, Fitness +6, Health +3", {-15, 3, 0, 0, 6, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.visitedGym = true;
                          std::cout << "LOCATION: Gym\n";
                          std::cout << "Alex greets you with genuine concern.\n";
                          if (p.health < 40) {
                              std::cout << "ALEX: \"You look rough. Let's do a light active recovery day, okay?\"\n";
                          }
                          std::cout << "What do you do?\n";
                          std::cout << "  1) Listen to Alex (take it easy)\n";
                          std::cout << "  2) Push anyway (stubborn but effective)\n";
                          int c = promptInt(1, 2);
                          if (c == 1) {
                              // Listen to Alex: easier day, more sustainable recovery.
                              p.adjustEnergy(5);   // net a bit less energy spent
                              p.adjustFitness(-3); // lighter training than the base effect
                              p.adjustHealth(2);   // more healing focus
                              rels.interactWith("Alex (Gym Crush)", 8);
                              std::cout << "You follow Alex's advice and focus on light movement and stretching.\n";
                          } else {
                              // Push anyway: higher gains but more strain.
                              p.adjustEnergy(-10);
                              p.adjustFitness(2);
                              p.adjustHealth(-8);
                              std::cout << "You insist on pushing hard despite Alex's concern.\n";
                              std::cout << "The workout feels productive but your body definitely protests.\n";
                          }

                          // Extra relationship beat if affinity is already high.
                          int alexAffinity = 0;
                          for (const auto& npc : rels.npcs) {
                              if (npc.name.find("Alex (Gym Crush)") != std::string::npos) {
                                  alexAffinity = npc.affinity;
                                  break;
                              }
                          }
                          if (alexAffinity > 50) {
                              std::cout << "\nALEX: \"I've noticed you've been consistent. "
                                           "Want to be workout partners officially?\"\n";
                              std::cout << "  1) \"Of course, thought we already are haha.\"\n";
                              std::cout << "  2) \"No bruh, I have a workout partner already and they're way bigger than you.\"\n";
                              int c2 = promptInt(1, 2);
                              if (c2 == 1) {
                                  rels.interactWith("Alex (Gym Crush)", 8);
                                  std::cout << "Alex grins. \"Then it's official. Let's crush this summer.\"\n";
                              } else {
                                  rels.interactWith("Alex (Gym Crush)", -60);
                                  std::cout << "Alex's smile fades. \"Oh. Right. Got it.\"\n";
                              }
                          }
                      }},
                     {"Study planning", "Energy -10, Academic +8", {-10, 0, 0, 8, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.metSam = true;
                          std::cout << "LOCATION: Library\n";
                          std::cout << "Sam looks more organized than ever, surrounded by color-coded notes.\n";

                          if (p.academic < 50) {
                              std::cout << "SAM: \"I made a study schedule that might help us both. Want to see it?\"\n";
                              std::cout << "You go over their plan together.\n";
                              p.adjustAcademic(10);
                              rels.interactWith("Sam (Study Buddy)", 8);
                          }

                          std::cout << "How do you respond to Sam's planning?\n";
                          std::cout << "  1) Follow Sam's plan\n";
                          std::cout << "  2) \"I have a better plan but thanks anyways\"\n";
                          std::cout << "  3) Wing it\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustEnergy(-5);
                              p.adjustAcademic(4);
                              rels.interactWith("Sam (Study Buddy)", 8);
                              std::cout << "You commit to Sam's schedule. It feels structured and achievable.\n";
                          } else if (c == 2) {
                              rels.interactWith("Sam (Study Buddy)", -10);
                              std::cout << "You brush off Sam's plan, insisting you have your own approach.\n";
                          } else {
                              p.adjustEnergy(5);
                              p.adjustAcademic(-4);
                              std::cout << "You decide to wing it and hope for the best.\n";
                          }

                          // Week 5 random event: research study money opportunity.
                          Events::week5StudyPlanningResearchStudy(p);
                      }},
                     {"Financial planning", "Energy -10, Money +20 (savings)", {-10, 0, 0, 0, 0, 20},
                      [](GameState&, Player& p, Relationships&) {
                          std::cout << "LOCATION: Dorm Room\n";
                          std::cout << "You spread out your bank statements and budgeting app on your desk.\n";
                          if (p.money < 100) {
                              std::cout << "FINANCIAL WAKE-UP CALL - you urgently need a stable income.\n";
                          }

                          std::cout << "How do you respond to your finances?\n";
                          std::cout << "  1) Budget strictly\n";
                          std::cout << "  2) Find additional work\n";
                          std::cout << "  3) Cut expenses drastically\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustMoney(30);
                              std::cout << "You tighten your budget and find extra savings in small habits.\n";
                          } else if (c == 2) {
                              p.adjustEnergy(-30);
                              p.adjustHealth(-15);
                              p.adjustMoney(50);
                              std::cout << "You pick up additional shifts wherever you can. It's exhausting but pays.\n";
                          } else {
                              p.adjustMoney(50);
                              p.adjustHealth(-5);
                              p.adjustSocial(-15);
                              std::cout << "You slash your social and lifestyle spending. Life feels a bit bleak, but cheaper.\n";
                          }

                          // Week 5 random event: unexpected refund.
                          Events::week5FinancialPlanningRefund(p);
                      }},
                 }});

    s.push_back({5, 2, "Various",
                 "Midweek balance. Time to address your weakest area...",
                 {
                     {"Fitness focus", "Energy -20, Fitness +10", {-20, 0, 0, 0, 10, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.visitedGym = true;
                          std::cout << "LOCATION: Gym\n";
                          std::cout << "Your body feels more responsive today as you warm up.\n";

                          if (p.fitness < 50) {
                              std::cout << "Breaking through the plateau feels amazing!\n";
                              p.adjustFitness(5);
                          }

                          int alexAffinity = 0;
                          for (const auto& npc : rels.npcs) {
                              if (npc.name.find("Alex (Gym Crush)") != std::string::npos) {
                                  alexAffinity = npc.affinity;
                                  break;
                              }
                          }
                          if (alexAffinity > 40) {
                              std::cout << "ALEX: \"Want to try some advanced techniques I saw yesterday on Instagram?\"\n";
                              p.adjustFitness(8);
                              rels.interactWith("Alex (Gym Crush)", 6);
                          }

                          // Week 5 random event: old injury flares up.
                          Events::week5FitnessFocusOldInjury(p);
                      }},
                     {"Academic catch-up", "Energy -25, Academic +15", {-25, 0, 0, 15, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.metSam = true;
                          std::cout << "LOCATION: Library\n";

                          bool samHere = roll(0.60);
                          if (samHere) {
                              std::cout << "Sam has reserved a quiet study corner for the afternoon.\n";
                              p.adjustAcademic(10);
                          } else {
                              std::cout << "You settle into a quiet spot on your own, determined to catch up.\n";
                          }

                          int beforeAffinity = 0;
                          for (const auto& npc : rels.npcs) {
                              if (npc.name.find("Sam (Study Buddy)") != std::string::npos) {
                                  beforeAffinity = npc.affinity;
                                  break;
                              }
                          }

                          std::cout << "SAM: \"Wanna grab dinner together tonight?\"\n";
                          std::cout << "  1) \"Sure\"\n";
                          std::cout << "  2) \"Been waiting for you to ask me out\"\n";
                          std::cout << "  3) \"Sorry, busy tonight\"\n";
                          std::cout << "  4) \"No.\"\n";
                          int c = promptInt(1, 4);
                          if (c == 1) {
                              rels.interactWith("Sam (Study Buddy)", 8);
                          } else if (c == 2) {
                              if (beforeAffinity < 60) {
                                  rels.interactWith("Sam (Study Buddy)", -10);
                              } else {
                                  rels.interactWith("Sam (Study Buddy)", 1);
                              }
                          } else if (c == 3) {
                              rels.interactWith("Sam (Study Buddy)", -5);
                          } else {
                              rels.interactWith("Sam (Study Buddy)", -15);
                          }

                          // Week 5 random event: professor offers extra help.
                          Events::week5AcademicCatchupExtraHelp(p);
                      }},
                     {"Social rebuilding", "Energy -15, Social +12, Money -10", {-15, 0, 12, 0, 0, -10},
                      [](GameState& st, Player& p, Relationships& rels) {
                          st.metRiley = true;
                          std::cout << "LOCATION: Campus Social Spaces\n";
                          std::cout << "Riley spots you and walks over.\n";

                          if (p.social < 40) {
                              std::cout << "RILEY: \"Hey stranger! We've missed you. Everything okay?\"\n";
                              p.adjustSocial(10);
                              rels.interactWith("Riley (Barista)", 8);
                          }

                          int rileyAffinity = 0;
                          for (const auto& npc : rels.npcs) {
                              if (npc.name.find("Riley (Barista)") != std::string::npos) {
                                  rileyAffinity = npc.affinity;
                                  break;
                              }
                          }
                          if (rileyAffinity > 50) {
                              std::cout << "RILEY: \"I was getting worried about you. Let's catch up properly.\"\n";
                              p.adjustSocial(12);
                              rels.interactWith("Riley (Barista)", 12);
                          }

                          std::cout << "How much do you open up to Riley?\n";
                          std::cout << "  1) Open up\n";
                          std::cout << "  2) Keep it light\n";
                          std::cout << "  3) Make excuses\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustEnergy(-10);
                              p.adjustSocial(15);
                              rels.interactWith("Riley (Barista)", 10);
                              std::cout << "You share honestly about how you've been feeling. Riley listens closely.\n";
                          } else if (c == 2) {
                              p.adjustEnergy(-5);
                              p.adjustSocial(8);
                              rels.interactWith("Riley (Barista)", 3);
                              std::cout << "You keep things light and chatty, avoiding anything too heavy.\n";
                          } else {
                              p.adjustEnergy(-2);
                              p.adjustSocial(2);
                              rels.interactWith("Riley (Barista)", -5);
                              std::cout << "You make vague excuses and change the subject. The distance remains.\n";
                          }

                          // Week 5 random event: moving gig pays well.
                          Events::week5SocialRebuildingMovingGig(p);
                      }},
                 }});

    s.push_back({5, 3, "Various",
                 "Late afternoon. The summer rhythm feels more natural now.",
                 {
                     {"Sustainable workout", "Energy -15, Fitness +7, Health +2", {-15, 2, 0, 0, 7, 0},
                      [](GameState&, Player& p, Relationships&) {
                          std::cout << "LOCATION: Gym\n";
                          std::cout << "You focus on form and consistency over intensity.\n";
                          if (roll(0.30)) {
                              std::cout << "RANDOM EVENT: DISCOVER NEW TRAINING METHOD!\n";
                              std::cout << "A small tweak in your routine makes everything feel more efficient.\n";
                              p.adjustFitness(8);
                              p.adjustEnergy(5);
                          }
                      }},
                     {"Effective studying", "Energy -18, Academic +13", {-18, 0, 0, 13, 0, 0},
                      [](GameState&, Player& p, Relationships&) {
                          std::cout << "LOCATION: Library\n";
                          std::cout << "You use proven study techniques instead of just grinding.\n";
                          std::cout << "Which technique do you use?\n";
                          std::cout << "  1) Pomodoro method (focused bursts)\n";
                          std::cout << "  2) Active recall (deep learning)\n";
                          std::cout << "  3) Spaced repetition (long-term retention)\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustEnergy(3);
                              p.adjustAcademic(-1);
                          } else if (c == 2) {
                              p.adjustAcademic(1);
                          } else {
                              p.adjustEnergy(6);
                              p.adjustAcademic(-3);
                          }
                          // Week 5 random event: paid study group leader.
                          Events::week5EffectiveStudyingStudyLeader(p);
                      }},
                     {"Meaningful socializing", "Energy -12, Social +10, Money -8", {-12, 0, 10, 0, 0, -8},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Various Social Spaces\n";
                          std::cout << "You prioritize quality connections over quantity.\n";

                          int rileyAffinity = 0;
                          for (const auto& npc : rels.npcs) {
                              if (npc.name.find("Riley (Barista)") != std::string::npos) {
                                  rileyAffinity = npc.affinity;
                                  break;
                              }
                          }

                          if (rileyAffinity > 45) {
                              std::cout << "RILEY: \"I feel like we're becoming real friends! Wanna go out tomorrow?\"\n";
                              p.adjustSocial(12);
                              rels.interactWith("Riley (Barista)", 10);
                          }

                          std::cout << "How do you want to spend this time?\n";
                          std::cout << "  1) One-on-one time\n";
                          std::cout << "  2) Small group gathering\n";
                          std::cout << "  3) Help someone in need\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustEnergy(-8);
                              p.adjustSocial(10);
                              p.adjustMoney(-20);
                              if (rileyAffinity > 45) {
                                  rels.interactWith("Riley (Barista)", 10);
                              } else {
                                  rels.interactWith("Riley (Barista)", -5);
                              }
                          } else if (c == 2) {
                              p.adjustEnergy(-12);
                              p.adjustSocial(8);
                              p.adjustMoney(-12);
                          } else {
                              p.adjustEnergy(-10);
                              p.adjustSocial(12);
                          }

                          // Week 5 random event: weekend getaway with friends.
                          Events::week5MeaningfulSocialGetaway(p);

                          st.metRiley = true;
                      }},
                 }});

    s.push_back({5, 4, "Dorm Room",
                 "Friday evening. You've found better balance this week.",
                 {
                     {"Restorative activities", "Energy +25, Health +8", {25, 8, 0, 0, 0, 0},
                      [](GameState&, Player& p, Relationships&) {
                          std::cout << "LOCATION: Dorm Room\n";
                          std::cout << "Your body and mind thank you for the proper rest.\n";

                          if (p.health < 60) {
                              std::cout << "Your body was desperately craving this recovery.\n";
                              p.adjustHealth(12);
                              p.adjustEnergy(15);
                          }

                          std::cout << "How do you spend this restorative time?\n";
                          std::cout << "  1) Yoga and meditation\n";
                          std::cout << "  2) Quality sleep\n";
                          std::cout << "  3) Nature walk\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustEnergy(20);
                              p.adjustHealth(6);
                              p.adjustAcademic(2);
                          } else if (c == 2) {
                              p.adjustEnergy(30);
                              p.adjustHealth(8);
                          } else {
                              p.adjustEnergy(15);
                              p.adjustHealth(5);
                              p.adjustSocial(3);
                          }

                          // Week 5 random event: perfect recovery day.
                          Events::week5RestorativePerfectRecovery(p);
                      }},
                     {"Weekend preparation", "Energy -15, All stats +4", {-15, 4, 4, 4, 4, 0},
                      [](GameState&, Player& p, Relationships&) {
                          std::cout << "LOCATION: Dorm Desk\n";
                          std::cout << "You plan the perfect balanced weekend.\n";

                          bool allHigh = (p.energy > 55 && p.health > 55 && p.social > 55 &&
                                          p.academic > 55 && p.fitness > 55);
                          if (allHigh) {
                              std::cout << "You're maintaining impressive all-around progress.\n";
                              p.adjustEnergy(3);
                              p.adjustHealth(3);
                              p.adjustSocial(3);
                              p.adjustAcademic(3);
                              p.adjustFitness(3);
                          }

                          std::cout << "What will you focus on this weekend?\n";
                          std::cout << "  1) Productivity focus\n";
                          std::cout << "  2) Social focus\n";
                          std::cout << "  3) Health focus\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustEnergy(-12);
                              p.adjustAcademic(8);
                              p.adjustMoney(20);
                          } else if (c == 2) {
                              p.adjustEnergy(-10);
                              p.adjustSocial(10);
                          } else {
                              p.adjustEnergy(-8);
                              p.adjustHealth(6);
                              p.adjustFitness(4);
                          }

                          // Week 5 random event: weekend gig pays double.
                          Events::week5WeekendPrepDoubleGig(p);
                      }},
                     {"Stock investing", "Energy -10, Social +8, Money swing", {-10, 0, 8, 0, 0, -5},
                      [](GameState&, Player& p, Relationships&) {
                          std::cout << "LOCATION: Dorm\n";
                          std::cout << "You open your brokerage app and stare at the charts.\n";
                          // Week 5 random event: stock investing outcome (handles the choice details).
                          Events::week5StockInvestingOutcome(p);
                      }},
                 }});

    // Week 6
    s.push_back({6, 1, "Dorm Room",
                 "Love triangles & drama: where do you focus your time today?",
                 {
                     {"Workout with Alex", "Energy -20, Fitness +8, Alex time", {-20, 0, 0, 0, 8, 0},
                      [](GameState&, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Gym\n";
                          std::cout << "Alex texts you: \"Gym later? I want to show you a new routine.\"\n";
                          std::cout << "You arrive to find Alex already warming up.\n";
                          std::cout << "How do you handle the time?\n";
                          std::cout << "  1) Focus entirely on Alex\n";
                          std::cout << "  2) Split your attention (check phone, think of others)\n";
                          std::cout << "  3) Cancel last minute\n";
                          int c = promptInt(1, 3);

                          if (c == 1) {
                              p.adjustFitness(4);
                              rels.interactWith("Alex (Gym Crush)", 15);
                              std::cout << "You give Alex your full attention. The workout and the vibe are great.\n";
                          } else if (c == 2) {
                              p.adjustFitness(2);
                              rels.interactWith("Alex (Gym Crush)", 3);
                              std::cout << "You keep glancing at your phone and thinking about other plans.\n";
                              std::cout << "Alex notices your distraction but lets it slide.\n";
                          } else {
                              p.adjustEnergy(10);
                              rels.interactWith("Alex (Gym Crush)", -10);
                              std::cout << "You bail, telling Alex you're too tired. The reply is short and cold.\n";
                          }

                          // Competitive tension flare-up between Alex and Sam (drama flavor).
                          if (roll(0.50)) {
                              std::cout << "\nCOMPETITIVE TENSION ERUPTS!\n";
                              std::cout << "Alex and Sam start subtly competing for your attention.\n";
                              bool favorAlex = roll(0.5);
                              if (favorAlex) {
                                  rels.interactWith("Alex (Gym Crush)", 12);
                                  rels.interactWith("Sam (Study Buddy)", -15);
                              } else {
                                  rels.interactWith("Sam (Study Buddy)", 12);
                                  rels.interactWith("Alex (Gym Crush)", -15);
                              }
                          }
                      }},
                     {"Study session with Sam", "Energy -18, Academic +10, Sam vs Riley tension", {-18, 0, 0, 10, 0, 0},
                      [](GameState&, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Library\n";
                          std::cout << "Sam has books spread everywhere when Riley shows up looking for you.\n";
                          std::cout << "SAM: \"I reserved this study room for us... Riley?\"\n";
                          std::cout << "RILEY: \"Hey! I was hoping to catch you before my shift. Got a minute?\"\n";
                          std::cout << "What do you do?\n";
                          std::cout << "  1) Study with Sam\n";
                          std::cout << "  2) Quick chat with Riley\n";
                          std::cout << "  3) Try to include both\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustAcademic(2);
                              rels.interactWith("Sam (Study Buddy)", 10);
                              rels.interactWith("Riley (Barista)", -10);
                              std::cout << "You stick with Sam. Riley leaves with a small frown.\n";
                          } else if (c == 2) {
                              p.adjustSocial(8);
                              rels.interactWith("Riley (Barista)", 8);
                              rels.interactWith("Sam (Study Buddy)", -12);
                              std::cout << "You step outside with Riley for a quick chat while Sam stews inside.\n";
                          } else {
                              p.adjustAcademic(-5);
                              p.adjustSocial(5);
                              rels.interactWith("Sam (Study Buddy)", -25);
                              rels.interactWith("Riley (Barista)", -25);
                              std::cout << "You try to juggle both at once. Nobody feels properly seen.\n";
                          }

                          // Positive random: unexpected study group
                          if (roll(0.30)) {
                              std::cout << "\nUNEXPECTED STUDY GROUP FORMS!\n";
                              std::cout << "A few classmates join in and it becomes a productive group session.\n";
                              p.adjustAcademic(15);
                              p.adjustSocial(8);
                              rels.interactWith("Sam (Study Buddy)", 25);
                              rels.interactWith("Riley (Barista)", 25);
                          }
                      }},
                     {"Coffee date with Riley", "Energy -15, Social +10, Riley vs Alex tension", {-15, 0, 10, 0, 0, 0},
                      [](GameState&, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Campus Cafe\n";
                          std::cout << "Riley has your usual ready, but Alex texts they're nearby and wants to join.\n";
                          std::cout << "RILEY: \"I was hoping we could have some one-on-one time... but if Alex wants to come...\"\n";
                          std::cout << "How do you respond?\n";
                          std::cout << "  1) Keep it just us\n";
                          std::cout << "  2) Invite Alex over\n";
                          std::cout << "  3) Reschedule both\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustSocial(5);
                              rels.interactWith("Riley (Barista)", 12);
                              rels.interactWith("Alex (Gym Crush)", -8);
                              std::cout << "You gently tell Alex another time. Riley seems genuinely happy.\n";
                          } else if (c == 2) {
                              p.adjustSocial(-4);
                              rels.interactWith("Riley (Barista)", -5);
                              rels.interactWith("Alex (Gym Crush)", 8);
                              std::cout << "Alex joins the table. Riley looks a little disappointed.\n";
                          } else {
                              p.adjustEnergy(15);
                              rels.interactWith("Riley (Barista)", -30);
                              rels.interactWith("Alex (Gym Crush)", -30);
                              std::cout << "You cancel on both. The devs are disappointed in you.\n";
                          }

                          // Drama random event
                          if (roll(0.35)) {
                              std::cout << "\nAWKWARD SILENCE! The person you didn't choose shows up anyway.\n";
                              p.adjustSocial(-5);
                              rels.interactWith("Riley (Barista)", -15);
                              rels.interactWith("Alex (Gym Crush)", -15);
                          }
                      }},
                 }});

    s.push_back({6, 2, "Various",
                 "Midday chaos! Group dynamics, solitude, or a crisis call your name.",
                 {
                     {"Campus event with friends", "Energy -15, Social +15", {-15, 0, 15, 0, 0, 0},
                      [](GameState&, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Campus Quad\n";
                          std::cout << "Alex, Sam, and Riley are all at the summer festival, sneaking glances your way.\n";
                          std::cout << "Who do you spend most of your time with?\n";
                          std::cout << "  1) Hang with Alex\n";
                          std::cout << "  2) Study corner with Sam\n";
                          std::cout << "  3) Food trucks with Riley\n";
                          std::cout << "  4) Try to rotate between all\n";
                          int c = promptInt(1, 4);
                          if (c == 1) {
                              p.adjustSocial(3);
                              rels.interactWith("Alex (Gym Crush)", 12);
                              rels.interactWith("Sam (Study Buddy)", -10);
                              rels.interactWith("Riley (Barista)", -10);
                          } else if (c == 2) {
                              p.adjustSocial(1);
                              rels.interactWith("Sam (Study Buddy)", 12);
                              rels.interactWith("Alex (Gym Crush)", -10);
                              rels.interactWith("Riley (Barista)", -10);
                          } else if (c == 3) {
                              p.adjustSocial(5);
                              rels.interactWith("Riley (Barista)", 12);
                              rels.interactWith("Alex (Gym Crush)", -10);
                              rels.interactWith("Sam (Study Buddy)", -10);
                          } else {
                              p.adjustSocial(-10);
                              rels.interactWith("Alex (Gym Crush)", 1);
                              rels.interactWith("Sam (Study Buddy)", 1);
                              rels.interactWith("Riley (Barista)", 1);
                              std::cout << "You try to divide your time equally. It's exhausting but diplomatic.\n";
                          }
                      }},
                     {"Quiet escape alone", "Energy +20, All stats +2", {20, 2, 2, 2, 2, 0},
                      [](GameState&, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Hidden Campus Spot\n";
                          std::cout << "You find a secret garden nobody knows about. Perfect solitude.\n";

                          int highRels = 0;
                          for (const auto& npc : rels.npcs) {
                              if (npc.affinity > 60) ++highRels;
                          }

                          if (p.social > 70) {
                              std::cout << "Your social battery is drained. This is necessary self-care.\n";
                              p.adjustEnergy(5);
                              p.adjustHealth(10);
                          }

                          if (highRels >= 2) {
                              std::cout << "You realize you've been spreading yourself too thin.\n";
                              std::cout << "Clarity emerges from solitude.\n";
                              p.adjustEnergy(3);
                              p.adjustHealth(3);
                              p.adjustSocial(3);
                              p.adjustAcademic(3);
                              p.adjustFitness(3);
                          }
                      }},
                     {"Help someone in crisis", "Energy -25, Hero moment", {-25, 0, 0, 0, 0, 0},
                      [](GameState&, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Various\n";
                          std::cout << "You get an urgent message from someone having a rough day.\n";
                          // 0: Alex, 1: Sam, 2: Riley
                          int who = rand() % 3;
                          std::string name;
                          if (who == 0) {
                              name = "Alex (Gym Crush)";
                              std::cout << "Alex was injured during a workout and needs help getting to the clinic.\n";
                          } else if (who == 1) {
                              name = "Sam (Study Buddy)";
                              std::cout << "Sam is having an academic meltdown before a big exam.\n";
                          } else {
                              name = "Riley (Barista)";
                              std::cout << "Riley has a family emergency and needs emotional support.\n";
                          }

                          std::cout << "How do you respond?\n";
                          std::cout << "  1) Drop everything and help\n";
                          std::cout << "  2) Send help but can't come\n";
                          std::cout << "  3) Ignore the message\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustEnergy(-5);
                              rels.interactWith(name, 20);
                          } else if (c == 2) {
                              p.adjustEnergy(15);
                              rels.interactWith(name, 5);
                          } else {
                              p.adjustEnergy(30);
                              rels.interactWith(name, -40);
                          }
                      }},
                 }});

    s.push_back({6, 3, "Various",
                 "Afternoon decisions get more intense as feelings surface.",
                 {
                     {"Fitness competition with Alex", "Energy -25, Fitness +12", {-25, 0, 0, 0, 12, 0},
                      [](GameState&, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Gym Competition\n";
                          std::cout << "You and Alex sign up for a fitness challenge. Sam and Riley come to cheer.\n";
                          std::cout << "How do you carry yourself?\n";
                          std::cout << "  1) Focus only on Alex\n";
                          std::cout << "  2) Acknowledge the crowd\n";
                          std::cout << "  3) Try to impress everyone\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustFitness(4);
                              rels.interactWith("Alex (Gym Crush)", 15);
                              rels.interactWith("Sam (Study Buddy)", -8);
                              rels.interactWith("Riley (Barista)", -8);
                          } else if (c == 2) {
                              p.adjustFitness(-4);
                              p.adjustSocial(5);
                              rels.interactWith("Alex (Gym Crush)", -10);
                          } else {
                              p.adjustFitness(-7);
                              p.adjustSocial(8);
                              rels.interactWith("Alex (Gym Crush)", 1);
                              rels.interactWith("Sam (Study Buddy)", 1);
                              rels.interactWith("Riley (Barista)", 1);
                          }
                      }},
                     {"Hackathon with Sam", "Energy -22, Academic +15", {-22, 0, 0, 15, 0, 0},
                      [](GameState&, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Hackathon Competition\n";
                          std::cout << "You and Sam make an unbeatable team, but distractions abound.\n";
                          int strongRels = 0;
                          for (const auto& npc : rels.npcs) {
                              if (npc.affinity > 50) ++strongRels;
                          }
                          if (strongRels >= 2) {
                              std::cout << "Your phone keeps buzzing with messages from other friends. Sam notices.\n";
                          }
                          // Week 6 random event: hackathon victory.
                          Events::week6HackathonWin(p, rels);
                      }},
                     {"Double date situation", "Energy -18, Social +15, Money -15", {-18, 0, 15, 0, 0, -15},
                      [](GameState& st, Player&, Relationships& rels) {
                          std::cout << "LOCATION: Various\n";
                          std::cout << "You end up at a social event with multiple people who might like you.\n";
                          std::cout << "It feels like a double date with extra spectators.\n";
                          std::cout << "How do you behave?\n";
                          std::cout << "  1) Flirt with person A (Alex)\n";
                          std::cout << "  2) Flirt with person B (Sam/Riley)\n";
                          std::cout << "  3) Play neutral\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              rels.interactWith("Alex (Gym Crush)", 15);
                              rels.interactWith("Sam (Study Buddy)", -15);
                              rels.interactWith("Riley (Barista)", -15);
                          } else if (c == 2) {
                              rels.interactWith("Sam (Study Buddy)", 15);
                              rels.interactWith("Riley (Barista)", 15);
                              rels.interactWith("Alex (Gym Crush)", -15);
                          } else {
                              rels.interactWith("Alex (Gym Crush)", -5);
                              rels.interactWith("Sam (Study Buddy)", -5);
                              rels.interactWith("Riley (Barista)", -5);
                          }
                          st.relationshipPath = "drama";
                      }},
                 }});

    s.push_back({6, 4, "Dorm Room",
                 "Friday night. Time to make some relationship decisions before the weekend.",
                 {
                     {"Confront the love triangle", "Energy -20, Force relationship choices", {-20, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Your Dorm\n";

                          int alexA = 0, samA = 0, rileyA = 0;
                          for (const auto& npc : rels.npcs) {
                              if (npc.name.find("Alex (Gym Crush)") != std::string::npos) alexA = npc.affinity;
                              if (npc.name.find("Sam (Study Buddy)") != std::string::npos) samA = npc.affinity;
                              if (npc.name.find("Riley (Barista)") != std::string::npos) rileyA = npc.affinity;
                          }
                          int over65 = (alexA > 65) + (samA > 65) + (rileyA > 65);

                          if (over65 == 0) {
                              std::cout << "Sorry, this option isn't really available... nobody is that into you yet.\n";
                              return;
                          }
                          if (over65 == 1) {
                              std::cout << "You actually already know who you care about most.\n";
                          } else {
                              std::cout << "This is going to be painful, but you need to choose.\n";
                          }

                          std::cout << "Who do you choose?\n";
                          std::cout << "  1) Alex\n";
                          std::cout << "  2) Sam\n";
                          std::cout << "  3) Riley\n";
                          std::cout << "  4) Propose polyamory\n";
                          int c = promptInt(1, 4);

                          auto dropOthers = [&](const std::string& keep) {
                              if (keep != "Alex (Gym Crush)") rels.interactWith("Alex (Gym Crush)", -40);
                              if (keep != "Sam (Study Buddy)") rels.interactWith("Sam (Study Buddy)", -40);
                              if (keep != "Riley (Barista)") rels.interactWith("Riley (Barista)", -40);
                          };

                          if (c == 1) {
                              st.relationshipPath = "partner";
                              st.partnerName = "Alex (Gym Crush)";
                              rels.interactWith("Alex (Gym Crush)", 25);
                              dropOthers("Alex (Gym Crush)");
                              p.adjustFitness(5);
                          } else if (c == 2) {
                              st.relationshipPath = "partner";
                              st.partnerName = "Sam (Study Buddy)";
                              rels.interactWith("Sam (Study Buddy)", 25);
                              dropOthers("Sam (Study Buddy)");
                              p.adjustAcademic(5);
                          } else if (c == 3) {
                              st.relationshipPath = "partner";
                              st.partnerName = "Riley (Barista)";
                              rels.interactWith("Riley (Barista)", 25);
                              dropOthers("Riley (Barista)");
                              p.adjustSocial(5);
                          } else {
                              // Polyamory attempt
                              if (roll(0.05)) {
                                  std::cout << "Somehow, it works. Everyone's surprisingly okay with it.\n";
                                  rels.interactWith("Alex (Gym Crush)", 30);
                                  rels.interactWith("Sam (Study Buddy)", 30);
                                  rels.interactWith("Riley (Barista)", 30);
                                  st.relationshipPath = "drama";
                              } else {
                                  std::cout << "The conversation explodes. Nobody is happy.\n";
                                  rels.interactWith("Alex (Gym Crush)", -50);
                                  rels.interactWith("Sam (Study Buddy)", -50);
                                  rels.interactWith("Riley (Barista)", -50);
                                  st.relationshipPath = "drama";
                              }
                          }
                      }},
                     {"Avoid everyone and game", "Energy -10, Social +5 (online)", {-10, 0, 5, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Dorm Room\n";
                          std::cout << "You log into your favorite game. Jordan is your safe space.\n";
                          std::cout << "JORDAN: \"Rough week? Let's forget real life for a few hours.\"\n";
                          st.relationshipPath = "avoidance";
                          rels.interactWith("Jordan (Gamer)", 10);
                          // Week 6 random event: therapeutic gaming marathon.
                          Events::week6GamingMarathonTherapeutic(p, rels);
                      }},
                 }});

    // Week 7
    s.push_back({7, 1, "Dorm Room",
                 "Monday morning. The fallout from last week is still fresh.",
                 {
                     {"Spend time with your partner", "Energy -20, Affinity +15, Social +10", {-20, 0, 10, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          if (st.relationshipPath != "partner" || st.partnerName.empty()) return;
                          std::cout << "LOCATION: Partner's spot\n";
                          std::cout << "You meet up with your chosen partner to start the week together.\n";
                          std::cout << "  1) Go for breakfast together\n";
                          std::cout << "  2) Workout/study together\n";
                          std::cout << "  3) Skip plans and just hang out\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustSocial(10);
                              p.adjustMoney(-15);
                              rels.interactWith(st.partnerName, 15);
                          } else if (c == 2) {
                              std::cout << "Focus on 1) Fitness or 2) Academics?\n";
                              int sub = promptInt(1, 2);
                              if (sub == 1) {
                                  p.adjustFitness(8);
                              } else {
                                  p.adjustAcademic(10);
                              }
                              p.adjustEnergy(-20);
                              rels.interactWith(st.partnerName, 12);
                          } else {
                              p.adjustEnergy(10);
                              rels.interactWith(st.partnerName, 8);
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "No partner chosen"},
                     {"Enjoy single life freedom", "Energy -15, Social +12, Money -10", {-15, 0, 12, 0, 0, -10},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Campus Grounds\n";
                          std::cout << "You decide to embrace being single and free.\n";
                          std::cout << "  1) Flirt casually with someone new\n";
                          std::cout << "  2) Go shopping alone\n";
                          std::cout << "  3) Relax solo at a cafe\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustSocial(12);
                              p.adjustEnergy(-15);
                              rels.interactWith("New Crush", 10);
                          } else if (c == 2) {
                              p.adjustMoney(-30);
                              p.adjustSocial(5);
                              p.adjustEnergy(-10);
                          } else {
                              p.adjustEnergy(20);
                              p.adjustSocial(3);
                              p.adjustMoney(-10);
                          }
                          st.relationshipPath = "single";
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath != "partner"; },
                      "Locked to non-partner paths"},
                     {"Deal with campus gossip", "Energy -25, Social -5", {-25, 0, -5, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Hallway\n";
                          std::cout << "Whispers follow you everywhere after last week's drama.\n";
                          std::cout << "  1) Confront the gossip directly\n";
                          std::cout << "  2) Laugh it off publicly\n";
                          std::cout << "  3) Hide from everyone\n";
                          int c = promptInt(1, 3);
                          int idx = rand() % static_cast<int>(rels.npcs.size());
                          std::string randName = rels.npcs[idx].name;
                          if (c == 1) {
                              p.adjustSocial(5);
                              p.adjustEnergy(-20);
                              rels.interactWith(randName, 10);
                          } else if (c == 2) {
                              p.adjustSocial(10);
                              p.adjustEnergy(-15);
                              rels.interactWith(randName, 5);
                          } else {
                              p.adjustEnergy(25);
                              p.adjustSocial(-10);
                          }
                          st.relationshipPath = "drama";
                      }},
                     {"Recharge alone", "Energy +25, Health +10", {25, 10, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          std::cout << "LOCATION: Dorm Room\n";
                          std::cout << "You decide to ignore everyone and focus on yourself.\n";
                          std::cout << "  1) Meditate and journal\n";
                          std::cout << "  2) Cook a healthy meal\n";
                          std::cout << "  3) Sleep in all day\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustHealth(10);
                              p.adjustEnergy(20);
                          } else if (c == 2) {
                              p.adjustHealth(12);
                              p.adjustMoney(-15);
                              p.adjustEnergy(10);
                          } else {
                              p.adjustEnergy(35);
                              p.adjustHealth(5);
                          }
                          st.relationshipPath = "avoidance";
                      }},
                 }});

    s.push_back({7, 2, "Campus Cafe",
                 "Attempted polyamory: Alex, Sam, and Riley all show up.",
                 {
                     {"Propose polyamory openly", "Energy -25, very risky conversation", {-25, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Cafe Table\n";
                          std::cout << "You explain your idea of being with more than one person honestly.\n";
                          std::cout << "  1) Make a heartfelt case\n";
                          std::cout << "  2) Frame it as casual fun\n";
                          std::cout << "  3) Apologize mid-speech\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              if (roll(0.20)) {
                                  std::cout << "Against all odds, they agree to try it.\n";
                                  rels.interactWith("Alex (Gym Crush)", 20);
                                  rels.interactWith("Sam (Study Buddy)", 20);
                                  rels.interactWith("Riley (Barista)", 20);
                                  p.adjustSocial(15);
                              } else {
                                  std::cout << "They react badly to the idea.\n";
                                  rels.interactWith("Alex (Gym Crush)", -30);
                                  rels.interactWith("Sam (Study Buddy)", -30);
                                  rels.interactWith("Riley (Barista)", -30);
                              }
                          } else if (c == 2) {
                              rels.interactWith("Riley (Barista)", -15);
                              rels.interactWith("Alex (Gym Crush)", -10);
                              rels.interactWith("Sam (Study Buddy)", -10);
                              p.adjustSocial(5);
                          } else {
                              p.adjustEnergy(10);
                              rels.interactWith("Alex (Gym Crush)", -5);
                              rels.interactWith("Sam (Study Buddy)", -5);
                              rels.interactWith("Riley (Barista)", -5);
                          }
                          st.relationshipPath = "drama";
                      }},
                     {"Try to juggle conversations", "Energy -20, Social +10", {-20, 0, 10, 0, 0, 0},
                      [](GameState& st, Player&, Relationships& rels) {
                          std::cout << "You bounce between Alex, Sam, and Riley, trying to keep everyone happy.\n";
                          std::cout << "  1) Focus more on Alex\n";
                          std::cout << "  2) Focus more on Sam\n";
                          std::cout << "  3) Focus more on Riley\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              rels.interactWith("Alex (Gym Crush)", 15);
                              rels.interactWith("Sam (Study Buddy)", -10);
                              rels.interactWith("Riley (Barista)", -10);
                          } else if (c == 2) {
                              rels.interactWith("Sam (Study Buddy)", 15);
                              rels.interactWith("Alex (Gym Crush)", -10);
                              rels.interactWith("Riley (Barista)", -10);
                          } else {
                              rels.interactWith("Riley (Barista)", 15);
                              rels.interactWith("Alex (Gym Crush)", -10);
                              rels.interactWith("Sam (Study Buddy)", -10);
                          }
                          st.relationshipPath = "drama";
                      }},
                     {"Back out awkwardly", "Energy +15, Social -5", {15, 0, -5, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Cafe Exit\n";
                          std::cout << "You panic and leave suddenly.\n";
                          std::cout << "  1) Send apology texts later\n";
                          std::cout << "  2) Ignore everyone\n";
                          std::cout << "  3) Distract yourself with Jordan online\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              int idx = rand() % static_cast<int>(rels.npcs.size());
                              rels.interactWith(rels.npcs[idx].name, 5);
                              p.adjustSocial(-5);
                          } else if (c == 2) {
                              for (auto& npc : rels.npcs) {
                                  npc.affinity = std::max(0, npc.affinity - 10);
                              }
                              p.adjustEnergy(20);
                          } else {
                              rels.interactWith("Jordan (Gamer)", 10);
                              p.adjustSocial(5);
                              p.adjustEnergy(-15);
                          }
                          st.relationshipPath = "avoidance";
                      }},
                 }});

    s.push_back({7, 3, "Various",
                 "Afternoon choices reflect your summer trajectory.",
                 {
                     {"Couple workout/study session", "Partner path: shared growth", {-20, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          if (st.relationshipPath != "partner" || st.partnerName.empty()) return;
                          std::cout << "LOCATION: Gym/Library\n";
                          std::cout << "You and your partner commit to growth together.\n";
                          std::cout << "  1) Push each other hard\n";
                          std::cout << "  2) Take it easy together\n";
                          std::cout << "  3) Skip halfway\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              std::cout << "Focus on 1) Fitness or 2) Academics?\n";
                              int sub = promptInt(1, 2);
                              if (sub == 1) {
                                  p.adjustFitness(10);
                              } else {
                                  p.adjustAcademic(15);
                              }
                              p.adjustEnergy(-25);
                              rels.interactWith(st.partnerName, 12);
                          } else if (c == 2) {
                              std::cout << "Focus on 1) Fitness or 2) Academics?\n";
                              int sub = promptInt(1, 2);
                              if (sub == 1) {
                                  p.adjustFitness(5);
                              } else {
                                  p.adjustAcademic(8);
                              }
                              p.adjustEnergy(-10);
                              rels.interactWith(st.partnerName, 8);
                          } else {
                              p.adjustEnergy(15);
                              rels.interactWith(st.partnerName, -5);
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "Partner path only"},
                     {"Party hopping", "Single path: enjoy the night", {-20, 0, 15, 0, 0, -30},
                      [](GameState& st, Player& p, Relationships&) {
                          std::cout << "LOCATION: Off-Campus Party\n";
                          std::cout << "You bounce between parties, enjoying the single life.\n";
                          std::cout << "  1) Dance all night\n";
                          std::cout << "  2) Drink heavily\n";
                          std::cout << "  3) Leave early\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustSocial(20);
                              p.adjustEnergy(-30);
                              p.adjustMoney(-20);
                          } else if (c == 2) {
                              p.adjustSocial(10);
                              p.adjustHealth(-15);
                              p.adjustEnergy(-20);
                          } else {
                              p.adjustEnergy(20);
                              p.adjustSocial(5);
                          }
                          if (st.relationshipPath != "partner") {
                              st.relationshipPath = "single";
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath != "partner"; },
                      "Single/drama paths only"},
                     {"Damage control with friends", "Drama path: repair attempts", {-15, 0, 10, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Common Area\n";
                          std::cout << "You try to repair relationships after the drama.\n";
                          std::cout << "  1) Apologize sincerely\n";
                          std::cout << "  2) Deflect blame\n";
                          std::cout << "  3) Offer favors\n";
                          int c = promptInt(1, 3);
                          int idx = rand() % static_cast<int>(rels.npcs.size());
                          std::string target = rels.npcs[idx].name;
                          if (c == 1) {
                              rels.interactWith(target, 15);
                              p.adjustSocial(10);
                              p.adjustEnergy(-15);
                          } else if (c == 2) {
                              rels.interactWith(target, -10);
                              p.adjustSocial(-5);
                              p.adjustEnergy(10);
                          } else {
                              p.adjustMoney(-20);
                              rels.interactWith(target, 10);
                              p.adjustSocial(5);
                          }
                          st.relationshipPath = "drama";
                      }},
                     {"Solo meditation", "Avoidance path: focus inward", {25, 12, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          std::cout << "LOCATION: Dorm Room\n";
                          std::cout << "You focus inward, ignoring external chaos.\n";
                          std::cout << "  1) Deep breathing exercises\n";
                          std::cout << "  2) Yoga session\n";
                          std::cout << "  3) Fall asleep mid-meditation\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustHealth(12);
                              p.adjustEnergy(20);
                          } else if (c == 2) {
                              p.adjustFitness(8);
                              p.adjustHealth(5);
                              p.adjustEnergy(15);
                          } else {
                              p.adjustEnergy(30);
                              p.adjustHealth(5);
                          }
                          st.relationshipPath = "avoidance";
                      }},
                 }});

    s.push_back({7, 4, "Dorm Room",
                 "Friday night. The week's choices weigh on you.",
                 {
                     {"Deep talk with partner", "Affinity +20, Social +10", {0, 0, 10, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          if (st.relationshipPath != "partner" || st.partnerName.empty()) return;
                          std::cout << "LOCATION: Partner's Room\n";
                          std::cout << "You open up about your feelings.\n";
                          std::cout << "  1) Confess deeper emotions\n";
                          std::cout << "  2) Keep it lighthearted\n";
                          std::cout << "  3) Avoid serious topics\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              rels.interactWith(st.partnerName, 20);
                              p.adjustSocial(10);
                          } else if (c == 2) {
                              rels.interactWith(st.partnerName, 10);
                              p.adjustSocial(5);
                              p.adjustEnergy(10);
                          } else {
                              rels.interactWith(st.partnerName, -5);
                              p.adjustSocial(-5);
                              p.adjustEnergy(20);
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "Partner path only"},
                     {"Casual gaming with Jordan", "Single path: chill session", {0, 0, 8, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Online\n";
                          std::cout << "You log in with Jordan for a chill gaming session.\n";
                          std::cout << "  1) Competitive mode\n";
                          std::cout << "  2) Casual fun\n";
                          std::cout << "  3) Quit early\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustSocial(8);
                              rels.interactWith("Jordan (Gamer)", 12);
                              p.adjustEnergy(-20);
                          } else if (c == 2) {
                              p.adjustSocial(5);
                              rels.interactWith("Jordan (Gamer)", 8);
                              p.adjustEnergy(-10);
                          } else {
                              p.adjustEnergy(20);
                              rels.interactWith("Jordan (Gamer)", -5);
                          }
                          if (st.relationshipPath != "partner") {
                              st.relationshipPath = "single";
                          }
                      }},
                     {"Distract yourself with work", "Drama path: bury yourself in tasks", {-20, 0, 0, 12, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          std::cout << "LOCATION: Library\n";
                          std::cout << "You bury yourself in assignments to avoid drama.\n";
                          std::cout << "  1) Full focus\n";
                          std::cout << "  2) Half-hearted effort\n";
                          std::cout << "  3) Give up quickly\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustAcademic(15);
                              p.adjustEnergy(-25);
                          } else if (c == 2) {
                              p.adjustAcademic(8);
                              p.adjustEnergy(-10);
                          } else {
                              p.adjustAcademic(-5);
                              p.adjustEnergy(20);
                          }
                          st.relationshipPath = "drama";
                      }},
                     {"Early sleep", "Avoidance path: rest instead of engaging", {30, 10, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          std::cout << "LOCATION: Dorm Room\n";
                          std::cout << "You decide to sleep instead of engaging with anyone.\n";
                          std::cout << "  1) Sleep immediately\n";
                          std::cout << "  2) Scroll then sleep\n";
                          std::cout << "  3) Nap and wake up late\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustEnergy(30);
                              p.adjustHealth(10);
                          } else if (c == 2) {
                              p.adjustEnergy(20);
                              p.adjustHealth(5);
                          } else {
                              p.adjustEnergy(25);
                              p.adjustHealth(5);
                          }
                          st.relationshipPath = "avoidance";
                      }},
                 }});

    // Week 8
    s.push_back({8, 1, "Library",
                 "Final exams week. Your focus depends on your path.",
                 {
                     {"Study with partner", "Partner path: Academic focus with support", {0, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          if (st.relationshipPath != "partner" || st.partnerName.empty()) return;
                          std::cout << "LOCATION: Library Study Room\n";
                          std::cout << "You and your partner sit side by side, motivating each other.\n";
                          std::cout << "  1) Push through all night\n";
                          std::cout << "  2) Balanced review\n";
                          std::cout << "  3) Get distracted chatting\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustAcademic(30);
                              rels.interactWith(st.partnerName, 15);
                              p.adjustEnergy(-40);
                          } else if (c == 2) {
                              p.adjustAcademic(20);
                              rels.interactWith(st.partnerName, 10);
                              p.adjustEnergy(-25);
                          } else {
                              p.adjustAcademic(10);
                              rels.interactWith(st.partnerName, 12);
                              p.adjustEnergy(-15);
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "Partner path only"},
                     {"Solo cram session", "Single/avoidance: grind alone", {0, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          std::cout << "LOCATION: Dorm Desk\n";
                          std::cout << "You grind through textbooks alone.\n";
                          std::cout << "  1) Go all night\n";
                          std::cout << "  2) Take breaks\n";
                          std::cout << "  3) Give up halfway\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustAcademic(25);
                              p.adjustEnergy(-35);
                              p.adjustHealth(-10);
                          } else if (c == 2) {
                              p.adjustAcademic(15);
                              p.adjustEnergy(-20);
                              p.adjustHealth(5);
                          } else {
                              p.adjustAcademic(-10);
                              p.adjustEnergy(20);
                          }
                          if (st.relationshipPath == "open") st.relationshipPath = "avoidance";
                      }},
                     {"Distracted by drama", "Drama path: phone buzzing non-stop", {0, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Library\n";
                          std::cout << "Your phone keeps buzzing with messages about last week's chaos.\n";
                          std::cout << "  1) Ignore the drama\n";
                          std::cout << "  2) Respond to everyone\n";
                          std::cout << "  3) Vent online\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustAcademic(15);
                              p.adjustSocial(-5);
                              p.adjustEnergy(-20);
                          } else if (c == 2) {
                              p.adjustSocial(10);
                              p.adjustAcademic(-5);
                              p.adjustEnergy(-25);
                          } else {
                              p.adjustSocial(5);
                              rels.interactWith("Jordan (Gamer)", 10);
                              p.adjustAcademic(-10);
                          }
                          st.relationshipPath = "drama";
                      }},
                 }});

      s.push_back({8, 2, "Campus Quad",
                   "The big end-of-summer party arrives. Everyone is there.",
                 {
                      {"Attend with partner", "Partner path: arrive together", {},
                        [](GameState& st, Player& p, Relationships& rels) {
                            if (st.relationshipPath != "partner" || st.partnerName.empty()) return;
                            p.adjustMoney(-40);
                            std::cout << "\nLOCATION: Party Grounds\n";
                            std::cout << "You and your partner arrive together, drawing attention.\n";
                            std::cout << "  1) Dance together all night\n";
                            std::cout << "  2) Stay low-key\n";
                            std::cout << "  3) Leave early\n";
                            int choice = promptInt(1, 3);
                            if (choice == 1) {
                                p.adjustSocial(20);
                                rels.interactWith(st.partnerName, 20);
                                p.adjustEnergy(-30);
                            } else if (choice == 2) {
                                p.adjustSocial(10);
                                rels.interactWith(st.partnerName, 10);
                                p.adjustEnergy(-15);
                            } else {
                                rels.interactWith(st.partnerName, 5);
                                p.adjustSocial(5);
                                p.adjustEnergy(20);
                            }
                        },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "Partner path only"},
                       {"Go wild single", "Single path: embrace chaos", {},
                        [](GameState& st, Player& p, Relationships& rels) {
                            p.adjustMoney(-50);
                            std::cout << "\nLOCATION: Party Grounds\n";
                            std::cout << "You embrace the chaos of being single.\n";
                            std::cout << "  1) Flirt with multiple people\n";
                            std::cout << "  2) Drink heavily\n";
                            std::cout << "  3) Dance until collapse\n";
                            int choice = promptInt(1, 3);
                            if (choice == 1) {
                                p.adjustSocial(20);
                                p.adjustEnergy(-25);
                                if (!rels.npcs.empty()) {
                                    int idx = rand() % static_cast<int>(rels.npcs.size());
                                    rels.interactWith(rels.npcs[idx].name, 10);
                                }
                            } else if (choice == 2) {
                                p.adjustSocial(10);
                                p.adjustHealth(-15);
                                p.adjustEnergy(-20);
                            } else {
                                p.adjustSocial(25);
                                p.adjustEnergy(-40);
                                p.adjustHealth(-10);
                            }
                            if (st.relationshipPath != "partner") {
                                st.relationshipPath = "single";
                            }
                        }},
                       {"Show up despite drama", "Drama/poly path: face tension", {},
                        [](GameState& st, Player& p, Relationships& rels) {
                            p.adjustMoney(-20);
                            std::cout << "\nLOCATION: Party Grounds\n";
                            std::cout << "Alex, Sam, and Riley all notice you at once.\n";
                            std::cout << "  1) Try to balance everyone\n";
                            std::cout << "  2) Focus on one person\n";
                            std::cout << "  3) Avoid them entirely\n";
                            int choice = promptInt(1, 3);
                            if (choice == 1) {
                                bool success = roll(0.20);
                                if (success) {
                                    rels.interactWith("Alex (Gym Crush)", 10);
                                    rels.interactWith("Sam (Study Buddy)", 10);
                                    rels.interactWith("Riley (Barista)", 10);
                                } else {
                                    rels.interactWith("Alex (Gym Crush)", -20);
                                    rels.interactWith("Sam (Study Buddy)", -20);
                                    rels.interactWith("Riley (Barista)", -20);
                                }
                            } else if (choice == 2) {
                                std::cout << "Focus on:\n";
                                std::cout << "  1) Alex\n";
                                std::cout << "  2) Sam\n";
                                std::cout << "  3) Riley\n";
                                int target = promptInt(1, 3);
                                std::string focusName;
                                if (target == 1) focusName = "Alex (Gym Crush)";
                                else if (target == 2) focusName = "Sam (Study Buddy)";
                                else focusName = "Riley (Barista)";
                                rels.interactWith(focusName, 15);
                                if (focusName != "Alex (Gym Crush)") rels.interactWith("Alex (Gym Crush)", -10);
                                if (focusName != "Sam (Study Buddy)") rels.interactWith("Sam (Study Buddy)", -10);
                                if (focusName != "Riley (Barista)") rels.interactWith("Riley (Barista)", -10);
                            } else {
                                p.adjustSocial(-5);
                                p.adjustEnergy(20);
                            }
                            st.relationshipPath = "drama";
                        }},
                       {"Skip party", "Avoidance path: stay in", {},
                        [](GameState& st, Player& p, Relationships& rels) {
                            std::cout << "\nLOCATION: Dorm Room\n";
                            std::cout << "You stay in, resting instead of partying.\n";
                            std::cout << "  1) Sleep early\n";
                            std::cout << "  2) Study quietly\n";
                            std::cout << "  3) Game with Jordan\n";
                            int choice = promptInt(1, 3);
                            if (choice == 1) {
                                p.adjustEnergy(30);
                                p.adjustHealth(10);
                            } else if (choice == 2) {
                                p.adjustAcademic(10);
                                p.adjustEnergy(-15);
                            } else {
                                p.adjustSocial(10);
                                p.adjustEnergy(-15);
                                rels.interactWith("Jordan (Gamer)", 12);
                            }
                            st.relationshipPath = "avoidance";
                        }},
                 }});

    s.push_back({8, 3, "Dorm Room",
                 "Final weekend. Time to reflect on your summer.",
                 {
                     {"Future planning with partner", "Partner path: talk about the future", {0, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          if (st.relationshipPath != "partner" || st.partnerName.empty()) return;
                          std::cout << "LOCATION: Partner's Room\n";
                          std::cout << "You and your partner talk about the future.\n";
                          std::cout << "  1) Plan next semester together\n";
                          std::cout << "  2) Plan fitness goals together\n";
                          std::cout << "  3) Plan social adventures\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustAcademic(10);
                          } else if (c == 2) {
                              p.adjustFitness(10);
                          } else {
                              p.adjustSocial(10);
                          }
                          rels.interactWith(st.partnerName, 15);
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "Partner path only"},
                     {"Set solo goals", "Single path: write down ambitions", {0, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          std::cout << "LOCATION: Dorm Desk\n";
                          std::cout << "You write down your ambitions for the next year.\n";
                          std::cout << "  1) Academic focus\n";
                          std::cout << "  2) Fitness focus\n";
                          std::cout << "  3) Social focus\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustAcademic(15);
                          } else if (c == 2) {
                              p.adjustFitness(15);
                          } else {
                              p.adjustSocial(15);
                          }
                          p.adjustEnergy(-20);
                          if (st.relationshipPath != "partner") {
                              st.relationshipPath = "single";
                          }
                      }},
                     {"Try to repair drama damage", "Drama/poly path: reach out", {0, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Cafe\n";
                          std::cout << "You reach out to those you hurt.\n";
                          std::cout << "  1) Apologize sincerely\n";
                          std::cout << "  2) Offer favors\n";
                          std::cout << "  3) Deflect blame again\n";
                          int c = promptInt(1, 3);
                          int idx = rand() % static_cast<int>(rels.npcs.size());
                          std::string target = rels.npcs[idx].name;
                          if (c == 1) {
                              rels.interactWith(target, 15);
                              p.adjustSocial(10);
                              p.adjustEnergy(-15);
                          } else if (c == 2) {
                              p.adjustMoney(-20);
                              rels.interactWith(target, 10);
                              p.adjustSocial(5);
                              p.adjustEnergy(-20); // net with base
                          } else {
                              rels.interactWith(target, -10);
                              p.adjustSocial(-5);
                              p.adjustEnergy(10);
                          }
                          st.relationshipPath = "drama";
                      }},
                     {"Rest deeply", "Avoidance path: prioritize recovery", {0, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          std::cout << "LOCATION: Dorm Room\n";
                          std::cout << "You prioritize recovery above all else.\n";
                          std::cout << "  1) Sleep long hours\n";
                          std::cout << "  2) Meditate deeply\n";
                          std::cout << "  3) Do nothing at all\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustEnergy(40);
                              p.adjustHealth(15);
                          } else if (c == 2) {
                              p.adjustHealth(10);
                              p.adjustEnergy(20);
                          } else {
                              p.adjustEnergy(25);
                              p.adjustSocial(-5);
                          }
                          st.relationshipPath = "avoidance";
                      }},
                 }});

    s.push_back({8, 4, "Campus Quad",
                 "Sunday evening. The summer ends.",
                 {
                     {"Farewell with partner", "Partner path: final moment", {0, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          if (st.relationshipPath != "partner" || st.partnerName.empty()) return;
                          std::cout << "LOCATION: Gym/Library/Cafe\n";
                          std::cout << "You share a final moment with your partner.\n";
                          std::cout << "  1) Promise to stay in touch\n";
                          std::cout << "  2) Make future plans\n";
                          std::cout << "  3) Keep it casual\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              rels.interactWith(st.partnerName, 20);
                              p.adjustSocial(10);
                          } else if (c == 2) {
                              rels.interactWith(st.partnerName, 15);
                              p.adjustAcademic(5);
                          } else {
                              rels.interactWith(st.partnerName, 10);
                              p.adjustEnergy(10);
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "partner"; },
                      "Partner path only"},
                     {"Farewell with friends", "Single path: gather friends", {0, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Common Area\n";
                          std::cout << "You gather your closest friends for a goodbye.\n";
                          std::cout << "  1) Host a small dinner\n";
                          std::cout << "  2) Play games together\n";
                          std::cout << "  3) Quick hugs and goodbyes\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustSocial(15);
                              p.adjustMoney(-30);
                          } else if (c == 2) {
                              p.adjustSocial(10);
                              rels.interactWith("Jordan (Gamer)", 10);
                          } else {
                              p.adjustSocial(5);
                              p.adjustEnergy(15);
                          }
                          if (st.relationshipPath != "partner") {
                              st.relationshipPath = "single";
                          }
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath != "partner"; },
                      "Single/drama/avoidance paths"},
                     {"Farewell awkwardly", "Drama/poly path: all at once", {0, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships& rels) {
                          std::cout << "LOCATION: Campus Quad\n";
                          std::cout << "You bump into Alex, Sam, and Riley at once.\n";
                          std::cout << "  1) Try to smooth things over\n";
                          std::cout << "  2) Say nothing\n";
                          std::cout << "  3) Leave abruptly\n";
                          int c = promptInt(1, 3);
                          int idx = rand() % static_cast<int>(rels.npcs.size());
                          std::string target = rels.npcs[idx].name;
                          if (c == 1) {
                              rels.interactWith(target, 5);
                              p.adjustSocial(5);
                          } else if (c == 2) {
                              p.adjustSocial(-5);
                              rels.interactWith(target, -10);
                          } else {
                              p.adjustEnergy(20);
                              p.adjustSocial(-10);
                          }
                          st.relationshipPath = "drama";
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "drama"; },
                      "Drama/poly paths only"},
                     {"Farewell quietly", "Avoidance path: reflect alone", {0, 0, 0, 0, 0, 0},
                      [](GameState& st, Player& p, Relationships&) {
                          std::cout << "LOCATION: Dorm Room\n";
                          std::cout << "You avoid everyone and reflect alone.\n";
                          std::cout << "  1) Write in journal\n";
                          std::cout << "  2) Sleep early\n";
                          std::cout << "  3) Watch a movie alone\n";
                          int c = promptInt(1, 3);
                          if (c == 1) {
                              p.adjustAcademic(5);
                              p.adjustEnergy(10);
                          } else if (c == 2) {
                              p.adjustEnergy(25);
                              p.adjustHealth(10);
                          } else {
                              p.adjustEnergy(15);
                              p.adjustSocial(-5);
                          }
                          st.relationshipPath = "avoidance";
                      },
                      [](const GameState& st, const Player&, const Relationships&) { return st.relationshipPath == "avoidance"; },
                      "Avoidance path only"},
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
    if (week == 2 || week == 3 || week == 4 || week == 5 || week == 6 || week == 7) {
        Events::runWeekEndRandomEvents(week, state.relationshipPath, state.partnerName, player, rels);
    } else if (week == 8) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        std::cout << "\nWEEK 8 COMPLETE: \"Summer concludes – the final outcomes of your choices are revealed.\"\n";

        // Path-based wrap-up.
        if (state.relationshipPath == "partner" && !state.partnerName.empty()) {
            std::cout << "[PATH] Partner path: You leave with a strong bond.\n";
            rels.interactWith(state.partnerName, 25);
        } else if (state.relationshipPath == "single") {
            std::cout << "[PATH] Single path: You leave with wide social circles.\n";
            player.adjustSocial(20);
        } else if (state.relationshipPath == "drama") {
            std::cout << "[PATH] Drama/polyamory path: You leave with mixed reputations.\n";
            player.adjustSocial(-10);
            if (!rels.npcs.empty()) {
                std::uniform_int_distribution<int> dist(0, static_cast<int>(rels.npcs.size()) - 1);
                int idx = dist(rng());
                rels.interactWith(rels.npcs[idx].name, -15);
            }
        } else if (state.relationshipPath == "avoidance") {
            std::cout << "[PATH] Avoidance path: You leave rested but isolated.\n";
            player.adjustHealth(15);
            player.adjustSocial(-10);
        }

        // Additional Week 8 random events based on path.
        Events::runWeekEndRandomEvents(week, state.relationshipPath, state.partnerName, player, rels);
    }

    if (week == 6 && state.relationshipPath == "partner" && !state.partnerName.empty()) {
        rels.interactWith(state.partnerName, 15);
        std::cout << "[RELATIONSHIP] Relationship solidifies with " << state.partnerName << ".\n";
    }

    // Additional weekly random events from the legacy event pool.
    auto weeklyEvents = Events::generateWeeklyEvents();
    for (const auto& e : weeklyEvents) {
        if (Events::rollEvent(e)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            std::cout << "\n********** Wait... **********\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            std::cout << "\n********** Something's coming... **********\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            std::cout << "\n********** RANDOM EVENT! **********\n";
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
// endingDescriptionFromStats()
// Returns a thematic ending name and description based on final stats.
std::pair<std::string, std::string> endingDescriptionFromStats(const Player& p)
{
    bool ultraChad =
        p.money   > 2000 &&
        p.fitness > 80   &&
        p.social  > 80   &&
        p.academic > 80  &&
        p.health  > 80;

    bool balanced =
        p.money   > 1000 &&
        p.health  > 60   &&
        p.energy  > 60   &&
        p.social  > 60   &&
        p.academic > 60  &&
        p.fitness > 60;

    bool workaholic =
        p.money   > 3000 &&
        p.social  < 40   &&
        p.fitness < 40;

    bool gymBro =
        p.fitness > 90   &&
        p.academic < 50  &&
        p.money   < 500;

    bool noLife =
        p.academic > 90  &&
        p.social   < 30  &&
        p.fitness  < 30;

    bool socialButterfly =
        p.social  > 90   &&
        p.money   < 300  &&
        p.academic < 50;

    int below40 = 0;
    below40 += (p.health   < 40);
    below40 += (p.energy   < 40);
    below40 += (p.social   < 40);
    below40 += (p.academic < 40);
    below40 += (p.fitness  < 40);
    below40 += (p.money    < 40);
    bool barelySurvived = below40 >= 3;

    if (ultraChad) {
        return {"Ultra Chad Ending",
                "You crushed this summer! Perfect body, perfect grades, loaded wallet, and an amazing social life. You're basically a superhero!"};
    }
    if (balanced) {
        return {"Balanced Ending",
                "Solid summer! You managed to improve in all areas without going overboard. Future you thanks present you."};
    }
    if (workaholic) {
        return {"Workaholic Ending",
                "You're rich but at what cost? Your Tinder matches have dried up and you get winded climbing stairs."};
    }
    if (gymBro) {
        return {"Gym Bro Ending",
                "You're absolutely shredded! Too bad you failed your summer courses and can't afford protein powder anymore."};
    }
    if (noLife) {
        return {"No-life Ending",
                "Straight A's! Your parents are proud but your only friend is the library security guard."};
    }
    if (socialButterfly) {
        return {"Social Butterfly Ending",
                "You're the life of the party! Unfortunately, you partied so much you forgot about real life responsibilities."};
    }
    if (barelySurvived) {
        return {"Barely Survived Ending",
                "You made it through summer but... was this really living? Time to reflect on your life choices."};
    }

    // Default: treat as a middling, mixed summer.
    return {"Mixed Results Ending",
            "You survived the summer with a mix of highs and lows. There's plenty to be proud of—and plenty to improve next time."};
}

// starsFromStats()
// Simple wrapper that derives a 1–5 rating from the named ending.
int starsFromStats(const Player& p)
{
    auto ending = endingDescriptionFromStats(p);
    const std::string& name = ending.first;
    if (name == "Ultra Chad Ending") return 5;
    if (name == "Balanced Ending") return 4;
    if (name == "Workaholic Ending" ||
        name == "Gym Bro Ending" ||
        name == "No-life Ending" ||
        name == "Social Butterfly Ending") return 3;
    if (name == "Barely Survived Ending") return 2;
    return 3; // Mixed results or other custom endings.
}

// runClosingSequence()
// Plays the final cinematic sequence, shows final checks and rating, and clears any save file.
// Input: final GameState, Player, and Relationships.
void runClosingSequence(GameState& state, Player& player, Relationships& rels)
{
    std::cout << "\nCLOSING SEQUENCE\n";
    if (state.gameOver) {
        std::cout << "You could not survive the summer. Better luck next time.\n";
        SaveGame::clear();
        return;
    }

    // Scene setup
    std::cout << "[SCENE: Campus Quad, sunset of the final summer day]\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    std::cout << "The semester is over. Your summer journey has reached its end.\n\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Reflective final check
    std::cout << "[FINAL CHECK]\n";
    std::cout << "• Did you survive all 8 weeks without hitting critical warnings?\n";
    std::cout << "• Did you balance your stats and relationships?\n";
    std::cout << "• Did you achieve your chosen path's ending?\n\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Results header
    std::cout << "[RESULTS APPEAR]\n\n";
    std::cout << "\"WELCOME TO SUMMER MAXXING — FINAL REPORT!\"\n\n";

    // Final performance summary
    std::cout << "YOUR PERFORMANCE:\n";
    std::cout << "• ⚡ ENERGY: "   << player.energy   << "\n";
    std::cout << "• ❤️ HEALTH: "   << player.health   << "\n";
    std::cout << "• 👥 SOCIAL: "   << player.social   << "\n";
    std::cout << "• 📚 ACADEMIC: " << player.academic << "\n";
    std::cout << "• 💪 FITNESS: "  << player.fitness  << "\n";
    std::cout << "• 💰 MONEY: "    << player.money    << "\n";
    std::cout << "• RELATIONSHIPS:\n";
    printRelationships(rels);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Pass / fail conditions
    std::cout << "\nPASS / FAIL CONDITIONS:\n";
    if (player.health <= 0) {
        std::cout << "🚨 You collapsed from illness. GAME OVER.\n";
        SaveGame::clear();
        return;
    }
    if (player.energy <= 0) {
        std::cout << "😴 You collapsed from exhaustion. GAME OVER.\n";
        SaveGame::clear();
        return;
    }
    if (player.money <= 0) {
        std::cout << "💸 Financial crisis hit. GAME OVER.\n";
        SaveGame::clear();
        return;
    }

    std::cout << "✅ You survived the summer!\n\n";

    auto ending1 = endingDescriptionFromStats(player);
    std::cout << "ENDING: " << ending1.first << "\n";
    std::cout << ending1.second << "\n\n";

    int stars = starsFromStats(player);

    std::cout << "ENDING RATING (Stars out of 5): " << stars << "/5\n";
    if (stars == 5) {
        std::cout << "5/5 \"Legendary Summer!\" — You mastered balance, thrived socially, and left with strong bonds.\n";
    } else if (stars == 4) {
        std::cout << "4/5 \"Great Summer!\" — You did well, with only minor struggles.\n";
    } else if (stars == 3) {
        std::cout << "3/5 \"Average Summer.\" — You survived, but missed key opportunities.\n";
    } else if (stars == 2) {
        std::cout << "2/5 \"Rough Summer.\" — You barely scraped by, stats suffered.\n";
    } else {
        std::cout << "1/5 \"Disaster Summer.\" — You failed to balance life, relationships, or finances.\n";
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "\nThank you for playing SUMMER MAXXING!\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "Your choices shaped your destiny. Every summer tells a different story.\n";
    std::cout << "[Press Enter to exit...]\n";
    waitForEnter();

    // Game completed successfully: clear any existing save file.
    SaveGame::clear();
}

// runGame()
// Main game loop: handles difficulty/load selection, iterates weeks/scenarios, and prints the final ending.
// Input: none. Output: runs the interactive session and prints results.
void runGame()
{
    Player player;
    Relationships rels;
    GameState state;
    std::cout << std::endl;
    std::cout << "------------------------------------------------------\n";
    std::cout << "*** For the best user experience, it is recommended to play in FullScreen.***\n";
    std::cout << "------------------------------------------------------\n";


    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    std::cout << "------------------------------------------------------\n";
    std::cout << "____  _     _      _      _____ ____    \n";
    std::cout << "/ ___\\/ \\\\ /\\/ \\\\__/|/ \\\\__/|/  __//  __\\\\   \n";
    std::cout << "|    \\\\| | ||| |\\\\/||| |\\\\/|||  \\\\  |  \\\\/|   \n";
    std::cout << "\\\\___ || \\\\_/|| |  ||| |  |||  /_ |    /   \n";
    std::cout << "\\\\____/\\\\____/\\\\_/  \\\\|\\\\_/  \\\\|\\\\____\\\\\\\\_/\\\\_\\\\   \n";
    std::cout << "                                         \n";
    std::cout << " _      ____ ___  ____  _ _  _      _____\n";
    std::cout << "/ \\\\__/|/  _ \\\\  \\\\//\\\\  \\\\/// \\\\/ \\\\  /|/  __/\n";
    std::cout << "| |\\\\||| / \\\\| \\\\  /  \\\\  / | || |\\\\ ||| |  _\n";
    std::cout << "| |  ||| |-|| /  \\\\  /  \\\\ | || | \\\\||| |_//\n";
    std::cout << "\\\\_/  \\\\|\\\\_/ \\\\|/__/\\\\/__/\\\\\\\\_/\\\\_/  \\\\|\\\\____\\\\\n";
    std::cout << "                                            \n";
    std::cout << "          |\n";
    std::cout << "        \\\\ _ /\n";
    std::cout << "      -= (_) =-\n";
    std::cout << "        /   \\\\         _\\\\/_\n";
    std::cout << "          |           //o\\\\  _\\\\/_\n";
    std::cout << "   _____ _ __ __ ____ _ | __/o\\\\\\\\ _\n";
    std::cout << " =-=-_-__=_-= _=_=-=_,-'|\"'\"\"-|-,_\n";
    std::cout << "  =- _=-=- -_=-=_,-\"          |=- =- \n";
    std::cout << "-=.--\"\n";
    std::cout << "------------------------------------------------------\n\n";
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << "[SCENE: Your dorm room, final exam week]\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << "Another semester down! Time to make this summer COUNT.\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << "Your goal: Survive 8 weeks while balancing relationships, fitness, academics, and finances.\n\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));


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
        std::cout << "\n[DIFFICULTY SELECTION]\n";
        std::cout << "► Trust Fund Kid (Easy) - \"Life on easy mode, just don't screw up\"\n";
        std::cout << "► Average Student (Medium) - \"Typical college student struggles\"\n";
        std::cout << "► Struggling Immigrant (Hard) - \"Got a family back home to provide for so you better work hard buddy\"\n";
        diff = chooseDifficulty(player, difficultyIndex);
    }

    std::cout << "\n\"WELCOME TO SUMMER MAXXING!\"\n\n";

    std::cout << "YOUR MISSION:\n";
    std::cout << "Survive 8 weeks of summer while balancing your stats and relationships.\n";
    std::cout << "Make smart choices to achieve your ideal summer ending!\n\n";

    std::cout << "- KEY STATS TO MANAGE:\n";
    std::cout << "- ENERGY (0-100) - Required for all activities\n";
    std::cout << "- HEALTH (0-100) - Affects everything you do\n";
    std::cout << "- SOCIAL (0-100) - Friendships and relationships\n";
    std::cout << "- ACADEMIC (0-100) - Summer courses and learning\n";
    std::cout << "- FITNESS (0-100) - Physical health and appearance\n";
    std::cout << "- MONEY ($0-5000) - Campus life isn't free!\n\n";

    std::cout << "CRITICAL WARNINGS:\n";
    std::cout << "- If HEALTH <= 10: Medical emergency! Forced hospital visit\n";
    std::cout << "- If ENERGY <= 10: Collapse from exhaustion! Forced sleep\n";
    std::cout << "- If MONEY <= 0: Financial crisis! GAME OVER\n\n";

    std::cout << "RELATIONSHIP SYSTEM:\n";
    std::cout << "• Meet Alex (Gym), Sam (Library), Riley (Cafe), Jordan (Online)\n";
    std::cout << "• Build affinity through interactions\n";
    std::cout << "• Relationships unlock special opportunities and storylines\n\n";

    std::cout << "HOW TO PLAY:\n";
    std::cout << "• The summer has 8 weeks\n";
    std::cout << "• Each week has 4 scenarios (like time slots)\n";
    std::cout << "• Choose activities that balance your stats\n";
    std::cout << "• Random events will spice things up\n\n";

    std::cout << "TIPS FOR SUCCESS:\n";
    std::cout << "• Don't neglect any stat for too long\n";
    std::cout << "• Balance work, rest, and social life\n";
    std::cout << "• Build relationships consistently\n";
    std::cout << "• Watch your Energy levels\n";
    std::cout << "• Save money for emergencies\n\n";

    std::cout << "READY TO MAX YOUR SUMMER?\n";
    waitForEnter();

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
                    std::cout << "  " << availableIndices.size() << ") " << c.title << " - " << c.detail << "\n";
                } else {
                    std::cout << "  - " << c.title << " [locked: " << c.lockReason << "]\n";
                }
            }

            if (availableIndices.empty()) {
                std::cout << "No available actions. You rest instead.\n";
                player.adjustEnergy(15);
                continue;
            }

            int selection = promptInt(1, static_cast<int>(availableIndices.size()));
            const Choice& chosen = sc.choices[availableIndices[selection - 1]];
            Player beforeChoice = player;
            Relationships beforeRels = rels;
            applyEffect(player, rels, chosen.effect, diff);
            if (chosen.special) {
                chosen.special(state, player, rels);
            }

            enforceCriticalRules(state, player);
            player.clampStats();
            printStatChanges(beforeChoice, player, beforeRels, rels);

            // Week 1 special random events happen between scenarios.
            if (week == 1) {
                Events::maybeRunWeek1RandomEvent(
                    slot,
                    state.currentWeek,
                    state.week1FriendEventDone,
                    state.week1RouterEventDone,
                    state.week1EveningEventDone,
                    player
                );
            } else if (week == 5 && slot == 2) {
                // Week 5 special random social event between scenarios 2 and 3.
                Events::week5MidweekSocialSpice(player, rels);
            }
        }

        if (!state.gameOver) {
            applyWeekEnd(week, state, player, rels, diff);

            // Simple save option at the end of each week.
            std::cout << "Save and quit? (y/n)\n";
            char saveChoice = 'n';
            if (std::cin >> saveChoice) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (saveChoice == 'y' || saveChoice == 'Y') {
                    // Save the *next* week to play so loading resumes after the last completed week.
                    int resumeWeek = std::min(week + 1, TOTAL_WEEKS);
                    if (SaveGame::save(player, resumeWeek, difficultyIndex)) {
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

    runClosingSequence(state, player, rels);
    return;

    std::cout << "\nCLOSING SEQUENCE\n";
    if (state.gameOver) {
        std::cout << "You could not survive the summer. Better luck next time.\n";
        SaveGame::clear();
        return;
    }

    std::cout << "[SCENE: Campus Quad, sunset of the final summer day]\n";
    std::cout << "The semester is over. Your summer journey has reached its end.\n\n";

    std::cout << "\"WELCOME TO SUMMER MAXXING — FINAL REPORT!\"\n";

    player.printStats();
    printRelationships(rels);

    // Pass / fail conditions.
    if (player.health <= 0) {
        std::cout << "You collapsed from illness. GAME OVER.\n";
        SaveGame::clear();
        return;
    }
    if (player.energy <= 0) {
        std::cout << "You collapsed from exhaustion. GAME OVER.\n";
        SaveGame::clear();
        return;
    }
    if (player.money <= 0) {
        std::cout << "Financial crisis hit. GAME OVER.\n";
        SaveGame::clear();
        return;
    }

    std::cout << "You survived the summer!\n";

    auto ending2 = endingDescriptionFromStats(player);
    std::cout << "ENDING: " << ending2.first << "\n";
    std::cout << ending2.second << "\n";

    int stars = starsFromStats(player);

    std::cout << "ENDING RATING (Stars out of 5): " << stars << "/5\n";
    if (stars == 5) {
        std::cout << "Legendary Summer! You mastered balance, thrived socially, and left with strong bonds.\n";
    } else if (stars == 4) {
        std::cout << "Great Summer! You did well, with only minor struggles.\n";
    } else if (stars == 3) {
        std::cout << "Average Summer. You survived, but missed key opportunities.\n";
    } else if (stars == 2) {
        std::cout << "Rough Summer. You barely scraped by, and your stats suffered.\n";
    } else {
        std::cout << "Disaster Summer. You failed to balance life, relationships, or finances.\n";
    }

    // Game completed successfully: clear any existing save file.
    SaveGame::clear();
}

int main()
{
    runGame();
    return 0;
}
