#include "events.h"
#include "player.h"
#include "relationships.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

// Forward declaration for the shared input helper defined in main.cpp.
int promptInt(int min, int max);

static bool seed_done = [](){ srand((unsigned)time(nullptr)); return true; }();

namespace {

// Local probability roll helper for events.
// Returns true if a random number between 0 and 1 is less than the given probability.
bool rollProbability(double probability)
{
    double r = static_cast<double>(rand()) / RAND_MAX;
    return r < probability;
}

// Helper to add a blank line before and after event text.
// This struct ensures consistent spacing around event messages.
struct EventSpacing {
    EventSpacing()  { std::cout << std::endl; }
    ~EventSpacing() { std::cout << std::endl; }
};

} // anonymous namespace

namespace Events {

// Generates a list of predefined weekly events with their probabilities and effects.
// Each event contains a description, probability, and a lambda function for applying effects to the player.
std::vector<Event> generateWeeklyEvents()
{
    std::vector<Event> out;

    out.push_back({"YOU RECEIVE AN INHERITANCE!!!", "A distant relative leaves you some money.", 0.05,
                   [](Player &p){ p.adjustMoney(500); }});

    out.push_back({"GYM SALE!!!!", "Gym equipment is cheaper this week.", 0.10,
                   [](Player &p){ /* could set a flag or reduce costs */ p.adjustSocial(0); }});

    out.push_back({"STUDY BREAKTHROUGH!!!", "You finally understand a tough concept.", 0.15,
                   [](Player &p){ p.adjustAcademic(20); }});

    out.push_back({"SPORTS INJURY!!!", "You injure yourself during exercise.", 0.05,
                   [](Player &p){ p.adjustHealth(-20); p.adjustFitness(-10); p.adjustMoney(-100); }});

    return out;
}

// Rolls to determine if a specific event occurs based on its probability.
bool rollEvent(const Event &e)
{
    return rollProbability(e.probability);
}

// Handles random events specific to Week 1.
// Includes friend phone call, router fried, and evening mini-choice events.
void maybeRunWeek1RandomEvent(
    int slot,
    int currentWeek,
    bool &friendEventDone,
    bool &routerEventDone,
    bool &eveningEventDone,
    Player &player
)
{
    (void)slot; // currently unused but kept for extensibility

    if (currentWeek != 1) return;

    // Friend phone call event (once per week).
    if (!friendEventDone && rollProbability(0.20)) {
        EventSpacing spacing;
        std::cout << "\n[RANDOM EVENT] Your phone rings - it's an old high school friend!\n";
        std::cout << "FRIEND: \"Hey! I'm in town for the weekend and heard you're staying on campus. Want to catch up tomorrow?\"\n";
        std::cout << "  1) Accept the invite\n";
        std::cout << "  2) Decline and stay focused\n";
        int choice = promptInt(1, 2);
        if (choice == 1) {
            player.adjustSocial(10);
            player.adjustMoney(-10);
            player.adjustAcademic(-3);
        } else {
            player.adjustSocial(-3);
        }
        friendEventDone = true;
        player.clampStats();
        return;
    }

    // Router fried event (once per week).
    if (!routerEventDone && rollProbability(0.15)) {
        EventSpacing spacing;
        std::cout << "\n[RANDOM EVENT] Power surge fries your router! No internet for the night.\n";
        std::cout << "Online activities feel impossible and you lose some focus.\n";
        player.adjustAcademic(-5);
        player.adjustEnergy(-5);
        routerEventDone = true;
        player.clampStats();
        return;
    }

    // Neutral evening mini-choice (once per week).
    if (!eveningEventDone && rollProbability(0.15)) {
        EventSpacing spacing;
        std::cout << "\n[RANDOM EVENT] Evening approaches. Your phone buzzes with group chat notifications.\n";
        std::cout << "You decide to...\n";
        std::cout << "  1) Play video games\n";
        std::cout << "  2) Study for your summer course\n";
        std::cout << "  3) Go to bed early\n";
        int choice = promptInt(1, 3);
        if (choice == 1) {
            player.adjustEnergy(-10);
            player.adjustSocial(5);
            player.adjustAcademic(-2);
        } else if (choice == 2) {
            player.adjustEnergy(-20);
            player.adjustAcademic(15);
        } else {
            player.adjustEnergy(35);
        }
        eveningEventDone = true;
        player.clampStats();
    }
}

// Handles random events specific to the end of each week.
// Includes special events for each week and their associated effects on the player and relationships.
void runWeekEndRandomEvents(
    int week,
    const std::string &relationshipPath,
    const std::string &partnerName,
    Player &player,
    Relationships &rels
)
{
    // Week 2: Campus-wide event.
    if (week == 2) {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        EventSpacing spacing;
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
        return;
    }

    // Week 3: GPA results.
    if (week == 3) {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        EventSpacing spacing;
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
        return;
    }

    // Week 4: Health crisis.
    if (week == 4) {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        EventSpacing spacing;
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
        return;
    }

    // Week 5: End-of-week bonuses and opportunities.
    if (week == 5) {
        // Random Event 1 - Positive (25% chance): Summer job bonus.
        if (rollProbability(0.25)) {
            EventSpacing spacing;
            std::cout << "\n[WEEK 5 BONUS] SUMMER JOB BONUS!\n";
            player.adjustMoney(200);
            player.adjustSocial(3);
        }

        // Random Event 2 - Relationship Boost (30% chance): Deepening friendship.
        if (rollProbability(0.30) && !rels.npcs.empty()) {
            EventSpacing spacing;
            int bestIndex = 0;
            int bestAffinity = rels.npcs[0].affinity;
            for (size_t i = 1; i < rels.npcs.size(); ++i) {
                if (rels.npcs[i].affinity > bestAffinity) {
                    bestAffinity = rels.npcs[i].affinity;
                    bestIndex = static_cast<int>(i);
                }
            }
            std::cout << "\n[WEEK 5 BONUS] DEEPENING FRIENDSHIP!\n";
            std::cout << rels.npcs[bestIndex].name << " feels especially close to you after this week.\n";
            rels.interactWith(rels.npcs[bestIndex].name, 10);
        }

        // Random Event 3 - Money Opportunity (20% chance): Skill-based freelance work.
        if (rollProbability(0.20)) {
            EventSpacing spacing;
            std::cout << "\n[WEEK 5 BONUS] SKILL-BASED FREELANCE WORK!\n";
            player.adjustMoney(100);
            std::cout << "Your specialized skills bring in extra income.\n";
        }

        return;
    }

    // Week 6: Emotional exhaustion and drama outcomes.
    if (week == 6) {
        // Random Event 1 - Negative: Fell ill.
        if (rollProbability(0.25)) {
            EventSpacing spacing;
            std::cout << "\n[WEEK 6 EVENT] FELL ILL!\n";
            player.adjustHealth(-30);
            player.adjustMoney(-120);
        }

        // Random Event 2 - Money Opportunity (30% chance): Drama-free freelance work.
        if (rollProbability(0.30)) {
            EventSpacing spacing;
            std::cout << "\n[WEEK 6 EVENT] DRAMA-FREE FREELANCE WORK!\n";
            player.adjustMoney(80);
            player.adjustEnergy(-15);
            std::cout << "You take a job that has nothing to do with your love life.\n";
        }

        return;
    }

    // Week 7: Path-based random relationship events.
    if (week == 7) {
        if (relationshipPath == "partner" && !partnerName.empty()) {
            // Random Event 1 - Positive, Partner Path: Couple spotlight on campus.
            if (rollProbability(0.30)) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 7 EVENT] COUPLE SPOTLIGHT ON CAMPUS!\n";
                std::cout << "People admire your relationship, boosting your confidence together.\n";
                player.adjustSocial(10);
                rels.interactWith(partnerName, 15);
            }

            // Random Event 5 - Neutral, Partner Path: Minor argument with partner.
            if (rollProbability(0.25)) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 7 EVENT] MINOR ARGUMENT WITH PARTNER.\n";
                std::cout << "1. Resolve quickly\n";
                std::cout << "2. Ignore it\n";
                int choice = promptInt(1, 2);
                if (choice == 1) {
                    rels.interactWith(partnerName, 5);
                    player.adjustSocial(5);
                } else {
                    rels.interactWith(partnerName, -10);
                    player.adjustEnergy(10);
                }
            }
        } else if (relationshipPath == "single") {
            // Random Event 2 - Neutral, Single Path: New friend group invite.
            if (rollProbability(0.30) && !rels.npcs.empty()) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 7 EVENT] NEW FRIEND GROUP INVITE.\n";
                std::cout << "A new friend group invites you to hang out.\n";
                std::cout << "1. Join them \n";
                std::cout << "2. Decline\n";
                int choice = promptInt(1, 2);
                if (choice == 1) {
                    player.adjustSocial(12);
                    player.adjustEnergy(-15);
                    int idx = rand() % static_cast<int>(rels.npcs.size());
                    rels.interactWith(rels.npcs[idx].name, 8);
                } else {
                    player.adjustSocial(-5);
                    player.adjustEnergy(10);
                }
            }

            // Random Event 6 - Positive, Single Path: Free entry to a club night.
            if (rollProbability(0.30)) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 7 EVENT] FREE ENTRY TO A CLUB NIGHT!\n";
                player.adjustSocial(15);
                player.adjustMoney(20);
                player.adjustEnergy(-15);
                std::cout << "Single perks — you get invited everywhere.\n";
            }
        } else if (relationshipPath == "drama") {
            // Random Event 3 - Negative, Drama Path: Rumor escalates.
            if (rollProbability(0.30) && !rels.npcs.empty()) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 7 EVENT] RUMOR ESCALATES!\n";
                player.adjustSocial(-10);
                player.adjustEnergy(-20);
                int idx = rand() % static_cast<int>(rels.npcs.size());
                rels.interactWith(rels.npcs[idx].name, -15);
                std::cout << "The gossip spreads further, making it harder to focus.\n";
            }
        } else if (relationshipPath == "avoidance") {
            // Random Event 4 - Positive, Avoidance Path: Peaceful campus walk.
            if (rollProbability(0.30)) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 7 EVENT] PEACEFUL CAMPUS WALK.\n";
                player.adjustHealth(10);
                player.adjustEnergy(20);
                std::cout << "Your solitude pays off with clarity and calm.\n";
            }

            // Random Event 7 - Negative, Avoidance Path: Feeling left out.
            if (rollProbability(0.25)) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 7 EVENT] FEELING LEFT OUT.\n";
                player.adjustSocial(-10);
                player.adjustHealth(-5);
                std::cout << "Isolation starts to weigh on you emotionally.\n";
            }
        }

        return;
    }

    // Week 8: Path-based extra random events.
    if (week == 8) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        if (relationshipPath == "partner" && !partnerName.empty()) {
            // Random Event 1 - Positive, Partner Path, 30% chance.
            if (rollProbability(0.30)) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 8 EVENT] PARTNER SURPRISES YOU WITH A GIFT!\n";
                rels.interactWith(partnerName, 20);
                player.adjustSocial(10);
                std::cout << "A thoughtful gesture strengthens your bond right before summer ends.\n";
            }
            // Random Event 5 - Neutral, Partner Path, 25% chance.
            if (rollProbability(0.25)) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 8 EVENT] PARTNER WANTS TO TALK ABOUT THE FUTURE.\n";
                std::cout << "1. Engage with the conversation\n";
                std::cout << "2. Dodge the topic\n";
                int choice = promptInt(1, 2);
                if (choice == 1) {
                    rels.interactWith(partnerName, 15);
                    player.adjustAcademic(5);
                } else {
                    rels.interactWith(partnerName, -10);
                    player.adjustEnergy(10);
                }
            }
        } else if (relationshipPath == "single") {
            // Random Event 2 - Neutral, Single Path, 25% chance.
            if (rollProbability(0.25)) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 8 EVENT] LAST-MINUTE FRIEND TRIP INVITE.\n";
                std::cout << "1. Join the trip\n";
                std::cout << "2. Decline and rest\n";
                int choice = promptInt(1, 2);
                if (choice == 1) {
                    player.adjustSocial(15);
                    player.adjustMoney(-40);
                    player.adjustEnergy(-20);
                } else {
                    player.adjustSocial(-5);
                    player.adjustEnergy(15);
                }
            }
            // Random Event 6 - Positive, Single Path, 30% chance.
            if (rollProbability(0.30)) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 8 EVENT] FREE CONCERT TICKETS!\n";
                player.adjustSocial(20);
                player.adjustEnergy(-15);
                player.adjustMoney(30);
                std::cout << "You and your friends enjoy one last big night out.\n";
            }
        } else if (relationshipPath == "drama") {
            // Random Event 3 - Negative, Drama/Polyamory Path, 20% chance.
            if (rollProbability(0.20) && !rels.npcs.empty()) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 8 EVENT] CONFRONTATION AT THE PARTY!\n";
                player.adjustSocial(-10);
                player.adjustEnergy(-25);
                int idx = rand() % static_cast<int>(rels.npcs.size());
                rels.interactWith(rels.npcs[idx].name, -20);
                std::cout << "Tensions boil over in front of everyone.\n";
            }
        } else if (relationshipPath == "avoidance") {
            // Random Event 4 - Positive, Avoidance Path, 30% chance.
            if (rollProbability(0.30)) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 8 EVENT] PEACEFUL STUDY NIGHT.\n";
                player.adjustAcademic(10);
                player.adjustEnergy(20);
                std::cout << "Your solitude pays off with quiet productivity.\n";
            }
            // Random Event 7 - Negative, Avoidance Path, 20% chance.
            if (rollProbability(0.20)) {
                EventSpacing spacing;
                std::cout << "\n[WEEK 8 EVENT] FEELING REGRET.\n";
                player.adjustSocial(-10);
                player.adjustHealth(-5);
                std::cout << "You wonder if avoiding everyone was the right choice.\n";
            }
        }
    }
}

