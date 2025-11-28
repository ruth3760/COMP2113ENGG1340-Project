#include "relationships.h"
#include <algorithm>

Relationships::Relationships()
{
    // Constructor for the Relationships class.
    // Purpose: Initialize the game’s relationship system by seeding a set of NPCs
    // (Non-Player Characters) with default names and starting relationship values.

    // Each call to push_back adds a new NPC to the 'npcs' container.
    // The NPCs are defined by a pair: {string name, int relationshipScore}.
    // Names are taken directly from the design document, and each starts with a score of 20.
    
    // seed some NPCs (names taken from the design doc)
    npcs.push_back({"Alex (Gym Crush)", 20});
    npcs.push_back({"Sam (Study Buddy)", 20});
    npcs.push_back({"Riley (Barista)", 20});
    npcs.push_back({"Jordan (Gamer)", 20});
}

void Relationships::interactWith(const std::string &name, int effect)
{
    // Purpose:
    // --------
    // Adjusts the relationship affinity score of an NPC (Non-Player Character)
    // based on an interaction. If the NPC already exists, their affinity is updated.
    // If the NPC does not exist, a new one is created with the given name and effect.

    // Inputs:
    // -------
    // - name  : The name (or partial name) of the NPC to interact with.
    // - effect: The change in affinity (positive or negative) caused by the interaction.
    //           Example: +10 for a positive event, -5 for a negative event.

    // Outputs:
    // --------
    // - No return value (void function).
    // - Side effect: Updates the 'npcs' container.
    //   * If NPC exists → modifies their affinity.
    //   * If NPC does not exist → adds a new NPC with the given name and initial affinity.
    
    for (auto &n : npcs) {
        if (n.name.find(name) != std::string::npos) {
            n.affinity = std::max(0, std::min(100, n.affinity + effect));
            return;
        }
    }
    // If no match, add as a new NPC
    npcs.push_back({name, std::max(0, std::min(100, effect))});
}

