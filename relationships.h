#ifndef RELATIONSHIPS_H
#define RELATIONSHIPS_H

#include <string>
#include <vector>

struct NPC {
    std::string name;
    int affinity; // 0-100
};

class Relationships {
public:
    std::vector<NPC> npcs;

    // Relationships()
    // Constructs a relationship tracker and seeds it with the core NPCs from the design doc.
    // Input: none. Output: Relationships instance with four default NPC entries.
    Relationships();

    // interactWith()
    // Modifies affinity for a named NPC, creating one if it does not exist.
    // Input: NPC name (substring match) and signed affinity effect. Output: updated affinity clamped to [0,100].
    void interactWith(const std::string &name, int effect);
};

#endif // RELATIONSHIPS_H