// NEW EVENTS FROM GAME PLAN

void mysteryPackageArrives(Player &player)
{
    if (!rollProbability(0.25)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] 📦 MYSTERY PACKAGE ARRIVES 📦\n";
    std::cout << "There's a battered cardboard box at your door. No return address.\n";
    std::cout << "Inside: homemade cookies, your favorite snacks, and a handwritten note.\n";
    std::cout << "Tucked in the corner: a $20 bill \"for emergencies.\"\n";
    player.adjustHealth(15);
    player.adjustSocial(8);
    player.adjustMoney(20);
    player.adjustEnergy(5);
}

void academicMeltdownImminent(Player &player)
{
    if (!rollProbability(0.20)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] 🔥 ACADEMIC MELTDOWN IMMINENT 🔥\n";
    std::cout << "You open the summer course portal and your blood runs cold.\n";
    std::cout << "\"FINAL RESEARCH PAPER: 5,000 words due: TOMORROW.\"\n";
    std::cout << "  1) Pull all-nighter\n";
    std::cout << "  2) Beg for extension\n";
    std::cout << "  3) Accept failure\n";
    int choice = promptInt(1, 3);
    if (choice == 1) {
        player.adjustAcademic(20);
        player.adjustEnergy(-40);
        player.adjustHealth(-10);
    } else if (choice == 2) {
        player.adjustSocial(-5);
    } else {
        player.adjustAcademic(-15);
        player.adjustEnergy(20);
    }
}

