#include "relationships.h"
#include <algorithm>

// Relationships::Relationships()
// Initializes the relationship tracker with the four main NPCs and a baseline affinity.
// Input: none. Output: Relationships object containing Alex, Sam, Riley, and Jordan with starting affinity.
Relationships::Relationships()
{
    // seed some NPCs (names taken from the design doc)
    npcs.push_back({"Alex (Gym Crush)", 20});
    npcs.push_back({"Sam (Study Buddy)", 20});
    npcs.push_back({"Riley (Barista)", 20});
    npcs.push_back({"Jordan (Gamer)", 20});
}

// Relationships::interactWith()
// Applies an affinity change to the NPC whose name contains the given substring, or adds a new NPC.
// Input: name fragment to search for and signed effect to apply. Output: Matching NPC affinity updated and clamped.
void Relationships::interactWith(const std::string &name, int effect)
{
    for (auto &n : npcs) {
        if (n.name.find(name) != std::string::npos) {
            n.affinity = std::max(0, std::min(100, n.affinity + effect));
            return;
        }
    }
    // If no match, add as a new NPC
    npcs.push_back({name, std::max(0, std::min(100, effect))});
}
