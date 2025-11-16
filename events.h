#ifndef EVENTS_H
#define EVENTS_H

#include <string>
#include <vector>
#include <functional>

class Player;

struct Event {
    std::string name;
    std::string description;
    double probability; // 0.0 - 1.0
    std::function<void(Player&)> apply;
};

namespace Events {
    std::vector<Event> generateWeeklyEvents();
    bool rollEvent(const Event &e);
}

#endif // EVENTS_H