void campusWifiUpgrade(Player &player)
{
    if (!rollProbability(0.15)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] Campus wifi upgrade in progress\n";
    std::cout << "Online activities unavailable for the next scenario.\n";
    player.adjustAcademic(-3);
    player.adjustSocial(-2);
}

void academicMiracleStrikes(Player &player)
{
    if (!rollProbability(0.20)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] 🎓 ACADEMIC MIRACLE STRIKES 🎓\n";
    std::cout << "EMAIL NOTIFICATION: \"URGENT: Financial Aid Office Update\"\n";
    std::cout << "CONGRATULATIONS! Due to outstanding academic performance, you've been awarded an unexpected refund.\n";
    player.adjustMoney(200);
    player.adjustAcademic(10);
    player.adjustEnergy(15);
}

void desperateClientAlert(Player &player)
{
    if (!rollProbability(0.25)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] 💻 DESPERATE CLIENT ALERT 💻\n";
    std::cout << "DISCORD NOTIFICATION: \"HELP! Need someone to fix my website ASAP! $100 for 2 hours work?\"\n";
    std::cout << "  1) Take the gig - how hard can it be?\n";
    std::cout << "  2) Negotiate higher rate\n";
    std::cout << "  3) Run away screaming\n";
    int choice = promptInt(1, 3);
    
    if (choice == 1) {
        if (rollProbability(0.40)) {
            std::cout << "SUCCESS! Client is ecstatic! 'You're a genius!'\n";
            player.adjustMoney(120);
        } else {
            std::cout << "DISASTER! Client hates it. 'This isn't what I envisioned!'\n";
            player.adjustMoney(0);
        }
        player.adjustEnergy(-25);
        player.adjustSocial(-5);
    } else if (choice == 2) {
        if (player.social > 60) {
            std::cout << "You talk them up to $200! The art of the deal!\n";
            player.adjustMoney(200);
        } else {
            std::cout << "They found someone cheaper on Fiverr. Client ghosts you.\n";
        }
        player.adjustEnergy(-30);
    } else {
        std::cout << "Some money isn't worth the mental health cost.\n";
        player.adjustEnergy(10);
    }
}

