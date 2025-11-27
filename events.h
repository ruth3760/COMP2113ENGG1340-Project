#ifndef EVENTS_H
#define EVENTS_H

#include <string>
#include <vector>
#include <functional>

class Player;
class Relationships;

struct Event {
    std::string name;
    std::string description;
    double probability; // 0.0 - 1.0
    std::function<void(Player&)> apply;
};

namespace Events {
    std::vector<Event> generateWeeklyEvents();
    bool rollEvent(const Event &e);

    // Week 1 in-between-scenario random events.
    // The boolean flags are passed by reference from GameState.
    void maybeRunWeek1RandomEvent(
        int slot,
        int currentWeek,
        bool &friendEventDone,
        bool &routerEventDone,
        bool &eveningEventDone,
        Player &player
    );

    // Week-end random events tied to the week index and relationship path.
    void runWeekEndRandomEvents(
        int week,
        const std::string &relationshipPath,
        const std::string &partnerName,
        Player &player,
        Relationships &rels
    );

    // Week 4 scenario-level random events.
    void week4GymOvertraining(Player &player);
    void week4StudyBreakthrough(Player &player);
    void week4ForcedWorkoutInjury(Player &player);
    void week4MoneyDesperationRobbery(Player &player);
    void week4FinalWorkoutPotion(Player &player);
    void week4LastMinuteStudyBurnout(Player &player);
    void week4SocialRecoveryParty(Player &player);
    void week4CompleteCollapseSick(Player &player);
    void week4EmergencyMeasuresRemorse(Player &player);

    // Week 5 scenario-level random events.
    void week5StudyPlanningResearchStudy(Player &player);
    void week5FinancialPlanningRefund(Player &player);
    void week5FitnessFocusOldInjury(Player &player);
    void week5AcademicCatchupExtraHelp(Player &player);
    void week5SocialRebuildingMovingGig(Player &player);
    void week5EffectiveStudyingStudyLeader(Player &player);
    void week5MeaningfulSocialGetaway(Player &player);
    void week5RestorativePerfectRecovery(Player &player);
    void week5WeekendPrepDoubleGig(Player &player);
    void week5StockInvestingOutcome(Player &player);
    void week5MidweekSocialSpice(Player &player, Relationships &rels);

    // Week 6 scenario-level random events.
    void week6HackathonWin(Player &player, Relationships &rels);
    void week6GamingMarathonTherapeutic(Player &player, Relationships &rels);

    // Week 7 end-of-week, path-based random events are handled inside runWeekEndRandomEvents.
}

#endif // EVENTS_H
