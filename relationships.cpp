#include "relationships.h"
#include <algorithm>

Relationships::Relationships()
{
    // seed some NPCs (names taken from the design doc)
    npcs.push_back({"Alex (Gym Crush)", 20});
    npcs.push_back({"Sam (Study Buddy)", 20});
    npcs.push_back({"Riley (Barista)", 20});
    npcs.push_back({"Jordan (Gamer)", 20});
}

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