void shadyOpportunity(Player &player)
{
    if (!rollProbability(0.20)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] 🕶️ SHADY OPPORTUNITY ARISES 🕶️\n";
    std::cout << "A mysterious student approaches you in the library stacks.\n";
    std::cout << "\"Hey... I hear you're good with computers. I need someone to help with a 'special project'. Pays $300 cash. No questions asked.\"\n";
    std::cout << "  1) I'm listening...\n";
    std::cout << "  2) Not interested\n";
    std::cout << "  3) Report to authorities\n";
    int choice = promptInt(1, 3);
    
    if (choice == 1) {
        double r = static_cast<double>(rand()) / RAND_MAX;
        if (r < 0.30) {
            std::cout << "BIG SCORE! The 'project' was totally legal... probably.\n";
            player.adjustMoney(300);
            // Bad Karma represented as social penalty
            player.adjustSocial(-5);
        } else if (r < 0.80) {
            std::cout << "SCAMMED! You got played. The 'client' disappeared.\n";
            player.adjustMoney(-50);
            player.adjustEnergy(-20);
        } else {
            std::cout << "CAUGHT! Campus security 'had a talk' with you.\n";
            player.adjustSocial(-15);
            player.adjustAcademic(-10);
        }
    } else if (choice == 2) {
        std::cout << "You sleep well knowing you avoided potential felonies.\n";
        player.adjustEnergy(5);
    } else {
        std::cout << "You report the student. You feel responsible.\n";
        player.adjustSocial(3);
    }
}

