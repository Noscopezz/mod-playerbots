#ifndef _PLAYERBOT_RAIDNAXXSTRATEGY_H
#define _PLAYERBOT_RAIDNAXXSTRATEGY_H

#include "AiObjectContext.h"
#include "Multiplier.h"
#include "Strategy.h"
#include "RaidNaxxMultipliers.h"

class RaidNaxxStrategy : public Strategy
{
public:
    RaidNaxxStrategy(PlayerbotAI* ai) : Strategy(ai) {}
    std::string const getName() override { return "naxx"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
};

#endif