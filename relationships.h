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
    Relationships();

    void interactWith(const std::string &name, int effect);
};

#endif // RELATIONSHIPS_H