void unexpectedInheritance(Player &player)
{
    if (!rollProbability(0.20)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] UNEXPECTED INHERITANCE FROM RELATIVE!\n";
    std::cout << "You receive $200 from a relative you barely remember.\n";
    player.adjustMoney(200);
    player.adjustSocial(5);
}

void urgentFreelanceGig(Player &player)
{
    if (!rollProbability(0.25)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] URGENT FREELANCE GIG - HIGH PAY!\n";
    std::cout << "Quick freelance work pays $75 but requires immediate attention.\n";
    player.adjustMoney(75);
    player.adjustEnergy(-25);
}

void constructionNoiseAdvisory(Player &player)
{
    if (!rollProbability(0.30)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] 🌋 UNEXPECTED CAMPUS CRISIS 🌋\n";
    std::cout << "CONSTRUCTION NOISE ADVISORY: Renovations starting at University Center.\n";
    std::cout << "Expected duration: 2 weeks. Earplugs recommended.\n";
    std::cout << "The noise is unbearable and affects your focus.\n";
    player.adjustEnergy(-5);
    player.adjustSocial(2); // Shared suffering brings people together
    // All stat gains reduced by 3 for next scenario (would need to be handled in scenario logic)
}

void printerBreakdown(Player &player, Relationships &rels)
{
    if (!rollProbability(0.25)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] 📚 PRINTER BREAKDOWN 📚\n";
    std::cout << "The library printer is having a meltdown. Sam looks frustrated.\n";
    std::cout << "SAM: \"Do you know what's going on with this thing today? I have 50 pages due in an hour!\"\n";
    std::cout << "  1) Let me help fix it\n";
    std::cout << "  2) Sorry, can't help\n";
    int choice = promptInt(1, 2);
    if (choice == 1) {
        player.adjustEnergy(-15);
        rels.interactWith("Sam (Study Buddy)", 7);
        player.adjustAcademic(5);
    } else {
        rels.interactWith("Sam (Study Buddy)", -3);
        player.adjustAcademic(10);
    }
}

void surpriseGymEvent(Player &player)
{
    if (!rollProbability(0.20)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] 🎉 SURPRISE GYM EVENT! 🎉\n";
    std::cout << "The gym manager approaches: \"We're doing a free personal training session giveaway for our most dedicated members... and YOU just won!\"\n";
    player.adjustFitness(10);
    player.adjustHealth(5);
}

void nearDisasterStrikes(Player &player)
{
    if (!rollProbability(0.15)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] 💀 NEAR-DISASTER STRIKES 💀\n";
    std::cout << "Your grip slips! The barbell tumbles toward your chest...\n";
    std::cout << "Alex's reflexes save you by centimeters.\n";
    std::cout << "\"Maybe we should stick to lighter weights today,\" Alex says, looking pale.\n";
    player.adjustHealth(-5);
    player.adjustEnergy(-10);
}

void studyStruggles(Player &player)
{
    if (!rollProbability(0.25)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] 📖 STUDY STRUGGLES 📖\n";
    std::cout << "The library is PACKED. Every seat taken, every outlet occupied.\n";
    std::cout << "The summer students have emerged from hibernation.\n";
    player.adjustAcademic(-7);
    player.adjustEnergy(-5);
}

void napTrap(Player &player)
{
    if (!rollProbability(0.20)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] 😪 THE NAP TRAP 😪\n";
    std::cout << "\"Just 20 minutes,\" you tell yourself...\n";
    std::cout << "*3 hours later* You wake up disoriented, drooling on your textbook.\n";
    player.adjustEnergy(15); // Instead of full 25
    player.adjustAcademic(-3);
}

void legendaryGamingDrop(Player &player, Relationships &rels)
{
    if (!rollProbability(0.15)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] 🏆 LEGENDARY DROP! 🏆\n";
    std::cout << "THE UNTHINKABLE HAPPENS! After months of farming, the ultra-rare item drops for your team.\n";
    std::cout << "Jordan screams in your ear: \"NO WAY! THAT'S A 0.001% DROP!\"\n";
    player.adjustEnergy(10);
    player.adjustSocial(5);
    rels.interactWith("Jordan (Gamer)", 10);
}

void noisyNeighbors(Player &player)
{
    if (!rollProbability(0.20)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] Neighbors are having a loud party next door!\n";
    std::cout << "Concentration broken, study efficiency reduced.\n";
    player.adjustEnergy(-10);
    player.adjustAcademic(-5);
}

void perfectSleep(Player &player)
{
    if (!rollProbability(0.25)) return;
    EventSpacing spacing;
    std::cout << "\n[RANDOM EVENT] Your body thanks you with perfect sleep!\n";
    player.adjustEnergy(10); // Bonus
    player.adjustHealth(5);
    player.adjustAcademic(1);
    player.adjustFitness(1);
    player.adjustSocial(1);
}

// Week 4 specific events
void week4GymOvertraining(Player &player)
{
    if (!rollProbability(0.40)) return;
    EventSpacing spacing;
    std::cout << "OVERTRAINING HITS HARD! Your body rebels against the abuse.\n";
    player.adjustHealth(-15);
    player.adjustEnergy(-25);
    player.adjustFitness(-5);
}

void week4StudyBreakthrough(Player &player)
{
    if (!rollProbability(0.50)) return;
    EventSpacing spacing;
    std::cout << "STUDY BREAKTHROUGH! Everything finally clicks into place.\n";
    player.adjustAcademic(15);
    player.adjustEnergy(10);
}

void week4ForcedWorkoutInjury(Player &player)
{
    if (!rollProbability(0.20)) return;
    EventSpacing spacing;
    std::cout << "MAJOR INJURY! Medical bill hits.\n";
    player.adjustHealth(-25);
    player.adjustFitness(-15);
    player.adjustEnergy(-30);
    player.adjustMoney(-140);
}

void week4MoneyDesperationRobbery(Player &player)
{
    if (!rollProbability(0.50)) return;
    EventSpacing spacing;
    std::cout << "Sketchy gig backfires. You get robbed.\n";
    player.adjustMoney(-120);
}

void week4FinalWorkoutPotion(Player &player)
{
    if (!rollProbability(0.20)) return;

    EventSpacing spacing;
    std::cout << "MAGIC POTION: increase your health and fitness by 20 for only $50!\n";
    std::cout << "  1) Buy protein powder\n";
    std::cout << "  2) Nah, magic potions are fake\n";
    int choice = promptInt(1, 2);
    if (choice == 1) {
        if (player.money >= 50) {
            player.adjustMoney(-50);
            player.adjustFitness(20);
            player.adjustHealth(20);
            std::cout << "You invest in the \"magic\" powder and feel a noticeable boost.\n";
        } else {
            std::cout << "You can't actually afford it. Maybe next time.\n";
        }
    } else {
        std::cout << "You decide it's not worth the money.\n";
    }
}

void week4LastMinuteStudyBurnout(Player &player)
{
    if (!rollProbability(0.35)) return;
    EventSpacing spacing;
    std::cout << "Mental burnout.\n";
    player.adjustAcademic(-10);
    player.adjustEnergy(-20);
    player.adjustHealth(-5);
}

void week4SocialRecoveryParty(Player &player)
{
    if (!rollProbability(0.30)) return;
    EventSpacing spacing;
    std::cout << "UNEXPECTED CAMPUS PARTY! Sometimes you need to let loose.\n";
    player.adjustSocial(15);
    player.adjustEnergy(-20);
    player.adjustAcademic(-10);
}

void week4CompleteCollapseSick(Player &player)
{
    if (!rollProbability(0.25)) return;
    EventSpacing spacing;
    std::cout << "WAKE UP SICK ANYWAY! The damage was already done.\n";
    player.adjustHealth(-10);
    player.adjustEnergy(-15);
}

void week4EmergencyMeasuresRemorse(Player &player)
{
    if (!rollProbability(0.40)) return;
    EventSpacing spacing;
    std::cout << "BUYER'S REMORSE! That money could have been saved.\n";
    player.adjustEnergy(-2);
    player.adjustHealth(-2);
    player.adjustSocial(-2);
    player.adjustAcademic(-2);
    player.adjustFitness(-2);
}

// Week 5 specific events
void week5StudyPlanningResearchStudy(Player &player)
{
    if (!rollProbability(0.30)) return;
    EventSpacing spacing;
    std::cout << "Research study pays participants! Quick cash for a one-hour cognitive test.\n";
    player.adjustMoney(40);
    player.adjustAcademic(2);
}

void week5FinancialPlanningRefund(Player &player)
{
    if (!rollProbability(0.25)) return;
    EventSpacing spacing;
    std::cout << "UNEXPECTED REFUND! Campus overcharged you last semester.\n";
    player.adjustMoney(60);
}

void week5FitnessFocusOldInjury(Player &player)
{
    if (!rollProbability(0.20)) return;
    EventSpacing spacing;
    std::cout << "Old injury flares up. Should have listened to your body.\n";
    player.adjustFitness(-10);
    player.adjustHealth(-8);
    player.adjustEnergy(-15);
}

void week5AcademicCatchupExtraHelp(Player &player)
{
    if (!rollProbability(0.35)) return;
    EventSpacing spacing;
    std::cout << "PROFESSOR OFFERS EXTRA HELP! One-on-one guidance makes everything clearer.\n";
    player.adjustAcademic(15);
}

void week5SocialRebuildingMovingGig(Player &player)
{
    if (!rollProbability(0.25)) return;
    EventSpacing spacing;
    std::cout << "FRIEND NEEDS HELP MOVING - PAYS WELL!\n";
    player.adjustMoney(60);
    player.adjustEnergy(-20);
    player.adjustSocial(5);
}

void week5EffectiveStudyingStudyLeader(Player &player)
{
    if (player.academic <= 65) return;
    if (!rollProbability(0.20)) return;
    EventSpacing spacing;
    std::cout << "PAID STUDY GROUP LEADER NEEDED! You take the role.\n";
    player.adjustMoney(25);
    player.adjustAcademic(3);
    player.adjustSocial(2);
}

void week5MeaningfulSocialGetaway(Player &player)
{
    if (!rollProbability(0.40)) return;
    EventSpacing spacing;
    std::cout << "FRIENDS PLAN WEEKEND GETAWAY! Summer memories in the making.\n";
    player.adjustSocial(15);
    player.adjustEnergy(-15);
    player.adjustMoney(-25);
}

void week5RestorativePerfectRecovery(Player &player)
{
    if (!rollProbability(0.35)) return;
    EventSpacing spacing;
    std::cout << "PERFECT RECOVERY DAY! Sometimes doing less accomplishes more.\n";
    player.adjustHealth(10);
    player.adjustEnergy(20);
    player.adjustSocial(3);
    player.adjustAcademic(3);
    player.adjustFitness(3);
}

void week5WeekendPrepDoubleGig(Player &player)
{
    if (!rollProbability(0.30)) return;
    EventSpacing spacing;
    std::cout << "WEEKEND GIG PAYS DOUBLE! Short-term sacrifice for cash.\n";
    player.adjustMoney(80);
    player.adjustEnergy(-25);
}

void week5StockInvestingOutcome(Player &player)
{
    EventSpacing spacing;
    std::cout << "Let us see... What should you invest in?\n";
    std::cout << "  1) \"Mag7\" (big tech)\n";
    std::cout << "  2) Small cap tech stock\n";
    std::cout << "  3) SPY ETF\n";
    int choice = promptInt(1, 3);

    if (choice == 1) {
        // 80% +30, 20% -5
        if (rollProbability(0.80)) {
            std::cout << "Mag7 rallies hard! You make solid gains.\n";
            player.adjustMoney(30);
        } else {
            std::cout << "Tech has a rough week. You lose a bit.\n";
            player.adjustMoney(-5);
        }
    } else if (choice == 2) {
        // 10% +200, 90% -50
        if (rollProbability(0.10)) {
            std::cout << "Your tiny tech rocket ships to the moon!\n";
            player.adjustMoney(200);
        } else {
            std::cout << "The speculative pick tanks badly.\n";
            player.adjustMoney(-50);
        }
    } else {
        // SPY ETF: steady +20
        std::cout << "You play it safe with a broad market ETF.\n";
        player.adjustMoney(20);
    }
}

void week5MidweekSocialSpice(Player &player, Relationships &rels)
{
    EventSpacing spacing;
    std::cout << "\n\"TIME TO SPICE UP THE SOCIAL GAME\"\n";

    double r = static_cast<double>(rand()) / RAND_MAX;
    if (r < 0.40) {
        std::cout << "It's your birthday! You host a great party with all your friends.\n";
        player.adjustSocial(15);
        player.adjustEnergy(-10);
        for (auto &npc : rels.npcs) {
            npc.affinity = std::max(0, std::min(100, npc.affinity + 5));
        }
    } else if (r < 0.75) {
        std::cout << "Someone has been spreading rumours about you and your latest relationships...\n";
        for (auto &npc : rels.npcs) {
            npc.affinity = std::max(0, std::min(100, npc.affinity - 20));
        }
    } else {
        std::cout << "Nothing special happens socially this week.\n";
    }
}

// Week 6 specific events
void week6HackathonWin(Player &player, Relationships &rels)
{
    if (!rollProbability(0.35)) return;
    EventSpacing spacing;
    std::cout << "YOU WIN THE HACKATHON! Brain power pays off literally.\n";
    player.adjustMoney(200);
    player.adjustAcademic(10);
    rels.interactWith("Sam (Study Buddy)", 20);
}

void week6GamingMarathonTherapeutic(Player &player, Relationships &rels)
{
    if (!rollProbability(0.45)) return;
    EventSpacing spacing;
    std::cout << "GAMING MARATHON TURNS THERAPEUTIC! Jordan gives surprisingly good relationship advice.\n";
    player.adjustSocial(12);
    rels.interactWith("Jordan (Gamer)", 20);
}

// New function to run general random events that can happen any week
void runGeneralRandomEvents(Player &player, Relationships &rels)
{
    // These events can happen in any week with varying probabilities
    if (rollProbability(0.15)) mysteryPackageArrives(player);
    if (rollProbability(0.12)) academicMeltdownImminent(player);
    if (rollProbability(0.10)) campusWifiUpgrade(player);
    if (rollProbability(0.08)) academicMiracleStrikes(player);
    if (rollProbability(0.10)) desperateClientAlert(player);
    if (rollProbability(0.07)) shadyOpportunity(player);
    if (rollProbability(0.09)) unexpectedInheritance(player);
    if (rollProbability(0.11)) urgentFreelanceGig(player);
    if (rollProbability(0.13)) constructionNoiseAdvisory(player);
    
    // Location-specific events
    if (rollProbability(0.08)) printerBreakdown(player, rels);
    if (rollProbability(0.06)) surpriseGymEvent(player);
    if (rollProbability(0.05)) nearDisasterStrikes(player);
    if (rollProbability(0.10)) studyStruggles(player);
    if (rollProbability(0.07)) napTrap(player);
    if (rollProbability(0.06)) legendaryGamingDrop(player, rels);
    if (rollProbability(0.09)) noisyNeighbors(player);
    if (rollProbability(0.08)) perfectSleep(player);
}

} // namespace Events
