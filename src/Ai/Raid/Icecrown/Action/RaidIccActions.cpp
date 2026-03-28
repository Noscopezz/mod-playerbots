#include "RaidIccActions.h"
#include "NearestNpcsValue.h"
#include "ObjectAccessor.h"
#include "Playerbots.h"
#include "Vehicle.h"
#include "RtiValue.h"
#include "GenericSpellActions.h"
#include "GenericActions.h"
#include "RaidIccTriggers.h"
#include "Multiplier.h"

// Lord Marrowgwar
bool IccLmTankPositionAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "lord marrowgar");
    if (!boss)
        return false;

    if (!botAI->IsTank(bot))
        return false;

    const bool isBossInBoneStorm = botAI->GetAura("Bone Storm", boss) != nullptr;

    if (isBossInBoneStorm)
        return false;

    if (botAI->HasAggro(boss) && botAI->IsMainTank(bot) && boss->GetVictim() == bot)
    {
        const float maxDistanceThreshold = 3.0f;
        const float distance = bot->GetExactDist2d(ICC_LM_TANK_POSITION.GetPositionX(), ICC_LM_TANK_POSITION.GetPositionY());

        if (distance > maxDistanceThreshold)
            return MoveTowardPosition(ICC_LM_TANK_POSITION, maxDistanceThreshold);
    }

    if (botAI->IsAssistTank(bot))
    {
        const float maxDistanceThreshold = 3.0f;
        const float distance = bot->GetExactDist2d(ICC_LM_TANK_POSITION.GetPositionX(), ICC_LM_TANK_POSITION.GetPositionY());

        if (distance > maxDistanceThreshold)
            return MoveTowardPosition(ICC_LM_TANK_POSITION, maxDistanceThreshold);

        if (distance < maxDistanceThreshold)
        {
            bot->SetFacingToObject(boss);
            return true;
        }
    }

    return false;
}

bool IccLmTankPositionAction::MoveTowardPosition(const Position& position, float incrementSize)
{
    // Calculate direction vector
    const float dirX = position.GetPositionX() - bot->GetPositionX();
    const float dirY = position.GetPositionY() - bot->GetPositionY();
    const float length = std::sqrt(dirX * dirX + dirY * dirY);

    // Normalize direction vector
    const float normalizedDirX = dirX / length;
    const float normalizedDirY = dirY / length;

    // Calculate new position with increment
    const float moveX = bot->GetPositionX() + normalizedDirX * incrementSize;
    const float moveY = bot->GetPositionY() + normalizedDirY * incrementSize;

    return MoveTo(bot->GetMapId(), moveX, moveY, bot->GetPositionZ(), false, false, false, false,
                  MovementPriority::MOVEMENT_COMBAT);
}

bool IccSpikeAction::Execute(Event /*event*/)
{
    // If we're impaled, we can't do anything
    if (botAI->GetAura("Impaled", bot))
        return false;

    // Find the boss
    Unit* boss = AI_VALUE2(Unit*, "find target", "lord marrowgar");
    if (!boss)
        return false;

    const bool isBossInBoneStorm = botAI->GetAura("Bone Storm", boss) != nullptr;
    const bool shouldMoveToSafePosition = boss->isInFront(bot) && !botAI->IsTank(bot) && !isBossInBoneStorm;

    if (shouldMoveToSafePosition)
    {
        const Position safePosition{-390.6757f, 2230.5283f, 0.0f};  // Z value to be overridden by actual bot Z
        const float distance = bot->GetExactDist2d(safePosition.GetPositionX(), safePosition.GetPositionY());
        const float maxDistanceThreshold = 3.0f;

        if (distance > maxDistanceThreshold)
            return MoveTowardPosition(safePosition, maxDistanceThreshold);

        return false;
    }

    if (!botAI->IsTank(bot))
        return false;

    return HandleSpikeTargeting(boss);
}

bool IccSpikeAction::HandleSpikeTargeting(Unit* boss)
{
    static const std::array<uint32, 3> spikeEntries = {NPC_SPIKE1, NPC_SPIKE2, NPC_SPIKE3};
    const GuidVector spikes = AI_VALUE(GuidVector, "possible targets no los");

    Unit* priorityTarget = nullptr;
    bool anySpikesExist = false;

    // First check for alive spikes
    for (const auto entry : spikeEntries)
    {
        for (auto const& guid : spikes)
        {
            if (Unit* unit = botAI->GetUnit(guid))
            {
                if (unit->GetEntry() == entry)
                {
                    anySpikesExist = true;  // At least one spike exists

                    if (unit->IsAlive())
                    {  // Only consider alive ones for targeting
                        priorityTarget = unit;
                        break;
                    }
                }
            }
        }
        if (priorityTarget)
            break;
    }

    // Only fallback to boss if NO spikes exist at all (alive or dead)
    if (!anySpikesExist && boss->IsAlive())
        priorityTarget = boss;

    // Update raid target icon if needed
    if (priorityTarget)
        UpdateRaidTargetIcon(priorityTarget);

    return false;
}

bool IccSpikeAction::MoveTowardPosition(const Position& position, float incrementSize)
{
    // Calculate direction vector
    const float dirX = position.GetPositionX() - bot->GetPositionX();
    const float dirY = position.GetPositionY() - bot->GetPositionY();
    const float length = std::sqrt(dirX * dirX + dirY * dirY);

    // Normalize direction vector
    const float normalizedDirX = dirX / length;
    const float normalizedDirY = dirY / length;

    // Calculate new position with increment
    const float moveX = bot->GetPositionX() + normalizedDirX * incrementSize;
    const float moveY = bot->GetPositionY() + normalizedDirY * incrementSize;

    return MoveTo(bot->GetMapId(), moveX, moveY, bot->GetPositionZ(), false, false, false, false,
                  MovementPriority::MOVEMENT_COMBAT);
}

bool IccSpikeAction::UpdateRaidTargetIcon(Unit* target)
{
    static constexpr uint8_t SKULL_ICON_INDEX = 7;

    if (Group* group = bot->GetGroup())
    {
        const ObjectGuid currentSkull = group->GetTargetIcon(SKULL_ICON_INDEX);
        Unit* currentSkullUnit = botAI->GetUnit(currentSkull);

        const bool needsUpdate = !currentSkullUnit || !currentSkullUnit->IsAlive() || currentSkullUnit != target;

        if (needsUpdate)
            group->SetTargetIcon(SKULL_ICON_INDEX, bot->GetGUID(), target->GetGUID());
    }

    return false;
}

// Lady Deathwhisper
bool IccDarkReckoningAction::Execute(Event /*event*/)
{
    constexpr float SAFE_DISTANCE_THRESHOLD = 2.0f;

    // Check if the bot needs to move to the safe position
    if (bot->HasAura(SPELL_DARK_RECKONING) &&
        bot->GetExactDist2d(ICC_DARK_RECKONING_SAFE_POSITION) > SAFE_DISTANCE_THRESHOLD)
    {
        // Move to the safe position with the same parameters as before
        return MoveTo(bot->GetMapId(), ICC_DARK_RECKONING_SAFE_POSITION.GetPositionX(),
                      ICC_DARK_RECKONING_SAFE_POSITION.GetPositionY(),
                      ICC_DARK_RECKONING_SAFE_POSITION.GetPositionZ(),
                      false, false, false, true, MovementPriority::MOVEMENT_NORMAL);
    }

    return false;
}

bool IccRangedPositionLadyDeathwhisperAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "lady deathwhisper");
    if (!boss)
        return false;

    const float currentDistance = bot->GetDistance2d(boss);
    const float minDistance = 7.0f;
    const float maxDistance = 30.0f;

    if (currentDistance < minDistance || currentDistance > maxDistance)
        return false;

    if (!botAI->IsRanged(bot) && !botAI->IsHeal(bot))
        return false;

    return MaintainRangedSpacing();
}

bool IccRangedPositionLadyDeathwhisperAction::MaintainRangedSpacing()
{
    const float safeSpacingRadius = 3.0f;
    const float moveIncrement = 2.0f;
    const float maxMoveDistance = 5.0f;  // Limit maximum movement distance
    const bool isRanged = botAI->IsRanged(bot) || botAI->IsHeal(bot);

    if (!isRanged)
        return false;

    // Ranged: spread from other members
    const GuidVector members = AI_VALUE(GuidVector, "group members");

    // Calculate a combined vector representing all nearby members' positions
    float totalX = 0.0f;
    float totalY = 0.0f;
    int nearbyCount = 0;

    for (auto const& memberGuid : members)
    {
        Unit* member = botAI->GetUnit(memberGuid);
        if (!member || !member->IsAlive() || member == bot)
        {
            continue;
        }

        const float distance = bot->GetExactDist2d(member);
        if (distance < safeSpacingRadius)
        {
            // Calculate vector from member to bot
            float dx = bot->GetPositionX() - member->GetPositionX();
            float dy = bot->GetPositionY() - member->GetPositionY();

            // Weight by inverse distance (closer members have more influence)
            float weight = (safeSpacingRadius - distance) / safeSpacingRadius;
            totalX += dx * weight;
            totalY += dy * weight;
            nearbyCount++;
        }
    }

    // If we have nearby members, move away in the combined direction
    if (nearbyCount > 0)
    {
        // Normalize the combined vector
        float magnitude = std::sqrt(totalX * totalX + totalY * totalY);
        if (magnitude > 0.001f)  // Avoid division by zero
        {
            totalX /= magnitude;
            totalY /= magnitude;

            // Calculate move distance based on nearest member
            float moveDistance = std::min(moveIncrement, maxMoveDistance);

            // Create target position in the combined direction
            float targetX = bot->GetPositionX() + totalX * moveDistance;
            float targetY = bot->GetPositionY() + totalY * moveDistance;
            float targetZ = bot->GetPositionZ();  // Maintain current Z

            // Check if the target position is valid and move there
            if (bot->IsWithinLOS(targetX, targetY, targetZ))
            {
                Position targetPos(targetX, targetY, targetZ);
                MoveTo(bot->GetMapId(), targetPos.GetPositionX(), targetPos.GetPositionY(), targetPos.GetPositionZ(),
                       false, false, false, true, MovementPriority::MOVEMENT_NORMAL);
            }
            else
            {
                // If los check fails, try shorter distance
                targetX = bot->GetPositionX() + totalX * (moveDistance * 0.5f);
                targetY = bot->GetPositionY() + totalY * (moveDistance * 0.5f);
                Position targetPos(targetX, targetY, targetZ);
                MoveTo(bot->GetMapId(), targetPos.GetPositionX(), targetPos.GetPositionY(), targetPos.GetPositionZ(),
                       false, false, false, true, MovementPriority::MOVEMENT_NORMAL);
            }
        }
    }

    return false;  // Everyone is properly spaced
}

bool IccAddsLadyDeathwhisperAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "lady deathwhisper");
    if (!boss)
        return false;

    // Dominate Mind handling
    if (botAI->HasAura("Dominate Mind", bot, false, false) && !bot->HasAura(SPELL_CYCLONE))
        bot->AddAura(SPELL_CYCLONE, bot);
    else if (bot->HasAura(SPELL_CYCLONE) && !botAI->HasAura("Dominate Mind", bot, false, false))
        bot->RemoveAura(SPELL_CYCLONE);

    // Dominate Mind handling for real players in group
    if (Group* group = bot->GetGroup())
    {
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->GetSource();
            if (!member || !member->IsAlive() || member == bot)
                continue;

            PlayerbotAI* memberBotAI = GET_PLAYERBOT_AI(member);
            if (memberBotAI && !memberBotAI->IsRealPlayer())
                continue;

            if (botAI->HasAura("Dominate Mind", member, false, false) && !member->HasAura(SPELL_CYCLONE))
                member->AddAura(SPELL_CYCLONE, member);
            else if (member->HasAura(SPELL_CYCLONE) && !botAI->HasAura("Dominate Mind", member, false, false))
                member->RemoveAura(SPELL_CYCLONE);
        }
    }

    const uint32 shadeEntryId = NPC_SHADE;

    // Tank behavior
    if (botAI->IsTank(bot))
    {
        // Remove debuff if present
        if (bot->HasAura(SPELL_TOUCH_OF_INSIGNIFICANCE))
            bot->RemoveAura(SPELL_TOUCH_OF_INSIGNIFICANCE);

        // If targeted by a shade, ignore (shades are handled elsewhere)
        if (IsTargetedByShade(shadeEntryId))
            return false;

        // Check if we are already engaged with an add
        Unit* currentTarget = bot->GetVictim();
        bool isAttackingAdd = currentTarget && IsAdd(currentTarget);
        if (isAttackingAdd && currentTarget->IsAlive())
        {
            // Already have an add – mark it if we are the designated marker
            if (botAI->IsAssistTank(bot) || !IsAssistTankAlive())
                UpdateRaidTargetIcon(currentTarget);
            return false;  // let combat AI handle attacking
        }

        // No add currently attacked – try to collect a loose add
        Unit* targetAdd = FindAndCollectAdd();

        if (targetAdd)
        {
            // Mark the add if we are the designated marker
            if (botAI->IsAssistTank(bot) || !IsAssistTankAlive())
                UpdateRaidTargetIcon(targetAdd);

            // Ensure we are facing and attacking it
            bot->SetTarget(targetAdd->GetGUID());
            bot->SetFacingToObject(targetAdd);
            Attack(targetAdd);
            return true;
        }

        // No adds alive – engage boss
        if (boss->IsAlive())
        {
            // Only mark the boss if we are the one actively tanking it and no assist tank
            if (!IsAssistTankAlive() && boss->GetVictim() == bot)
                UpdateRaidTargetIcon(boss);

            bot->SetTarget(boss->GetGUID());
            bot->SetFacingToObject(boss);
            Attack(boss);
        }

        return false;
    }

    // Non-tank behavior
    // 1. If targeted by an add: apply CC and move toward the nearest tank
    if (HandleNonTankAddEvasion())
        return false;

    // 2. Even if not targeted, apply CC to nearby adds to help tanks
    ApplyNearbyAddCC();

    // 3. If no adds are alive, engage the boss
    if (IsAddsAlive())
        return false;  // adds still present, we already handled evasion/CC

    // No adds – move to appropriate position and attack boss
    if (boss->HealthAbovePct(95))
        return EngageBoss();

    return false;
}

Unit* IccAddsLadyDeathwhisperAction::FindAndCollectAdd()
{
    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    Unit* bestAdd = nullptr;
    float bestScore = FLT_MAX;  // lower score is better

    for (auto const& npcGuid : npcs)
    {
        Unit* unit = botAI->GetUnit(npcGuid);
        if (!unit || !unit->IsAlive())
            continue;

        // Only consider Lady Deathwhisper adds
        if (!IsAdd(unit))
            continue;

        // Skip adds already tanked by another tank (victim is a tank player)
        Unit* victim = unit->GetVictim();
        if (victim && victim->IsPlayer() && botAI->IsTank(victim->ToPlayer()))
            continue;

        // Score: distance (closer is better) + penalty if the add is currently targeting someone else
        float distance = bot->GetExactDist(unit);
        float score = distance;
        if (victim && victim != bot)
            score += 5.0f;  // slight penalty to encourage picking adds already on us

        if (score < bestScore)
        {
            bestScore = score;
            bestAdd = unit;
        }
    }

    if (!bestAdd)
        return nullptr;

    // Move into range and taunt
    float tauntRange = 30.0f;  // typical taunt range
    float currentDist = bot->GetExactDist(bestAdd);

    if (currentDist > tauntRange)
    {
        // Move toward the add in increments until in taunt range
        float moveX = bot->GetPositionX();
        float moveY = bot->GetPositionY();
        float moveZ = bot->GetPositionZ();
        float step = 5.0f;
        float dx = bestAdd->GetPositionX() - bot->GetPositionX();
        float dy = bestAdd->GetPositionY() - bot->GetPositionY();
        float dist = sqrt(dx * dx + dy * dy);
        if (dist > 0.1f)
        {
            dx /= dist;
            dy /= dist;
            float moveDist = std::min(step, dist - (tauntRange - 5.0f));
            moveX = bot->GetPositionX() + dx * moveDist;
            moveY = bot->GetPositionY() + dy * moveDist;
        }
        MoveTo(bot->GetMapId(), moveX, moveY, moveZ, false, false, false, true, MovementPriority::MOVEMENT_COMBAT);
        return nullptr;  // will be called again next tick
    }

    // In taunt range – try to taunt
    bool taunted = false;
    if (botAI->CastSpell("taunt", bestAdd))
        taunted = true;
    else
    {
        switch (bot->getClass())
        {
            case CLASS_PALADIN:
                taunted = botAI->CastSpell("hand of reckoning", bestAdd);
                break;
            case CLASS_DEATH_KNIGHT:
                taunted = botAI->CastSpell("dark command", bestAdd);
                break;
            case CLASS_DRUID:
                taunted = botAI->CastSpell("growl", bestAdd);
                break;
            default:
                break;
        }
    }

    if (!taunted)
    {
        // If taunt failed, move into melee range and start attacking
        if (currentDist > 5.0f)
        {
            // Move closer
            float dx = bestAdd->GetPositionX() - bot->GetPositionX();
            float dy = bestAdd->GetPositionY() - bot->GetPositionY();
            float dist = sqrt(dx * dx + dy * dy);
            dx /= dist;
            dy /= dist;
            float step = std::min(3.0f, dist - 3.0f);
            float moveX = bot->GetPositionX() + dx * step;
            float moveY = bot->GetPositionY() + dy * step;
            MoveTo(bot->GetMapId(), moveX, moveY, bot->GetPositionZ(), false, false, false, true,
                   MovementPriority::MOVEMENT_COMBAT);
            return nullptr;
        }
    }

    return bestAdd;
}

bool IccAddsLadyDeathwhisperAction::HandleNonTankAddEvasion()
{
    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    // Check if any add is targeting this bot
    bool targetedByAdd = false;
    for (auto const& npcGuid : npcs)
    {
        Unit* unit = botAI->GetUnit(npcGuid);
        if (!unit || !unit->IsAlive() || unit->GetVictim() != bot)
            continue;

        if (IsAdd(unit))
        {
            targetedByAdd = true;
            // Apply CC to this add
            ApplyCCToAdd(unit);
        }
    }

    if (!targetedByAdd)
        return false;

    // Find nearest alive tank in group
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    Unit* nearestTank = nullptr;
    float minDist = FLT_MAX;

    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (!member || !member->IsAlive() || member == bot)
            continue;

        if (!botAI->IsTank(member))
            continue;

        float dist = bot->GetExactDist2d(member);
        if (dist < minDist)
        {
            minDist = dist;
            nearestTank = member;
        }
    }

    if (!nearestTank)
        return false;

    // Already close enough to the tank, stop moving
    if (minDist <= 5.0f)
        return false;

    // Move toward the tank in increments
    float dx = nearestTank->GetPositionX() - bot->GetPositionX();
    float dy = nearestTank->GetPositionY() - bot->GetPositionY();
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < 0.001f)
        return false;

    dx /= dist;
    dy /= dist;

    float step = std::min(5.0f, dist);
    float moveX = bot->GetPositionX() + dx * step;
    float moveY = bot->GetPositionY() + dy * step;

    return MoveTo(bot->GetMapId(), moveX, moveY, bot->GetPositionZ(), false, false, false, false,
                  MovementPriority::MOVEMENT_COMBAT);
}

bool IccAddsLadyDeathwhisperAction::ApplyNearbyAddCC()
{
    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    const float CC_RANGE = 10.0f;  // apply CC to adds within this range, even if not targeting us

    for (auto const& npcGuid : npcs)
    {
        Unit* unit = botAI->GetUnit(npcGuid);
        if (!unit || !unit->IsAlive())
            continue;
        if (!IsAdd(unit))
            continue;
        if (bot->GetDistance(unit) <= CC_RANGE)
            ApplyCCToAdd(unit);
    }

    return false;
}

bool IccAddsLadyDeathwhisperAction::ApplyCCToAdd(Unit* add)
{
    if (!add || !add->IsAlive())
        return false;

    // Avoid applying CC if the add is already CCed or dead
    // The aura checks inside each case handle that.
    switch (bot->getClass())
    {
        case CLASS_MAGE:
            if (!botAI->HasAura("Frost Nova", add))
                botAI->CastSpell("Frost Nova", add);
            break;
        case CLASS_DRUID:
            if (!botAI->HasAura("Entangling Roots", add))
                botAI->CastSpell("Entangling Roots", add);
            break;
        case CLASS_PALADIN:
            if (!botAI->HasAura("Hammer of Justice", add))
                botAI->CastSpell("Hammer of Justice", add);
            break;
        case CLASS_WARRIOR:
            if (!botAI->HasAura("Hamstring", add))
                botAI->CastSpell("Hamstring", add);
            break;
        case CLASS_HUNTER:
            if (!botAI->HasAura("Concussive Shot", add))
                botAI->CastSpell("Concussive Shot", add);
            break;
        case CLASS_ROGUE:
            if (!botAI->HasAura("Kidney Shot", add))
                botAI->CastSpell("Kidney Shot", add);
            break;
        case CLASS_SHAMAN:
            if (!botAI->HasAura("Frost Shock", add))
                botAI->CastSpell("Frost Shock", add);
            break;
        case CLASS_DEATH_KNIGHT:
            if (!botAI->HasAura("Chains of Ice", add))
                botAI->CastSpell("Chains of Ice", add);
            break;
        case CLASS_PRIEST:
            if (!botAI->HasAura("Psychic Scream", add))
                botAI->CastSpell("Psychic Scream", add);
            break;
        case CLASS_WARLOCK:
            if (!botAI->HasAura("Fear", add))
                botAI->CastSpell("Fear", add);
            break;
        default:
            break;
    }

    return false;
}

bool IccAddsLadyDeathwhisperAction::IsAddsAlive()
{
    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto const& npcGuid : npcs)
    {
        Unit* unit = botAI->GetUnit(npcGuid);
        if (unit && unit->IsAlive() && IsAdd(unit))
            return true;
    }

    return false;
}

bool IccAddsLadyDeathwhisperAction::EngageBoss()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "lady deathwhisper");
    if (!boss || !boss->IsAlive())
        return false;

    // Determine if we are melee or ranged
    bool isMelee = botAI->IsMelee(bot);
    bool isRanged = botAI->IsRanged(bot) || botAI->IsHeal(bot);

    // For melee: move into melee range of boss
    if (isMelee)
    {
        float dist = bot->GetDistance(boss);
        if (dist > 3.0f)
        {
            // Move toward boss in increments
            float dx = boss->GetPositionX() - bot->GetPositionX();
            float dy = boss->GetPositionY() - bot->GetPositionY();
            float len = sqrt(dx * dx + dy * dy);
            if (len > 0.1f)
            {
                dx /= len;
                dy /= len;
                float step = std::min(3.0f, len);
                float moveX = bot->GetPositionX() + dx * step;
                float moveY = bot->GetPositionY() + dy * step;
                MoveTo(bot->GetMapId(), moveX, moveY, bot->GetPositionZ(), false, false, false, true,
                       MovementPriority::MOVEMENT_COMBAT);
                return false;
            }
        }
        // Already in melee range or we moved enough
        bot->SetTarget(boss->GetGUID());
        bot->SetFacingToObject(boss);
        Attack(boss);
        return false;
    }

    if (isRanged)
    {
        // Use the ranged stack position defined for Lady Deathwhisper
        const Position rangedPos = ICC_LDW_RANGED_POSITION;  // Assume this exists
        float distToPos = bot->GetDistance2d(rangedPos.GetPositionX(), rangedPos.GetPositionY());
        if (distToPos > 20.0f)
        {
            MoveTo(bot->GetMapId(), rangedPos.GetPositionX(), rangedPos.GetPositionY(), rangedPos.GetPositionZ(), false, false, false, true, MovementPriority::MOVEMENT_COMBAT);
            return false;
        }
        // At position, attack boss
        bot->SetTarget(boss->GetGUID());
        bot->SetFacingToObject(boss);
        Attack(boss);
        return false;
    }

    return false;
}

bool IccAddsLadyDeathwhisperAction::IsAdd(Unit* unit)
{
    if (!unit)
        return false;
    uint32 entry = unit->GetEntry();
    return std::find(addEntriesLady.begin(), addEntriesLady.end(), entry) != addEntriesLady.end();
}

bool IccAddsLadyDeathwhisperAction::IsAssistTankAlive()
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (!member || !member->IsAlive() || member == bot)
            continue;
        if (botAI->IsTank(member) && !botAI->IsMainTank(member))
            return true;
    }

    return false;
}

bool IccAddsLadyDeathwhisperAction::IsTargetedByShade(uint32 shadeEntry)
{
    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto const& npcGuid : npcs)
    {
        Unit* unit = botAI->GetUnit(npcGuid);
        if (unit && unit->GetEntry() == shadeEntry && unit->GetVictim() == bot)
            return true;
    }

    return false;
}

bool IccAddsLadyDeathwhisperAction::MoveTowardPosition(const Position& position, float incrementSize)
{
    // Calculate direction vector
    const float dirX = position.GetPositionX() - bot->GetPositionX();
    const float dirY = position.GetPositionY() - bot->GetPositionY();
    const float length = std::sqrt(dirX * dirX + dirY * dirY);

    // Normalize direction vector
    const float normalizedDirX = dirX / length;
    const float normalizedDirY = dirY / length;

    // Calculate new position with increment
    const float moveX = bot->GetPositionX() + normalizedDirX * incrementSize;
    const float moveY = bot->GetPositionY() + normalizedDirY * incrementSize;

    return MoveTo(bot->GetMapId(), moveX, moveY, bot->GetPositionZ(), false, false, false, false,
                  MovementPriority::MOVEMENT_COMBAT);
}

bool IccAddsLadyDeathwhisperAction::HandleAddTargeting(Unit* boss)
{
    const GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    Unit* priorityTarget = nullptr;
    bool hasValidAdds = false;

    // First check for alive adds
    for (auto const& entry : addEntriesLady)
    {
        for (auto const& guid : targets)
        {
            Unit* unit = botAI->GetUnit(guid);
            if (unit && unit->IsAlive() && unit->GetEntry() == entry)
            {
                priorityTarget = unit;
                hasValidAdds = true;
                break;
            }
        }
        if (priorityTarget)
            break;
    }

    // Only fallback to boss if NO adds exist
    if (!hasValidAdds && boss->IsAlive())
        priorityTarget = boss;

    // Update skull icon if needed
    if (priorityTarget)
        UpdateRaidTargetIcon(priorityTarget);

    return false;
}

bool IccAddsLadyDeathwhisperAction::UpdateRaidTargetIcon(Unit* target)
{
    static constexpr uint8_t SKULL_ICON_INDEX = 7;

    if (!target || !target->IsAlive())
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    ObjectGuid currentSkull = group->GetTargetIcon(SKULL_ICON_INDEX);
    Unit* currentSkullUnit = botAI->GetUnit(currentSkull);

    if (!currentSkullUnit || !currentSkullUnit->IsAlive() || currentSkullUnit != target)
        group->SetTargetIcon(SKULL_ICON_INDEX, bot->GetGUID(), target->GetGUID());

    return false;
}

bool IccShadeLadyDeathwhisperAction::Execute(Event /*event*/)
{
    static constexpr uint32 VENGEFUL_SHADE_ID = NPC_SHADE;
    static constexpr float SAFE_DISTANCE = 12.0f;

    // Get the nearest hostile NPCs
    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    for (auto const& npcGuid : npcs)
    {
        Unit* shade = botAI->GetUnit(npcGuid);

        // Skip if not a vengeful shade
        if (!shade || shade->GetEntry() != VENGEFUL_SHADE_ID)
            continue;

        // Only run away if the shade is targeting us
        // Check by GUID comparison to ensure we're accurately identifying the specific shade
        // This is especially important in 25HC where multiple shades can spawn
        if (!shade->GetVictim() || shade->GetVictim()->GetGUID() != bot->GetGUID())
            continue;

        const float currentDistance = bot->GetDistance2d(shade);

        // Move away from the Vengeful Shade if the bot is too close
        if (currentDistance < SAFE_DISTANCE)
        {
            // Calculate direction away from shade
            float dx = bot->GetPositionX() - shade->GetPositionX();
            float dy = bot->GetPositionY() - shade->GetPositionY();
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist < 0.001f)
                continue;

            dx /= dist;
            dy /= dist;

            float moveDistance = SAFE_DISTANCE - currentDistance;
            float targetX = bot->GetPositionX() + dx * moveDistance;
            float targetY = bot->GetPositionY() + dy * moveDistance;
            float targetZ = bot->GetPositionZ();

            if (bot->IsWithinLOS(targetX, targetY, targetZ))
            {
                botAI->Reset();
                MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
            }
        }
    }

    return false;
}

bool IccRottingFrostGiantTankPositionAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "rotting frost giant");
    if (!boss)
        return false;

    Aura* aura = botAI->GetAura("death plague", bot, false, false);
    if (aura)
        bot->RemoveAura(aura->GetId());

/* TODO: code works for handling plague, but atm script is bugged and one bot can have 2 plagues at the same time or when cured, which should not happen, and it is immpossible to handle plague atm the legit way.
    const bool hasCure = botAI->GetAura("recently infected", bot) != nullptr;

    // Tank behavior - unchanged
    if (botAI->IsTank(bot) && botAI->HasAggro(boss) && !isInfected)
        if (bot->GetExactDist2d(ICC_ROTTING_FROST_GIANT_TANK_POSITION) > 5.0f)
            return MoveTo(bot->GetMapId(), ICC_ROTTING_FROST_GIANT_TANK_POSITION.GetPositionX(),
                          ICC_ROTTING_FROST_GIANT_TANK_POSITION.GetPositionY(),
                          ICC_ROTTING_FROST_GIANT_TANK_POSITION.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_NORMAL);

    if (botAI->IsTank(bot))
        return false;

    // Handle infected bot behavior - move near a non-infected, non-cured bot
    if (isInfected)
    {
        const GuidVector members = AI_VALUE(GuidVector, "group members");

        // Count how many bots are targeting each potential target
        std::map<ObjectGuid, int> targetCounts;

        // First, identify all infected bots and their current targets (approximate)
        for (auto const& memberGuid : members)
        {
            Unit* member = botAI->GetUnit(memberGuid);
            if (!member || !member->IsAlive() || member == bot)
                continue;

            const bool memberIsInfected = botAI->GetAura("death plague", member) != nullptr;

            if (memberIsInfected)
            {
                // Find the nearest non-infected bot to this infected bot (as a guess of its target)
                float minDist = 5.0f;  // Only count if they're close enough to likely be targeting
                Unit* likelyTarget = nullptr;

                for (auto const& targetGuid : members)
                {
                    Unit* potentialTarget = botAI->GetUnit(targetGuid);
                    if (!potentialTarget || !potentialTarget->IsAlive() || potentialTarget == member)
                        continue;

                    const bool targetIsInfected = botAI->GetAura("death plague", potentialTarget) != nullptr;
                    const bool targetHasCure = botAI->GetAura("recently infected", potentialTarget) != nullptr;

                    if (!targetIsInfected && !targetHasCure)
                    {
                        float dist = member->GetExactDist2d(potentialTarget);
                        if (dist < minDist)
                        {
                            minDist = dist;
                            likelyTarget = potentialTarget;
                        }
                    }
                }

                if (likelyTarget)
                {
                    targetCounts[likelyTarget->GetGUID()]++;
                }
            }
        }

        // Find viable targets and score them based on various factors
        std::vector<std::pair<Unit*, float>> viableTargets;

        // First try to find ranged, non-infected, non-cured bots
        for (auto const& memberGuid : members)
        {
            Unit* member = botAI->GetUnit(memberGuid);
            if (!member || !member->IsAlive() || member == bot)
                continue;

            const bool memberHasCure = botAI->GetAura("recently infected", member) != nullptr;
            const bool memberIsInfected = botAI->GetAura("death plague", member) != nullptr;

            if (!memberIsInfected && !memberHasCure)
            {
                // Base score is distance (lower is better)
                float score = bot->GetExactDist2d(member);

                // Prefer ranged targets
                if (botAI->IsRanged(bot))
                {
                    score *= 0.7f;  // Bonus for ranged targets
                }

                // Apply penalty based on how many other infected bots are targeting this one
                int targetingCount = targetCounts[member->GetGUID()];
                score *= (1.0f + targetingCount * 0.5f);  // Increase score (worse) for heavily targeted bots

                viableTargets.push_back(std::make_pair(member, score));
            }
        }

        // Sort targets by score (lowest/best first)
        std::sort(viableTargets.begin(), viableTargets.end(),
                  [](const std::pair<Unit*, float>& a, const std::pair<Unit*, float>& b)
                  { return a.second < b.second; });

        // Choose the best target
        Unit* targetBot = nullptr;
        if (!viableTargets.empty())
        {
            targetBot = viableTargets[0].first;
        }

        // Move to target bot if found
        if (targetBot)
        {
            // If we're already close enough (1 yard), no need to move
            if (bot->GetExactDist2d(targetBot) > 1.0f)
            {
                // Calculate a unique angle based on bot's GUID to ensure different approach angles
                // This helps spread infected bots around the target
                uint32 guidLow = bot->GetGUID().GetCounter();
                float angleOffset = float(guidLow % 628) / 100.0f;  // Random angle between 0 and 2π

                // Calculate position 1 yard away from target at our unique angle
                float angle = targetBot->GetOrientation() + angleOffset;
                float targetX = targetBot->GetPositionX() + cos(angle) * 1.0f;
                float targetY = targetBot->GetPositionY() + sin(angle) * 1.0f;
                float targetZ = targetBot->GetPositionZ();

                return MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, true,
                              MovementPriority::MOVEMENT_COMBAT);
            }
            return true;  // Already in position
        }
        // No suitable target found, continue with normal behavior
    }

    // For ranged bots, only spread from non-infected bots
    if (botAI->IsRanged(bot))
    {
        const float safeSpacingRadius = 11.0f;
        const float moveIncrement = 2.0f;
        const float maxMoveDistance = 15.0f;

        const GuidVector members = AI_VALUE(GuidVector, "group members");

        // Calculate a combined vector representing all nearby NON-INFECTED members' positions
        float totalX = 0.0f;
        float totalY = 0.0f;
        int nearbyCount = 0;

        for (auto const& memberGuid : members)
        {
            Unit* member = botAI->GetUnit(memberGuid);
            if (!member || !member->IsAlive() || member == bot)
                continue;

            // Only spread from non-infected bots (can stay near infected or cured bots)
            const bool memberIsInfected = botAI->GetAura("death plague", member) != nullptr;
            if (memberIsInfected)
                continue;

            const float distance = bot->GetExactDist2d(member);
            if (distance < safeSpacingRadius)
            {
                // Calculate vector from member to bot
                float dx = bot->GetPositionX() - member->GetPositionX();
                float dy = bot->GetPositionY() - member->GetPositionY();

                // Weight by inverse distance (closer members have more influence)
                float weight = (safeSpacingRadius - distance) / safeSpacingRadius;
                totalX += dx * weight;
                totalY += dy * weight;
                nearbyCount++;
            }
        }

        // If we have nearby non-infected members, move away in the combined direction
        if (nearbyCount > 0)
        {
            // Normalize the combined vector
            float magnitude = std::sqrt(totalX * totalX + totalY * totalY);
            if (magnitude > 0.001f)  // Avoid division by zero
            {
                totalX /= magnitude;
                totalY /= magnitude;

                // Calculate move distance based on nearest member
                float moveDistance = std::min(moveIncrement, maxMoveDistance);

                // Create target position in the combined direction
                float targetX = bot->GetPositionX() + totalX * moveDistance;
                float targetY = bot->GetPositionY() + totalY * moveDistance;
                float targetZ = bot->GetPositionZ();  // Maintain current Z

                // Check if the target position is valid and move there
                if (bot->IsWithinLOS(targetX, targetY, targetZ))
                {
                    return MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, true,
                                  MovementPriority::MOVEMENT_NORMAL);
                }
                else
                {
                    // If los check fails, try shorter distance
                    targetX = bot->GetPositionX() + totalX * (moveDistance * 0.5f);
                    targetY = bot->GetPositionY() + totalY * (moveDistance * 0.5f);
                    return MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, true,
                                  MovementPriority::MOVEMENT_NORMAL);
                }
            }
        }
    }
*/
    return false;  // No movement needed
}

// Gunship Battle
bool IccCannonFireAction::Execute(Event /*event*/)
{
    Unit* vehicleBase = bot->GetVehicleBase();
    Vehicle* vehicle = bot->GetVehicle();

    if (!vehicleBase || !vehicle)
        return false;

    Unit* target = FindValidCannonTarget();
    if (!target)
        return false;

    // Use Incinerating Blast when we have enough energy, otherwise Cannon Blast
    static constexpr float ENERGY_THRESHOLD = 90.0f;
    if (vehicleBase->GetPower(POWER_ENERGY) >= ENERGY_THRESHOLD)
    {
        uint32 const blastSpellId = AI_VALUE2(uint32, "vehicle spell id", "incinerating blast");
        if (TryCastCannonSpell(blastSpellId, target, vehicleBase))
            return true;
    }

    uint32 const cannonSpellId = AI_VALUE2(uint32, "vehicle spell id", "cannon blast");
    return TryCastCannonSpell(cannonSpellId, target, vehicleBase);
}

Unit* IccCannonFireAction::FindValidCannonTarget()
{
    GuidVector const attackers = AI_VALUE(GuidVector, "possible targets no los");

    for (ObjectGuid const& attackerGuid : attackers)
    {
        Unit* unit = botAI->GetUnit(attackerGuid);
        if (!unit)
            continue;

        for (uint32 const entry : availableTargetsGS)
        {
            if (unit->GetEntry() == entry)
                return unit;
        }
    }

    return nullptr;
}

bool IccCannonFireAction::TryCastCannonSpell(uint32 spellId, Unit* target, Unit* vehicleBase)
{
    static constexpr uint32 COOLDOWN_MS = 1000;

    if (botAI->CanCastVehicleSpell(spellId, target) && botAI->CastVehicleSpell(spellId, target))
    {
        vehicleBase->AddSpellCooldown(spellId, 0, COOLDOWN_MS);
        return true;
    }

    return false;
}

bool IccGunshipEnterCannonAction::Execute(Event /*event*/)
{
    // Never switch vehicles while already in one
    if (bot->GetVehicle())
        return false;

    Unit* bestVehicle = FindBestAvailableCannon();
    if (!bestVehicle)
        return false;

    return EnterVehicle(bestVehicle, true);
}

Unit* IccGunshipEnterCannonAction::FindBestAvailableCannon()
{
    Unit* bestVehicle = nullptr;

    GuidVector const npcs = AI_VALUE(GuidVector, "nearest vehicles");
    for (ObjectGuid const& npcGuid : npcs)
    {
        Unit* vehicleBase = botAI->GetUnit(npcGuid);
        if (!IsValidCannon(vehicleBase))
            continue;

        // Prefer the closest valid cannon
        if (!bestVehicle || bot->GetExactDist(vehicleBase) < bot->GetExactDist(bestVehicle))
            bestVehicle = vehicleBase;
    }

    return bestVehicle;
}

bool IccGunshipEnterCannonAction::IsValidCannon(Unit* vehicle)
{
    if (!vehicle)
        return false;

    if (vehicle->HasUnitFlag(UNIT_FLAG_NOT_SELECTABLE))
        return false;

    if (!vehicle->IsFriendlyTo(bot))
        return false;

    if (!vehicle->GetVehicleKit() || !vehicle->GetVehicleKit()->GetAvailableSeatCount())
        return false;

    uint32 const entry = vehicle->GetEntry();
    if (entry != NPC_CANNONA && entry != NPC_CANNONH)
        return false;

    // Frozen or disabled cannon — skip
    if (vehicle->HasAura(69704) || vehicle->HasAura(69705))
        return false;

    return true;
}

bool IccGunshipEnterCannonAction::EnterVehicle(Unit* vehicleBase, bool moveIfFar)
{
    float const dist = bot->GetDistance(vehicleBase);

    if (dist > INTERACTION_DISTANCE && !moveIfFar)
        return false;

    if (dist > INTERACTION_DISTANCE)
        return MoveTo(vehicleBase);

    botAI->RemoveShapeshift();
    bot->GetMotionMaster()->Clear();
    bot->StopMoving();

    vehicleBase->HandleSpellClick(bot);

    if (!bot->IsOnVehicle(vehicleBase))
        return false;

    // Dismount — bots can enter a vehicle while still mounted
    WorldPacket emptyPacket;
    bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);

    return true;
}

IccGunshipTeleportAction::GunshipSide IccGunshipTeleportAction::DetectShip() const
{
    Unit* cannonA = bot->FindNearestCreature(NPC_CANNONA, 100.0f);
    if (cannonA && cannonA->IsFriendlyTo(bot))
        return GunshipSide::ALLY;

    Unit* cannonH = bot->FindNearestCreature(NPC_CANNONH, 100.0f);
    if (cannonH && cannonH->IsFriendlyTo(bot))
        return GunshipSide::HORDE;

    return GunshipSide::NONE;
}

bool IccGunshipTeleportAction::Execute(Event /*event*/)
{
    static constexpr float MAX_WAITING_DISTANCE = 45.0f;
    static constexpr float MAX_ATTACK_DISTANCE = 15.0f;
    static constexpr float HOLD_RADIUS = 20.0f;
    static constexpr uint8 SKULL_ICON_INDEX = 7;

    GunshipSide const side = DetectShip();
    if (side == GunshipSide::NONE)
        return false;

    char const* bossName = (side == GunshipSide::ALLY) ? "kor'kron battle-mage" : "skybreaker sorcerer";
    Position const& waitPos = (side == GunshipSide::ALLY) ? ICC_GUNSHIP_TELEPORT_ALLY2 : ICC_GUNSHIP_TELEPORT_HORDE2;
    Position const& attackPos = (side == GunshipSide::ALLY) ? ICC_GUNSHIP_TELEPORT_ALLY : ICC_GUNSHIP_TELEPORT_HORDE;

    Unit* boss = AI_VALUE2(Unit*, "find target", bossName);

    CleanupSkullIcon(SKULL_ICON_INDEX);

    uint32 const cannonEntry = (side == GunshipSide::ALLY) ? NPC_CANNONA : NPC_CANNONH;
    bool cannonsHaveBelowZero = false;
    Unit* cannon = bot->FindNearestCreature(cannonEntry, 100.0f);
    if (cannon && cannon->IsFriendlyTo(bot) && cannon->HasAura(SPELL_BELOW_ZERO))
        cannonsHaveBelowZero = true;

    bool const botOnEnemyShip = bot->GetExactDist2d(attackPos) <= MAX_ATTACK_DISTANCE;
    bool const mageAlive = boss && boss->IsAlive() && boss->HasUnitState(UNIT_STATE_CASTING);

    // Assist tank stays on friendly ship to collect and tank adds
    if (botAI->IsAssistTank(bot))
    {
        GuidVector const npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

        // Find nearest uncollected add near hold position
        Unit* targetAdd = nullptr;
        float closestDist = FLT_MAX;

        for (ObjectGuid const& guid : npcs)
        {
            Unit* unit = botAI->GetUnit(guid);
            if (!unit || !unit->IsAlive())
                continue;

            if (unit->GetExactDist2d(waitPos) > HOLD_RADIUS)
                continue;

            Unit* victim = unit->GetVictim();
            bool alreadyTanked = victim && victim->IsPlayer() && botAI->IsTank(victim->ToPlayer());
            if (alreadyTanked)
                continue;

            float dist = bot->GetExactDist2d(unit);
            if (dist < closestDist)
            {
                closestDist = dist;
                targetAdd = unit;
            }
        }

        if (targetAdd)
        {
            // Taunt the add
            if (botAI->CastSpell("taunt", targetAdd))
            {
            }
            else
            {
                switch (bot->getClass())
                {
                    case CLASS_PALADIN:
                        botAI->CastSpell("hand of reckoning", targetAdd);
                        break;
                    case CLASS_DEATH_KNIGHT:
                        botAI->CastSpell("dark command", targetAdd);
                        break;
                    case CLASS_DRUID:
                        botAI->CastSpell("growl", targetAdd);
                        break;
                    default:
                        break;
                }
            }

            if (bot->GetExactDist2d(targetAdd) > 5.0f)
            {
                return MoveTo(bot->GetMapId(), targetAdd->GetPositionX(), targetAdd->GetPositionY(),
                              targetAdd->GetPositionZ(), false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
            }

            bot->SetTarget(targetAdd->GetGUID());
            bot->SetFacingToObject(targetAdd);
            Attack(targetAdd);
            return false;
        }

        // No uncollected adds — keep attacking adds targeting us
        for (ObjectGuid const& guid : npcs)
        {
            Unit* unit = botAI->GetUnit(guid);
            if (!unit || !unit->IsAlive())
                continue;

            if (unit->GetExactDist2d(waitPos) > HOLD_RADIUS)
                continue;

            if (unit->GetVictim() == bot)
            {
                bot->SetTarget(unit->GetGUID());
                bot->SetFacingToObject(unit);
                Attack(unit);
                return false;
            }
        }

        // No adds — return to hold position if drifted
        if (bot->GetExactDist2d(waitPos) > HOLD_RADIUS)
        {
            return MoveTo(bot->GetMapId(), waitPos.GetPositionX(), waitPos.GetPositionY(), waitPos.GetPositionZ(),
                          false, false, false, false, MovementPriority::MOVEMENT_NORMAL);
        }

        return false;
    }

    // Non-assist-tank teleport logic
    float const maxWaitingDistance = (side == GunshipSide::ALLY) ? 30.0f : 45.0f;

     // Non-assist-tank teleport logic
    if (cannonsHaveBelowZero && mageAlive)
    {
        UpdateBossSkullIcon(boss, SKULL_ICON_INDEX);
        if (!botOnEnemyShip)
            return TeleportTo(attackPos);
    }
    else if (!botOnEnemyShip)
    {
        if (bot->GetExactDist2d(waitPos) > MAX_WAITING_DISTANCE)
            return TeleportTo(waitPos);
    }

    return false;
}

bool IccGunshipTeleportAction::TeleportTo(Position const& position)
{
    return bot->TeleportTo(bot->GetMapId(), position.GetPositionX(), position.GetPositionY(), position.GetPositionZ(),
                           bot->GetOrientation());
}

bool IccGunshipTeleportAction::CleanupSkullIcon(uint8 skullIconIndex)
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    ObjectGuid const currentSkullTarget = group->GetTargetIcon(skullIconIndex);
    if (currentSkullTarget.IsEmpty())
        return false;

    Unit* skullTarget = ObjectAccessor::GetUnit(*bot, currentSkullTarget);
    if (!skullTarget || !skullTarget->IsAlive())
        group->SetTargetIcon(skullIconIndex, bot->GetGUID(), ObjectGuid::Empty);

    return false;
}

bool IccGunshipTeleportAction::UpdateBossSkullIcon(Unit* boss, uint8 skullIconIndex)
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    if (group->GetTargetIcon(skullIconIndex) != boss->GetGUID())
        group->SetTargetIcon(skullIconIndex, bot->GetGUID(), boss->GetGUID());

    return false;
}

//DBS
bool IccDbsTankPositionAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "deathbringer saurfang");
    if (!boss)
        return false;

    Unit* beast = AI_VALUE2(Unit*, "find target", "blood beast");

    // Handle tank positioning
    if (botAI->IsTank(bot) && !beast)
    {
        if (bot->GetExactDist2d(ICC_DBS_TANK_POSITION) > 5.0f)
            return MoveTo(bot->GetMapId(), ICC_DBS_TANK_POSITION.GetPositionX(), ICC_DBS_TANK_POSITION.GetPositionY(),
                          ICC_DBS_TANK_POSITION.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_NORMAL);

        // Early return if this tank has Rune of Blood
        if (botAI->GetAura("Rune of Blood", bot))
            return true;
    }

    if (!botAI->IsTank(bot))
    {
        if (CrowdControlBloodBeasts())
            return true;
    }

    // Handle ranged and healer positioning
    if (botAI->IsRanged(bot) || botAI->IsHeal(bot))
    {
        // Handle evasion from blood beasts
        if (EvadeBloodBeasts())
            return true;

        // Position in formation
        return PositionInRangedFormation();
    }

    return false;
}

bool IccDbsTankPositionAction::CrowdControlBloodBeasts()
{
    const std::array<uint32_t, 4> bloodBeastEntries = {NPC_BLOOD_BEAST1, NPC_BLOOD_BEAST2, NPC_BLOOD_BEAST3,
                                                       NPC_BLOOD_BEAST4};
    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    for (auto const& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (!unit || !unit->IsAlive())
            continue;

        // Check if this is a blood beast
        const bool isBloodBeast =
            std::find(bloodBeastEntries.begin(), bloodBeastEntries.end(), unit->GetEntry()) != bloodBeastEntries.end();

        if (!isBloodBeast)
            continue;

        // Apply class-specific CC
        switch (bot->getClass())
        {
            case CLASS_MAGE:
                if (!botAI->HasAura("Frost Nova", unit))
                    botAI->CastSpell("Frost Nova", unit);
                break;
            case CLASS_DRUID:
                if (!botAI->HasAura("Entangling Roots", unit))
                    botAI->CastSpell("Entangling Roots", unit);
                break;
            case CLASS_PALADIN:
                if (!botAI->HasAura("Hammer of Justice", unit))
                    botAI->CastSpell("Hammer of Justice", unit);
                break;
            case CLASS_WARRIOR:
                if (!botAI->HasAura("Hamstring", unit))
                    botAI->CastSpell("Hamstring", unit);
                break;
            case CLASS_HUNTER:
                if (!botAI->HasAura("Concussive Shot", unit))
                    botAI->CastSpell("Concussive Shot", unit);
                break;
            case CLASS_ROGUE:
                if (!botAI->HasAura("Kidney Shot", unit))
                    botAI->CastSpell("Kidney Shot", unit);
                break;
            case CLASS_SHAMAN:
                if (!botAI->HasAura("Frost Shock", unit))
                    botAI->CastSpell("Frost Shock", unit);
                break;
            case CLASS_DEATH_KNIGHT:
                if (!botAI->HasAura("Chains of Ice", unit))
                    botAI->CastSpell("Chains of Ice", unit);
                break;
            case CLASS_PRIEST:
                if (!botAI->HasAura("Psychic Scream", unit))
                    botAI->CastSpell("Psychic Scream", unit);
                break;
            case CLASS_WARLOCK:
                if (!botAI->HasAura("Fear", unit))
                    botAI->CastSpell("Fear", unit);
                break;
            default:
                break;
        }
    }

    return false;
}

bool IccDbsTankPositionAction::EvadeBloodBeasts()
{
    const float evasionDistance = 12.0f;
    const std::array<uint32_t, 4> bloodBeastEntries = {NPC_BLOOD_BEAST1, NPC_BLOOD_BEAST2, NPC_BLOOD_BEAST3, NPC_BLOOD_BEAST4};

    // Get the nearest hostile NPCs
    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    for (auto const& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (!unit)
            continue;

        // Check if this is a blood beast
        const bool isBloodBeast = std::find(bloodBeastEntries.begin(), bloodBeastEntries.end(), unit->GetEntry()) != bloodBeastEntries.end();

        // Only evade if it's a blood beast targeting us
        if (isBloodBeast && unit->GetVictim() == bot)
        {
            float currentDistance = bot->GetDistance2d(unit);

            // Move away if too close
            if (currentDistance < evasionDistance)
                return MoveAway(unit, evasionDistance - currentDistance);
        }
    }

    return false;
}

bool IccDbsTankPositionAction::PositionInRangedFormation()
{
    // Get group
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    // Find this bot's position among ranged/healers in the group
    int rangedIndex = -1;
    int currentIndex = 0;

    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || !member->IsAlive())
            continue;

        if ((botAI->IsRanged(member) || botAI->IsHeal(member)) && !botAI->IsTank(member))
        {
            if (member == bot)
            {
                rangedIndex = currentIndex;
                break;
            }
            currentIndex++;
        }
    }

    if (rangedIndex == -1)
        return false;

    // Fixed positions calculation
    constexpr float tankToBossAngle = 3.14f;
    constexpr float minBossDistance = 11.0f;
    constexpr float spreadDistance = 10.0f;
    constexpr int columnsPerRow = 5;

    // Calculate position in a fixed grid (3 rows x 5 columns)
    int row = rangedIndex / columnsPerRow;
    int col = rangedIndex % columnsPerRow;

    // Calculate base position
    float xOffset = (col - 2) * spreadDistance;                // Center around tank position
    float yOffset = minBossDistance + (row * spreadDistance);  // Each row further back

    // Add zigzag offset for odd rows
    if (row % 2 == 1)
        xOffset += spreadDistance / 2;

    // Rotate position based on tank-to-boss angle
    float finalX =
        ICC_DBS_TANK_POSITION.GetPositionX() + (cos(tankToBossAngle) * yOffset - sin(tankToBossAngle) * xOffset);
    float finalY =
        ICC_DBS_TANK_POSITION.GetPositionY() + (sin(tankToBossAngle) * yOffset + cos(tankToBossAngle) * xOffset);
    float finalZ = ICC_DBS_TANK_POSITION.GetPositionZ();

    // Update Z coordinate
    bot->UpdateAllowedPositionZ(finalX, finalY, finalZ);

    // Move if not in position
    if (bot->GetExactDist2d(finalX, finalY) > 3.0f)
        return MoveTo(bot->GetMapId(), finalX, finalY, finalZ, false, false, false, true,
                      MovementPriority::MOVEMENT_COMBAT);

    return false;
}

bool IccAddsDbsAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "deathbringer saurfang");
    if (!boss)
        return false;

    // This action is only for melee
    if (!botAI->IsMelee(bot))
        return false;

    Unit* priorityTarget = FindPriorityTarget(boss);

    // Update raid target icons if needed
    UpdateSkullMarker(priorityTarget);

    return false;
}

Unit* IccAddsDbsAction::FindPriorityTarget(Unit* boss)
{
    const GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    // Blood beast entry IDs
    constexpr std::array<uint32_t, 4> addEntries = {NPC_BLOOD_BEAST1, NPC_BLOOD_BEAST2, NPC_BLOOD_BEAST3, NPC_BLOOD_BEAST4};

    // First check for alive adds
    for (uint32_t entry : addEntries)
    {
        for (const ObjectGuid& guid : targets)
        {
            Unit* unit = botAI->GetUnit(guid);
            if (unit && unit->IsAlive() && unit->GetEntry() == entry)
                return unit;
        }
    }

    // Only fallback to boss if it's alive
    return boss->IsAlive() ? const_cast<Unit*>(boss) : nullptr;
}

bool IccAddsDbsAction::UpdateSkullMarker(Unit* priorityTarget)
{
    if (!priorityTarget)
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    constexpr uint8_t skullIconId = 7;

    // Get current skull target
    ObjectGuid currentSkull = group->GetTargetIcon(skullIconId);
    Unit* currentSkullUnit = botAI->GetUnit(currentSkull);

    // Determine if skull marker needs updating
    bool needsUpdate = !currentSkullUnit || !currentSkullUnit->IsAlive() || currentSkullUnit != priorityTarget;

    // Update if needed
    if (needsUpdate)
        group->SetTargetIcon(skullIconId, bot->GetGUID(), priorityTarget->GetGUID());

    return false;
}

// Festergut
bool IccFestergutGroupPositionAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "festergut");
    if (!boss)
        return false;

    bot->SetTarget(boss->GetGUID());

    // Handle tank positioning
    if ((botAI->HasAggro(boss) && botAI->IsMainTank(bot)) || botAI->IsAssistTank(bot))
    {
        if (bot->GetExactDist2d(ICC_FESTERGUT_TANK_POSITION) > 5.0f)
            return MoveTo(bot->GetMapId(), ICC_FESTERGUT_TANK_POSITION.GetPositionX(),
                          ICC_FESTERGUT_TANK_POSITION.GetPositionY(), ICC_FESTERGUT_TANK_POSITION.GetPositionZ(), false,
                          false, false, true, MovementPriority::MOVEMENT_NORMAL);
    }

    // Check for spores in the group
    if (HasSporesInGroup())
        return false;

    // Position non-tank ranged and healers
    return PositionNonTankMembers();
}

bool IccFestergutGroupPositionAction::HasSporesInGroup()
{
    const GuidVector members = AI_VALUE(GuidVector, "group members");

    for (auto const& memberGuid : members)
    {
        Unit* unit = botAI->GetUnit(memberGuid);
        if (unit && unit->HasAura(SPELL_GAS_SPORE))
            return true;
    }

    return false;
}

bool IccFestergutGroupPositionAction::PositionNonTankMembers()
{
    // Only position ranged and healers without spores
    if (!(botAI->IsRanged(bot) || botAI->IsHeal(bot)))
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    int positionIndex = CalculatePositionIndex(group);
    if (positionIndex == -1)
        return false;

    // Position calculation parameters
    constexpr float tankToBossAngle = 4.58f;
    constexpr float minBossDistance = 15.0f;
    constexpr float spreadDistance = 10.0f;
    constexpr int columnsPerRow = 6;

    // Calculate grid position
    int row = positionIndex / columnsPerRow;
    int col = positionIndex % columnsPerRow;

    // Calculate base position
    float xOffset = (col - 2) * spreadDistance;                // Center around tank position
    float yOffset = minBossDistance + (row * spreadDistance);  // Each row further back

    // Add zigzag offset for odd rows
    if (row % 2 == 1)
        xOffset += spreadDistance / 2;

    // Rotate position based on tank-to-boss angle
    float finalX =
        ICC_FESTERGUT_TANK_POSITION.GetPositionX() + (cos(tankToBossAngle) * yOffset - sin(tankToBossAngle) * xOffset);
    float finalY =
        ICC_FESTERGUT_TANK_POSITION.GetPositionY() + (sin(tankToBossAngle) * yOffset + cos(tankToBossAngle) * xOffset);
    float finalZ = ICC_FESTERGUT_TANK_POSITION.GetPositionZ();

    // Update Z coordinate
    bot->UpdateAllowedPositionZ(finalX, finalY, finalZ);

    // Move if not in position
    if (bot->GetExactDist2d(finalX, finalY) > 3.0f)
        return MoveTo(bot->GetMapId(), finalX, finalY, finalZ, false, false, false, true,
                      MovementPriority::MOVEMENT_COMBAT);

    return false;
}

int IccFestergutGroupPositionAction::CalculatePositionIndex(Group* group)
{
    std::vector<ObjectGuid> healerGuids;
    std::vector<ObjectGuid> rangedDpsGuids;
    std::vector<ObjectGuid> hunterGuids;

    // Collect all eligible members with their GUIDs
    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || !member->IsAlive() || botAI->IsTank(member))
            continue;

        ObjectGuid memberGuid = member->GetGUID();

        if (botAI->IsHeal(member))
        {
            healerGuids.push_back(memberGuid);
        }
        else if (botAI->IsRanged(member))
        {
            if (member->getClass() == CLASS_HUNTER)
            {
                hunterGuids.push_back(memberGuid);
            }
            else
            {
                rangedDpsGuids.push_back(memberGuid);
            }
        }
    }

    // Sort GUIDs for consistent ordering across all bots
    std::sort(healerGuids.begin(), healerGuids.end());
    std::sort(rangedDpsGuids.begin(), rangedDpsGuids.end());
    std::sort(hunterGuids.begin(), hunterGuids.end());

    ObjectGuid botGuid = bot->GetGUID();

    // Find which group this bot belongs to
    auto healerIt = std::find(healerGuids.begin(), healerGuids.end(), botGuid);
    auto rangedIt = std::find(rangedDpsGuids.begin(), rangedDpsGuids.end(), botGuid);
    auto hunterIt = std::find(hunterGuids.begin(), hunterGuids.end(), botGuid);

    // Calculate global position index considering group constraints
    const int HEALER_ROWS = 2;
    const int COLUMNS_PER_ROW = 6;

    // Healers: rows 0-1 (first two rows)
    if (healerIt != healerGuids.end())
    {
        int healerIndex = static_cast<int>(std::distance(healerGuids.begin(), healerIt));

        // Ensure healers only occupy first two rows
        if (healerIndex < HEALER_ROWS * COLUMNS_PER_ROW)
        {
            return healerIndex;
        }

        // If too many healers, overflow to later rows but keep them early
        return healerIndex;  // Will be in row = index / 6, col = index % 6
    }

    // Non-hunter ranged DPS: can be any row (no strict restriction)
    if (rangedIt != rangedDpsGuids.end())
    {
        int rangedIndex = static_cast<int>(std::distance(rangedDpsGuids.begin(), rangedIt));

        // Start after all healers, then fill remaining spots
        return static_cast<int>(healerGuids.size()) + rangedIndex;
    }

    // Hunters: never in 1st row (row 0)
    if (hunterIt != hunterGuids.end())
    {
        int hunterIndex = static_cast<int>(std::distance(hunterGuids.begin(), hunterIt));

        // Each row holds up to COLUMNS_PER_ROW hunters
        // Skip first row completely: start at offset HEALER_ROWS * COLUMNS_PER_ROW + totalRangedDps

        // Calculate how many non-healer positions are before this hunter position
        int baseOffset = static_cast<int>(healerGuids.size()) + static_cast<int>(rangedDpsGuids.size());

        // Each row of hunters starts at positions that are multiples of COLUMNS_PER_ROW
        // To avoid row 0, skip first column slots reserved for healers/non-hunters
        return baseOffset + hunterIndex;
    }

    return -1;
}

bool IccFestergutSporeAction::Execute(Event /*event*/)
{
    constexpr float POSITION_TOLERANCE = 4.0f;

    // Check if bot has spore
    bool hasSpore = bot->HasAura(SPELL_GAS_SPORE);  // gas spore

    // Stop attacking if spored
    if (hasSpore)
        botAI->Reset();

    // Calculate unique spread position for ranged
    Position spreadRangedPos = CalculateSpreadPosition();

    // Find spored players
    SporeInfo sporeInfo = FindSporedPlayers();

    // Determine target position
    Position targetPos = DetermineTargetPosition(hasSpore, sporeInfo, spreadRangedPos);

    // Move to position if not already there
    if (bot->GetExactDist2d(targetPos) > POSITION_TOLERANCE)
    {
        botAI->Reset();
        return MoveTo(bot->GetMapId(), targetPos.GetPositionX(), targetPos.GetPositionY(), targetPos.GetPositionZ(),
                      true, false, false, true, MovementPriority::MOVEMENT_FORCED);
    }

    return hasSpore;
}

Position IccFestergutSporeAction::CalculateSpreadPosition()
{
    constexpr float SPREAD_RADIUS = 2.0f;

    // Unique angle based on bot's GUID
    float angle = (bot->GetGUID().GetCounter() % 16) * (M_PI / 8);

    Position spreadRangedPos = ICC_FESTERGUT_RANGED_SPORE;
    spreadRangedPos.Relocate(spreadRangedPos.GetPositionX() + cos(angle) * SPREAD_RADIUS,
                             spreadRangedPos.GetPositionY() + sin(angle) * SPREAD_RADIUS,
                             spreadRangedPos.GetPositionZ(),
                             spreadRangedPos.GetOrientation());

    return spreadRangedPos;
}

IccFestergutSporeAction::SporeInfo IccFestergutSporeAction::FindSporedPlayers()
{
    SporeInfo info;
    const GuidVector members = AI_VALUE(GuidVector, "group members");

    for (auto const& memberGuid : members)
    {
        Unit* unit = botAI->GetUnit(memberGuid);
        if (!unit)
            continue;

        if (unit->HasAura(SPELL_GAS_SPORE))
        {
            info.sporedPlayers.push_back(unit);

            if (!info.hasLowestGuid || unit->GetGUID() < info.lowestGuid)
            {
                info.lowestGuid = unit->GetGUID();
                info.hasLowestGuid = true;
            }
        }
    }

    return info;
}

Position IccFestergutSporeAction::DetermineTargetPosition(bool hasSpore, const SporeInfo& sporeInfo, const Position& spreadRangedPos)
{
    // No spores at all
    if (sporeInfo.sporedPlayers.empty())
        return botAI->IsMelee(bot) ? ICC_FESTERGUT_MELEE_SPORE : spreadRangedPos;

    // Bot has no spore, go to standard position
    if (!hasSpore)
        return botAI->IsMelee(bot) ? ICC_FESTERGUT_MELEE_SPORE : spreadRangedPos;

    // Check if main tank has spore
    bool mainTankHasSpore = CheckMainTankSpore();

    // Determine position based on spore logic
    if (botAI->IsMainTank(bot))
        return ICC_FESTERGUT_MELEE_SPORE;

    if (bot->GetGUID() == sporeInfo.lowestGuid && !botAI->IsTank(bot) && !mainTankHasSpore)
        return ICC_FESTERGUT_MELEE_SPORE;

    return spreadRangedPos;
}

bool IccFestergutSporeAction::CheckMainTankSpore()
{
    const GuidVector members = AI_VALUE(GuidVector, "group members");

    for (auto const& memberGuid : members)
    {
        Unit* unit = botAI->GetUnit(memberGuid);
        if (!unit)
            continue;

        if (botAI->IsMainTank(unit->ToPlayer()) && unit->HasAura(SPELL_GAS_SPORE))
            return true;
    }

    return false;
}

// Rotface
bool IccRotfaceTankPositionAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "rotface");
    if (!boss)
        return false;

    Unit* smallOoze = AI_VALUE2(Unit*, "find target", "little ooze");

    MarkBossWithSkull(boss);

    if (botAI->IsMainTank(bot))
    {
        // Check if assist tank is alive
        bool assistTankAlive = false;
        if (Group* group = bot->GetGroup())
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player* member = itr->GetSource();
                if (member && member->IsAlive() && member != bot && botAI->IsAssistTank(member))
                {
                    assistTankAlive = true;
                    break;
                }
            }
        }

        // Check if big ooze is already being handled by assist tank
        Unit* bigOoze = AI_VALUE2(Unit*, "find target", "big ooze");
        bool bigOozeHandled = bigOoze && bigOoze->IsAlive() && bigOoze->GetVictim() &&
                              bigOoze->GetVictim()->IsPlayer() && botAI->IsAssistTank(bigOoze->GetVictim()->ToPlayer());

        if (bigOoze && bigOoze->IsAlive() && !assistTankAlive && !bigOozeHandled)
            return HandleAssistTankPositioning(boss);

        return PositionMainTankAndMelee(boss, smallOoze);
    }

    if (botAI->IsAssistTank(bot))
        return HandleAssistTankPositioning(boss);

    return false;
}

bool IccRotfaceTankPositionAction::MarkBossWithSkull(Unit* boss)
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    constexpr uint8_t skullIconId = 7;
    ObjectGuid skullGuid = group->GetTargetIcon(skullIconId);
    if (skullGuid != boss->GetGUID())
        group->SetTargetIcon(skullIconId, bot->GetGUID(), boss->GetGUID());

    return false;
}

bool IccRotfaceTankPositionAction::PositionMainTankAndMelee(Unit* boss, Unit* smallOoze)
{
    bool isBossCasting = false;
    if (boss && boss->HasUnitState(UNIT_STATE_CASTING) && boss->GetCurrentSpell(SPELL_SLIME_SPRAY))
        isBossCasting = true;

    // MT moves to center only when not kiting or taunting big oozes
    if (bot->GetExactDist2d(ICC_ROTFACE_CENTER_POSITION) > 7.0f && botAI->HasAggro(boss) && botAI->IsMainTank(bot))
    {
        MoveTo(bot->GetMapId(), ICC_ROTFACE_CENTER_POSITION.GetPositionX(), ICC_ROTFACE_CENTER_POSITION.GetPositionY(),
               ICC_ROTFACE_CENTER_POSITION.GetPositionZ(), false, false, false, true,
               MovementPriority::MOVEMENT_COMBAT);
    }

    if (boss && isBossCasting && !botAI->IsTank(bot))
    {
        float x = boss->GetPositionX();
        float y = boss->GetPositionY();
        float z = boss->GetPositionZ();

        if (bot->GetExactDist2d(x, y) > 0.5f)
            MoveTo(bot->GetMapId(), x, y, z, false, false, false, false, MovementPriority::MOVEMENT_FORCED, true,
                   false);
    }

    // MT ignores big oozes — only focus on Rotface unless forced
    if (!isBossCasting && (bot->GetExactDist2d(ICC_ROTFACE_CENTER_POSITION) < 2.0f ||
                           bot->GetExactDist2d(ICC_ROTFACE_CENTER_POSITION) > 7.0f))
    {
        // Skip moving away to help oozes unless explicitly assigned
        if (!botAI->IsAssistTank(bot))
            return false;
    }

    return false;
}

bool IccRotfaceTankPositionAction::HandleAssistTankPositioning(Unit* boss)
{
    GuidVector bigOozes = AI_VALUE(GuidVector, "nearest hostile npcs");
    std::vector<Unit*> activeBigOozes;

    for (auto const& guid : bigOozes)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && unit->GetEntry() == NPC_BIG_OOZE && unit->IsVisible())
            activeBigOozes.push_back(unit);
    }

    if (activeBigOozes.empty())
        return false;

    // Taunt ALL big oozes not currently on us
    for (Unit* ooze : activeBigOozes)
    {
        if (ooze->GetVictim() != bot)
        {
            botAI->CastSpell("taunt", ooze);
            switch (bot->getClass())
            {
                case CLASS_PALADIN:
                    botAI->CastSpell("hand of reckoning", ooze);
                    break;
                case CLASS_DEATH_KNIGHT:
                    botAI->CastSpell("dark command", ooze);
                    break;
                case CLASS_DRUID:
                    botAI->CastSpell("growl", ooze);
                    break;
                default:
                    break;
            }
        }
    }

    // Find closest big ooze to kite
    Unit* targetOoze = nullptr;
    float minDist = FLT_MAX;
    for (Unit* ooze : activeBigOozes)
    {
        float dist = bot->GetExactDist2d(ooze);
        if (dist < minDist)
        {
            minDist = dist;
            targetOoze = ooze;
        }
    }

    if (!targetOoze)
        return false;

    // Move toward ooze if far to get in taunt range
    if (minDist > 20.0f)
    {
        float dx = targetOoze->GetPositionX() - bot->GetPositionX();
        float dy = targetOoze->GetPositionY() - bot->GetPositionY();
        float len = std::hypot(dx, dy);
        float moveX = bot->GetPositionX() + (dx / len) * 5.0f;
        float moveY = bot->GetPositionY() + (dy / len) * 5.0f;
        if (!bot->HasAura(SPELL_NITRO_BOOSTS))
            bot->AddAura(SPELL_NITRO_BOOSTS, bot);
        return MoveTo(bot->GetMapId(), moveX, moveY, bot->GetPositionZ(), false, false, false, false,
                      MovementPriority::MOVEMENT_COMBAT);
    }

    return HandleBigOozeKiting(targetOoze);
}

Unit* IccRotfaceTankPositionAction::FindAssignedBigOoze(Unit* /*boss*/, std::vector<Unit*>& bigOozes)
{
    Group* group = bot->GetGroup();
    if (!group)
        return nullptr;

    Unit* bestOoze = nullptr;
    float minDistance = FLT_MAX;

    for (Unit* ooze : bigOozes)
    {
        if (!ooze || !ooze->IsAlive() || !ooze->IsVisible())
            continue;

        ObjectGuid oozeGuid = ooze->GetGUID();

        // Check whether any other group member already targets this ooze
        bool isAssignedToOther = false;
        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            Player* member = itr->GetSource();
            if (!member || !member->IsInWorld() || member == bot)
                continue;

            Unit* memberTarget = botAI->GetUnit(member->GetTarget());
            if (memberTarget && memberTarget->GetGUID() == oozeGuid)
            {
                isAssignedToOther = true;
                break;
            }
        }

        if (!isAssignedToOther)
        {
            float dist = bot->GetExactDist2d(ooze);
            if (dist < minDistance)
            {
                minDistance = dist;
                bestOoze = ooze;
            }
        }
    }

    return bestOoze;
}

bool IccRotfaceTankPositionAction::HandleBigOozeKiting(Unit* bigOoze)
{
    if (bigOoze->GetVictim() != bot && bigOoze->IsAlive())
    {
        botAI->CastSpell("taunt", bigOoze);
        switch (bot->getClass())
        {
            case CLASS_PALADIN:
                botAI->CastSpell("hand of reckoning", bigOoze);
                break;
            case CLASS_DEATH_KNIGHT:
                botAI->CastSpell("dark command", bigOoze);
                break;
            case CLASS_DRUID:
                botAI->CastSpell("growl", bigOoze);
                break;
            default:
                break;
        }
    }

    float oozeDistance = bot->GetExactDist2d(bigOoze);

    // Stop moving if far enough
    if (oozeDistance > 12.0f)
    {
        bot->SetTarget(bigOoze->GetGUID());
        bot->SetFacingToObject(bigOoze);

        // Always re-taunt even if already on us to maintain aggro during kiting
        botAI->CastSpell("taunt", bigOoze);
        switch (bot->getClass())
        {
            case CLASS_PALADIN:
                botAI->CastSpell("hand of reckoning", bigOoze);
                break;
            case CLASS_DEATH_KNIGHT:
                botAI->CastSpell("dark command", bigOoze);
                break;
            case CLASS_DRUID:
                botAI->CastSpell("growl", bigOoze);
                break;
            default:
                break;
        }

        return true;
    }

    // Kite logic for assigned big ooze
    const float minRadius = 24.0f;
    const float maxRadius = 34.0f;
    const float safeDistanceFromOoze = 13.0f;
    const Position centerPosition = ICC_ROTFACE_CENTER_POSITION;

    float currentDistance = bot->GetExactDist2d(centerPosition);

    // Adjust radius if needed
    if (currentDistance < minRadius || currentDistance > maxRadius)
    {
        float dirX = bot->GetPositionX() - centerPosition.GetPositionX();
        float dirY = bot->GetPositionY() - centerPosition.GetPositionY();
        float length = std::sqrt(dirX * dirX + dirY * dirY);
        dirX /= length;
        dirY /= length;

        float targetX = centerPosition.GetPositionX() + dirX * maxRadius;
        float targetY = centerPosition.GetPositionY() + dirY * maxRadius;

        if (bigOoze->GetExactDist2d(targetX, targetY) >= safeDistanceFromOoze)
            return MoveTo(bot->GetMapId(), targetX, targetY, bot->GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_COMBAT);
    }

    // Circular kiting logic
    float currentAngle =
        atan2(bot->GetPositionY() - centerPosition.GetPositionY(), bot->GetPositionX() - centerPosition.GetPositionX());

    for (int i = 0; i < 16; ++i)
    {
        float angleOffset = (i % 2 == 0 ? 1 : -1) * (M_PI / 16.0f) * (i / 2.0f);
        float newAngle = currentAngle + angleOffset;

        float newX = centerPosition.GetPositionX() + maxRadius * cos(newAngle);
        float newY = centerPosition.GetPositionY() + maxRadius * sin(newAngle);

        if (bigOoze->GetExactDist2d(newX, newY) >= safeDistanceFromOoze)
        {
            GuidVector puddles = AI_VALUE(GuidVector, "nearest hostile npcs");
            bool isSafeFromPuddles = true;

            for (auto const& puddleGuid : puddles)
            {
                Unit* puddle = botAI->GetUnit(puddleGuid);
                if (puddle && botAI->GetAura("Ooze Flood", puddle))
                {
                    float puddleDistance = puddle->GetDistance2d(newX, newY);
                    if (puddleDistance < 30.0f)
                    {
                        isSafeFromPuddles = false;
                        break;
                    }
                }
            }

            if (isSafeFromPuddles)
            {
                MoveTo(bot->GetMapId(), newX, newY, bot->GetPositionZ(), false, false, false, true,
                       MovementPriority::MOVEMENT_COMBAT);
                return true;
            }
        }
    }

    return false;
}

bool IccRotfaceGroupPositionAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "rotface");
    if (!boss)
        return false;

    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    bool hasOozeFlood = botAI->HasAura("Ooze Flood", bot);
    Unit* smallOoze = AI_VALUE2(Unit*, "find target", "little ooze");
    bool hasMutatedInfection = botAI->HasAura("Mutated Infection", bot);

    // Handle puddle avoidance
    if (!botAI->IsTank(bot) && HandlePuddleAvoidance(boss))
        return true;

    // Handle little ooze or mutated infection
    if (HandleOozeTargeting())
        return true;

    // Position ranged and healers
    if (/*!floodPresent && */ !((smallOoze && smallOoze->GetVictim() == bot) || hasMutatedInfection) && !hasOozeFlood && PositionRangedAndHealers(boss, smallOoze))
        return true;

    //if (!hasOozeFlood && bigOoze && bigOoze->IsAlive() && MoveAwayFromBigOoze(bigOoze))
        //return true;

    return false;
}

bool IccRotfaceGroupPositionAction::HandlePuddleAvoidance(Unit* boss)
{
    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    for (auto const& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (!unit || !botAI->HasAura("Ooze Flood", unit))
            continue;

        float puddleDistance = bot->GetExactDist2d(unit);
        float bossDistance = bot->GetExactDist2d(ICC_ROTFACE_CENTER_POSITION);

        if (bossDistance < 15.0f)  // Reduced center distance threshold
            return false;

        if (puddleDistance < 30.0f)
            return MoveAwayFromPuddle(boss, unit, puddleDistance);
    }

    return false;
}

bool IccRotfaceGroupPositionAction::MoveAwayFromPuddle(Unit* boss, Unit* puddle, float)
{
    if (!boss || !puddle)
        return false;

    // Calculate angle and move direction
    float dx = puddle->GetPositionX() - bot->GetPositionX();
    float dy = puddle->GetPositionY() - bot->GetPositionY();
    float angle = atan2(dy, dx);

    // Try to find a valid position in 8 directions
    const float increment = 5.0f;
    const float minPuddleDistance = 30.0f;
    const float minCenterDistance = 15.0f;  // Reduced center distance threshold
    const float maxCenterDistance = 25.0f;  // New maximum center distance threshold
    const int directions = 8;               // Number of directions to check
    float bestX = bot->GetPositionX();
    float bestY = bot->GetPositionY();
    float bestZ = bot->GetPositionZ();
    float maxSafetyScore = 0.0f;

    for (int i = 0; i < directions; ++i)
    {
        float testAngle = angle + (i * M_PI / 4);  // 8 directions (45-degree increments)
        for (float distance = increment; distance <= 35.0f; distance += increment)
        {
            float moveX = bot->GetPositionX() - distance * cos(testAngle);
            float moveY = bot->GetPositionY() - distance * sin(testAngle);
            float moveZ = bot->GetPositionZ();

            // Check distances and line of sight
            float newPuddleDistance = puddle->GetDistance2d(moveX, moveY);
            float newCenterDistance = sqrt(pow(moveX - ICC_ROTFACE_CENTER_POSITION.GetPositionX(), 2) +
                                           pow(moveY - ICC_ROTFACE_CENTER_POSITION.GetPositionY(), 2));

            if (newPuddleDistance >= minPuddleDistance && newCenterDistance >= minCenterDistance &&
                newCenterDistance <= maxCenterDistance && bot->IsWithinLOS(moveX, moveY, moveZ))
            {
                // Calculate safety score (favor positions farther from puddle and center)
                float safetyScore = newPuddleDistance + newCenterDistance;
                if (safetyScore > maxSafetyScore)
                {
                    maxSafetyScore = safetyScore;
                    bestX = moveX;
                    bestY = moveY;
                    bestZ = moveZ;
                }
            }
        }

        // If we are already in a valid position, stop moving
        if (maxSafetyScore > 0.0f && bot->GetExactDist2d(bestX, bestY) <= increment)
            return false;
    }

    // Move to the best position found
    if (maxSafetyScore > 0.0f)
    {
        return MoveTo(bot->GetMapId(), bestX, bestY, bestZ, false, false, false, false,
                      MovementPriority::MOVEMENT_COMBAT);
    }

    return false;
}

bool IccRotfaceGroupPositionAction::HandleOozeTargeting()
{
    Unit* smallOoze = AI_VALUE2(Unit*, "find target", "little ooze");
    bool hasMutatedInfection = botAI->HasAura("Mutated Infection", bot);

    if ((smallOoze && smallOoze->GetVictim() == bot) || hasMutatedInfection)
        return HandleOozeMemberPositioning();

    return false;
}

bool IccRotfaceGroupPositionAction::HandleOozeMemberPositioning()
{
    Unit* bigOoze = AI_VALUE2(Unit*, "find target", "big ooze");

    // First case: No big ooze exists or is not alive, move to designated position
    if (!bigOoze || !bigOoze->IsAlive() || !bigOoze->IsVisible())
    {
        if (bot->GetExactDist2d(ICC_ROTFACE_BIG_OOZE_POSITION) > 3.0f)
        {
            return MoveTo(bot->GetMapId(), ICC_ROTFACE_BIG_OOZE_POSITION.GetPositionX(),
                          ICC_ROTFACE_BIG_OOZE_POSITION.GetPositionY(), ICC_ROTFACE_BIG_OOZE_POSITION.GetPositionZ(),
                          false, false, false, true, MovementPriority::MOVEMENT_FORCED);
        }

        return true;  // Already at position, hold here
    }
    // Second case: Big ooze exists and is alive, move to it for merging
    else if (bot->GetExactDist2d(bigOoze) > 2.0f && bigOoze->IsAlive() && bigOoze->IsVisible())
    {
        float dx = bigOoze->GetPositionX() - bot->GetPositionX();
        float dy = bigOoze->GetPositionY() - bot->GetPositionY();
        float dz = bigOoze->GetPositionZ() - bot->GetPositionZ();
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist > 5.0f)
        {
            dx /= dist;
            dy /= dist;
            float moveX = bot->GetPositionX() + dx * 5.0f;
            float moveY = bot->GetPositionY() + dy * 5.0f;
            float moveZ = bot->GetPositionZ() + (dz / dist) * 5.0f;
            return MoveTo(bot->GetMapId(), moveX, moveY, moveZ, false, false, false, true,
                          MovementPriority::MOVEMENT_FORCED);
        }
        return MoveTo(bot->GetMapId(), bigOoze->GetPositionX(), bigOoze->GetPositionY(), bigOoze->GetPositionZ(), false,
                      false, false, true, MovementPriority::MOVEMENT_FORCED);
    }

    return false;
}

bool IccRotfaceGroupPositionAction::PositionRangedAndHealers(Unit* boss,Unit *smallOoze)
{
    // Only for ranged and healers
    if (!(botAI->IsRanged(bot) || botAI->IsHeal(bot)))
        return false;

    // If we are victim of small ooze, skip positioning entirely
    if (smallOoze && smallOoze->GetVictim() == bot)
        return false;

    Difficulty diff = bot->GetRaidDifficulty();
    bool isBossCasting = false;
    if (boss && boss->HasUnitState(UNIT_STATE_CASTING) && boss->GetCurrentSpell(SPELL_SLIME_SPRAY))
        isBossCasting = true;

    bool isHeroic = (diff == RAID_DIFFICULTY_10MAN_HEROIC || diff == RAID_DIFFICULTY_25MAN_HEROIC);

   // Move to the exact same position as the boss during slime spray
    if (boss && isBossCasting && !isHeroic)
    {
        float x = boss->GetPositionX();
        float y = boss->GetPositionY();
        float z = boss->GetPositionZ();

        // If not already close to the boss's position, move there
        if (bot->GetExactDist2d(x, y) > 0.5f)
        {
            MoveTo(bot->GetMapId(), x, y, z, false, false, false, false, MovementPriority::MOVEMENT_FORCED, true,
                          false);
        }
        // Otherwise, already at the correct position
        return false;
    }

    if (!isHeroic && !isBossCasting && boss && !(bot->getClass() == CLASS_HUNTER) &&
        (bot->GetExactDist2d(boss->GetPositionX(), boss->GetPositionY()) < 2.0f ||
         bot->GetExactDist2d(boss->GetPositionX(), boss->GetPositionY()) > 5.0f))
    {
        MoveTo(bot->GetMapId(), boss->GetPositionX(), boss->GetPositionY(),
                      bot->GetPositionZ(),  false, false, false, true, MovementPriority::MOVEMENT_COMBAT);
    }

    if (!isHeroic)
        return false;

    return FindAndMoveFromClosestMember(boss, smallOoze);
}

bool IccRotfaceGroupPositionAction::FindAndMoveFromClosestMember(Unit* boss, Unit* smallOoze)
{

    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    Unit* puddle = nullptr;

    for (auto const& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (!unit || !botAI->HasAura("Ooze Flood", unit))
            continue;

        puddle = unit;
        break;
    }

    const float safeSpacingRadius = 10.0f;
    const float moveIncrement = 2.0f;
    const float maxMoveDistance = 12.0f;     // Limit maximum movement distance
    const float puddleSafeDistance = 30.0f;  // Minimum distance to stay away from puddle
    const float minCenterDistance = 20.0f;   // Minimum distance from center position

    // Ranged: spread from other members
    const GuidVector members = AI_VALUE(GuidVector, "group members");

    // Calculate a combined vector representing all nearby members' positions
    float totalX = 0.0f;
    float totalY = 0.0f;
    int nearbyCount = 0;

    for (auto const& memberGuid : members)
    {
        Unit* member = botAI->GetUnit(memberGuid);
        if (!member || !member->IsAlive() || member == bot ||
            (smallOoze && smallOoze->GetVictim() == member) ||  // ignore small ooze victims
            (member->IsPlayer() && botAI->IsAssistTank(static_cast<Player*>(member))))
            continue;

        const float distance = bot->GetExactDist2d(member);
        if (distance < safeSpacingRadius)
        {
            // Calculate vector from member to bot
            float dx = bot->GetPositionX() - member->GetPositionX();
            float dy = bot->GetPositionY() - member->GetPositionY();

            // Weight by inverse distance (closer members have more influence)
            float weight = (safeSpacingRadius - distance) / safeSpacingRadius;
            totalX += dx * weight;
            totalY += dy * weight;
            nearbyCount++;
        }
    }

    // If we have nearby members, move away in the combined direction
    if (nearbyCount > 0)
    {
        // Normalize the combined vector
        float magnitude = std::sqrt(totalX * totalX + totalY * totalY);
        if (magnitude > 0.001f)  // Avoid division by zero
        {
            totalX /= magnitude;
            totalY /= magnitude;

            // Calculate move distance based on nearest member
            float moveDistance = std::min(moveIncrement, maxMoveDistance);

            // Create target position in the combined direction
            float targetX = bot->GetPositionX() + totalX * moveDistance;
            float targetY = bot->GetPositionY() + totalY * moveDistance;
            float targetZ = bot->GetPositionZ();  // Maintain current Z

            // Ensure the target position is at least 30 yards away from the puddle
            if (puddle)
            {
                float puddleDistance = puddle->GetDistance2d(targetX, targetY);
                if (puddleDistance < puddleSafeDistance)
                {
                    // Adjust the target position to move further away from the puddle
                    float dx = targetX - puddle->GetPositionX();
                    float dy = targetY - puddle->GetPositionY();
                    float adjustmentFactor = (puddleSafeDistance - puddleDistance) / puddleDistance;
                    targetX += dx * adjustmentFactor;
                    targetY += dy * adjustmentFactor;
                }
            }

            // Ensure the target position is at least 20 yards away from the center position
            const float centerX = ICC_ROTFACE_CENTER_POSITION.GetPositionX();
            const float centerY = ICC_ROTFACE_CENTER_POSITION.GetPositionY();
            float centerDistance = std::sqrt(std::pow(targetX - centerX, 2) + std::pow(targetY - centerY, 2));
            if (centerDistance < minCenterDistance)
            {
                // Adjust the target position to move further away from the center
                float dx = targetX - centerX;
                float dy = targetY - centerY;
                float adjustmentFactor = (minCenterDistance - centerDistance) / centerDistance;
                targetX += dx * adjustmentFactor;
                targetY += dy * adjustmentFactor;
            }

            // Check if the target position is valid and move there
            if (bot->IsWithinLOS(targetX, targetY, targetZ))
            {
                Position targetPos(targetX, targetY, targetZ);
                MoveTo(bot->GetMapId(), targetPos.GetPositionX(), targetPos.GetPositionY(), targetPos.GetPositionZ(),
                       false, false, false, true, MovementPriority::MOVEMENT_NORMAL);
            }
            else
            {
                // If los check fails, try shorter distance
                targetX = bot->GetPositionX() + totalX * (moveDistance * 0.5f);
                targetY = bot->GetPositionY() + totalY * (moveDistance * 0.5f);
                Position targetPos(targetX, targetY, targetZ);
                MoveTo(bot->GetMapId(), targetPos.GetPositionX(), targetPos.GetPositionY(), targetPos.GetPositionZ(),
                       false, false, false, true, MovementPriority::MOVEMENT_NORMAL);
            }
        }
    }

    return false;  // Everyone is properly spaced
}

bool IccRotfaceMoveAwayFromExplosionAction::Execute(Event /*event*/)
{
    // Skip if main tank or ooze flood
    if (botAI->IsMainTank(bot))
        return false;

    botAI->Reset();

    return MoveToRandomSafeLocation();
}

bool IccRotfaceMoveAwayFromExplosionAction::MoveToRandomSafeLocation()
{
    // Generate random angle
    float angle = frand(0, 2 * M_PI);

    // Calculate initial move position
    float moveX = bot->GetPositionX() + 5.0f * cos(angle);
    float moveY = bot->GetPositionY() + 5.0f * sin(angle);
    float moveZ = bot->GetPositionZ();

    // Ensure the position is at least 30 yards away from any puddle
    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto const& npc : npcs)
    {
        Unit* puddle = botAI->GetUnit(npc);
        if (!puddle || !botAI->HasAura("Ooze Flood", puddle))
            continue;

        float puddleDistance = puddle->GetDistance2d(moveX, moveY);
        if (puddleDistance < 30.0f)
        {
            // Adjust the position to move further away from the puddle
            float dx = moveX - puddle->GetPositionX();
            float dy = moveY - puddle->GetPositionY();
            float adjustmentFactor = (30.0f - puddleDistance) / puddleDistance;
            moveX += dx * adjustmentFactor;
            moveY += dy * adjustmentFactor;
        }
    }

    // Ensure the position is at least 30 yards away from the center position
    const Position centerPosition = ICC_ROTFACE_CENTER_POSITION;
    float centerDistance = std::sqrt(std::pow(moveX - centerPosition.GetPositionX(), 2) +
                                     std::pow(moveY - centerPosition.GetPositionY(), 2));
    if (centerDistance < 40.0f)
    {
        // Adjust the position to move further away from the center
        float dx = moveX - centerPosition.GetPositionX();
        float dy = moveY - centerPosition.GetPositionY();
        float adjustmentFactor = (40.0f - centerDistance) / centerDistance;
        moveX += dx * adjustmentFactor;
        moveY += dy * adjustmentFactor;
    }

    // Check line of sight
    if (!bot->IsWithinLOS(moveX, moveY, moveZ))
        return false;

    // Move in increments of 5.0f towards the calculated position
    float currentX = bot->GetPositionX();
    float currentY = bot->GetPositionY();

    float directionX = moveX - currentX;
    float directionY = moveY - currentY;
    float distance = std::sqrt(directionX * directionX + directionY * directionY);

    if (distance > 5.0f)
    {
        directionX /= distance;
        directionY /= distance;

        moveX = currentX + directionX * 5.0f;
        moveY = currentY + directionY * 5.0f;
    }

    // Move to the position
    return MoveTo(bot->GetMapId(), moveX, moveY, moveZ, false, false, false, false, MovementPriority::MOVEMENT_FORCED);
}

// Proffesor Putricide
bool IccPutricideGrowingOozePuddleAction::Execute(Event /*event*/)
{
    Unit* closestPuddle = FindClosestThreateningPuddle();
    if (!closestPuddle)
        return false;

    Position movePosition = CalculateSafeMovePosition(closestPuddle);
    return MoveTo(bot->GetMapId(), movePosition.GetPositionX(), movePosition.GetPositionY(),
                  movePosition.GetPositionZ(), false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
}

Unit* IccPutricideGrowingOozePuddleAction::FindClosestThreateningPuddle()
{
    static const float BASE_RADIUS = 2.0f;
    static const float STACK_MULTIPLIER = 0.6f;
    static const float MIN_DISTANCE = 0.1f;

    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    if (npcs.empty())
        return nullptr;

    Unit* closestPuddle = nullptr;
    float closestDistance = FLT_MAX;

    for (auto const& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (!unit || unit->GetEntry() != NPC_GROWING_OOZE_PUDDLE)
            continue;

        float currentDistance = std::max(MIN_DISTANCE, bot->GetExactDist(unit));
        float safeDistance = BASE_RADIUS;

        if (Aura* grow = unit->GetAura(SPELL_GROW_AURA))
            safeDistance += (grow->GetStackAmount() * STACK_MULTIPLIER);

        if (currentDistance < safeDistance && currentDistance < closestDistance)
        {
            closestDistance = currentDistance;
            closestPuddle = unit;
        }
    }

    return closestPuddle;
}

Position IccPutricideGrowingOozePuddleAction::CalculateSafeMovePosition(Unit* closestPuddle)
{
    static const float BASE_RADIUS = 2.0f;
    static const float STACK_MULTIPLIER = 0.6f;
    static const float BUFFER_DISTANCE = 2.0f;
    static const float MIN_DISTANCE = 0.1f;
    static const int NUM_ANGLES_TO_TEST = 8;

    float botX = bot->GetPositionX();
    float botY = bot->GetPositionY();
    float botZ = bot->GetPositionZ();

    // Calculate current distance and safe distance
    float currentDistance = std::max(MIN_DISTANCE, bot->GetExactDist(closestPuddle));
    float safeDistance = BASE_RADIUS;
    if (Aura* grow = closestPuddle->GetAura(SPELL_GROW_AURA))
        safeDistance += (grow->GetStackAmount() * STACK_MULTIPLIER);

    // Calculate direction vector
    float dx = botX - closestPuddle->GetPositionX();
    float dy = botY - closestPuddle->GetPositionY();
    float dist = std::max(MIN_DISTANCE, sqrt(dx * dx + dy * dy));

    if (dist < MIN_DISTANCE * 2)
    {
        float randomAngle = float(rand()) / float(RAND_MAX) * 2 * M_PI;
        dx = cos(randomAngle);
        dy = sin(randomAngle);
    }
    else
    {
        dx /= dist;
        dy /= dist;
    }

    float moveDistance = safeDistance - currentDistance + BUFFER_DISTANCE;

    // Try different angles to find safe position
    for (int i = 0; i < NUM_ANGLES_TO_TEST; i++)
    {
        float angle = (2 * M_PI * i) / NUM_ANGLES_TO_TEST;
        float rotatedDx = dx * cos(angle) - dy * sin(angle);
        float rotatedDy = dx * sin(angle) + dy * cos(angle);

        float testX = botX + rotatedDx * moveDistance;
        float testY = botY + rotatedDy * moveDistance;

        if (!IsPositionTooCloseToOtherPuddles(testX, testY, closestPuddle) && bot->IsWithinLOS(testX, testY, botZ))
        {
            // If main tank, add 6f to calculated position in the direction away from the puddle
            if (botAI->IsTank(bot))
            {
                float awayDx = testX - closestPuddle->GetPositionX();
                float awayDy = testY - closestPuddle->GetPositionY();
                float awayDist = std::sqrt(awayDx * awayDx + awayDy * awayDy);
                if (awayDist > 0.001f)
                {
                    awayDx /= awayDist;
                    awayDy /= awayDist;
                    testX += awayDx * 6.0f;
                    testY += awayDy * 6.0f;
                }
            }
            return Position(testX, testY, botZ);
        }
    }

    // Fallback position if no safe position found
    float fallbackX = botX + dx * moveDistance;
    float fallbackY = botY + dy * moveDistance;
    if (botAI->IsTank(bot))
    {
        float awayDx = fallbackX - closestPuddle->GetPositionX();
        float awayDy = fallbackY - closestPuddle->GetPositionY();
        float awayDist = std::sqrt(awayDx * awayDx + awayDy * awayDy);
        if (awayDist > 0.001f)
        {
            awayDx /= awayDist;
            awayDy /= awayDist;
            fallbackX += awayDx * 6.0f;
            fallbackY += awayDy * 6.0f;
        }
    }
    return Position(fallbackX, fallbackY, botZ);
}

bool IccPutricideGrowingOozePuddleAction::IsPositionTooCloseToOtherPuddles(float x, float y, Unit* ignorePuddle)
{
    static const float BASE_RADIUS = 2.0f;
    static const float STACK_MULTIPLIER = 0.6f;

    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto const& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (!unit || unit == ignorePuddle || unit->GetEntry() != NPC_GROWING_OOZE_PUDDLE)
            continue;

        float safeDistance = BASE_RADIUS;
        if (Aura* grow = unit->GetAura(SPELL_GROW_AURA))
            safeDistance += (grow->GetStackAmount() * STACK_MULTIPLIER);

        float dist = unit->GetDistance2d(x, y);
        if (dist < safeDistance)
            return true;
    }

    return false;
}

bool IccPutricideVolatileOozeAction::Execute(Event /*event*/)
{
    static const float STACK_DISTANCE = 7.0f;

    Unit* ooze = AI_VALUE2(Unit*, "find target", "volatile ooze");
    if (!ooze)
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "professor putricide");
    if (!boss)
        return false;

    // Main tank handling
    if (botAI->IsMainTank(bot) && bot->GetExactDist2d(ICC_PUTRICIDE_TANK_POSITION) > 20.0f &&
        !boss->HealthBelowPct(36) && boss->GetVictim() == bot)
        return MoveTo(bot->GetMapId(), ICC_PUTRICIDE_TANK_POSITION.GetPositionX(), ICC_PUTRICIDE_TANK_POSITION.GetPositionY(),
               ICC_PUTRICIDE_TANK_POSITION.GetPositionZ(), false, false, false, true, MovementPriority::MOVEMENT_COMBAT, true, false);

    // Skip if we have forbidden auras
    if (botAI->HasAura("Gaseous Bloat", bot) || botAI->HasAura("Unbound Plague", bot))
        return false;

    // Find all alive oozes
    std::vector<Unit*> aliveOozes;
    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && unit->GetEntry() == ooze->GetEntry())
            aliveOozes.push_back(unit);
    }

    // If more than one ooze is alive, kill all but one
    if (aliveOozes.size() > 1)
    {
        for (size_t i = 0; i < aliveOozes.size() - 1; ++i)
        {
            bot->Kill(bot, aliveOozes[i]);
        }
    }

    // Mark ooze with skull
    MarkOozeWithSkull(ooze);

    // Melee handling (non-tanks)
    if (botAI->IsMelee(bot) && !botAI->IsMainTank(bot))
    {
        bot->SetTarget(ooze->GetGUID());
        bot->SetFacingToObject(ooze);
        if (bot->IsWithinMeleeRange(ooze))
            return Attack(ooze);
    }

    // Ranged/healer handling
    if (botAI->IsRanged(bot) || botAI->IsHeal(bot))
    {
        Unit* stackTarget = FindAuraTarget();
        if (stackTarget && bot->GetDistance2d(stackTarget) > STACK_DISTANCE)
        {
            return MoveTo(bot->GetMapId(), stackTarget->GetPositionX(), stackTarget->GetPositionY(),
                          stackTarget->GetPositionZ(), false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
        }

        if (ooze && !botAI->IsHeal(bot) && stackTarget && bot->GetDistance2d(stackTarget) <= STACK_DISTANCE)
        {
            bot->SetTarget(ooze->GetGUID());
            bot->SetFacingToObject(ooze);
            if (bot->IsWithinRange(ooze, 25.0f))
                return Attack(ooze);
        }
    }

    return false;
}

bool IccPutricideVolatileOozeAction::MarkOozeWithSkull(Unit* ooze)
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    constexpr uint8_t skullIconId = 7;
    ObjectGuid skullGuid = group->GetTargetIcon(skullIconId);
    Unit* markedUnit = botAI->GetUnit(skullGuid);

    // Clear dead marks or marks that are not on ooze
    if (markedUnit && (!markedUnit->IsAlive() || (ooze && markedUnit != ooze)))
        group->SetTargetIcon(skullIconId, bot->GetGUID(), ObjectGuid::Empty);

    // Mark alive ooze if needed
    if (ooze && ooze->IsAlive() && (!skullGuid || !markedUnit))
        group->SetTargetIcon(skullIconId, bot->GetGUID(), ooze->GetGUID());

    return false;
}

Unit* IccPutricideVolatileOozeAction::FindAuraTarget()
{
    Group* group = bot->GetGroup();
    if (!group)
        return nullptr;

    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || !member->IsAlive() || member == bot)
            continue;

        if (botAI->HasAura("Volatile Ooze Adhesive", member))
            return member;
    }

    return nullptr;
}

bool IccPutricideGasCloudAction::Execute(Event /*event*/)
{
    Unit* gasCloud = AI_VALUE2(Unit*, "find target", "gas cloud");
    if (!gasCloud)
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "professor putricide");
    if (!boss)
        return false;

    // Tank positioning logic
    if (botAI->IsTank(bot) && bot->GetExactDist2d(ICC_PUTRICIDE_TANK_POSITION) > 20.0f && !boss->HealthBelowPct(36) &&
        boss->GetVictim() == bot)
        return MoveTo(bot->GetMapId(), ICC_PUTRICIDE_TANK_POSITION.GetPositionX(),
                      ICC_PUTRICIDE_TANK_POSITION.GetPositionY(), ICC_PUTRICIDE_TANK_POSITION.GetPositionZ(), false,
                      false, false, true, MovementPriority::MOVEMENT_COMBAT, true, false);

    if (botAI->IsMainTank(bot))
        return false;

    bool hasGaseousBloat = botAI->HasAura("Gaseous Bloat", bot);
    Unit* volatileOoze = AI_VALUE2(Unit*, "find target", "volatile ooze");

    // Find all alive gasCloud
    std::vector<Unit*> aliveGasCloud;
    const GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && unit->GetEntry() == gasCloud->GetEntry())
            aliveGasCloud.push_back(unit);
    }

    // If more than one GasCloud is alive, kill all but one
    if (aliveGasCloud.size() > 1)
    {
        for (size_t i = 0; i < aliveGasCloud.size() - 1; ++i)
        {
            bot->Kill(bot, aliveGasCloud[i]);
        }
    }

    // Skip if we have no aura but ooze exists
    if (!hasGaseousBloat && volatileOoze)
        return false;

    if (hasGaseousBloat)
        return HandleGaseousBloatMovement(gasCloud);

    return HandleGroupAuraSituation(gasCloud);
}

bool IccPutricideGasCloudAction::HandleGaseousBloatMovement(Unit* gasCloud)
{
    bool hasGaseousBloat = botAI->HasAura("Gaseous Bloat", bot);

    if (!hasGaseousBloat)
        return false;

    if (hasGaseousBloat && !bot->HasAura(SPELL_NITRO_BOOSTS))
        bot->AddAura(SPELL_NITRO_BOOSTS, bot);  // to make it a bit easier when abo fails to slow or bots take forever to kill oozes

    static const int NUM_ANGLES = 32;  // Increased from 16 for better corner escape
    static const float MIN_SAFE_DISTANCE = 35.0f;
    static const float EMERGENCY_DISTANCE = 15.0f;
    static const float GAS_BOMB_SAFE_DIST = 6.0f;
    static const float MOVEMENT_INCREMENT = 5.0f;  // Fixed movement increment

    Position botPos = bot->GetPosition();
    Position cloudPos = gasCloud->GetPosition();
    float cloudDist = gasCloud->GetExactDist2d(botPos);

    if (cloudDist >= MIN_SAFE_DISTANCE)
        return false;

    // Gather all choking gas bombs
    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    std::vector<Unit*> gasBombs;
    for (auto const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && unit->GetEntry() == NPC_CHOKING_GAS_BOMB)
            gasBombs.push_back(unit);
    }

    // Calculate direction away from cloud
    float dx = botPos.GetPositionX() - cloudPos.GetPositionX();
    float dy = botPos.GetPositionY() - cloudPos.GetPositionY();
    float dist = std::max(0.1f, sqrt(dx * dx + dy * dy));

    if (dist <= 0)
        return false;

    dx /= dist;
    dy /= dist;

    // Try to find safe movement position with strict corner avoidance
    Position bestPos;
    bool foundPath = false;
    float bestScore = 0.0f;

    for (int i = 0; i < NUM_ANGLES; i++)
    {
        float angle = (2 * M_PI * i) / NUM_ANGLES;
        float rotatedDx = dx * cos(angle) - dy * sin(angle);
        float rotatedDy = dx * sin(angle) + dy * cos(angle);

        // Only test positions at fixed increments of 5.0f
        for (float testDist = MOVEMENT_INCREMENT; testDist <= 20.0f; testDist += MOVEMENT_INCREMENT)
        {
            float testX = botPos.GetPositionX() + rotatedDx * testDist;
            float testY = botPos.GetPositionY() + rotatedDy * testDist;
            float testZ = botPos.GetPositionZ();
            Position testPos(testX, testY, testZ);

            float newCloudDist = cloudPos.GetExactDist2d(testPos);

            // Check gas bomb distance
            float minGasBombDist = FLT_MAX;
            for (Unit* bomb : gasBombs)
            {
                float bombDist = bomb->GetDistance2d(testX, testY);
                if (bombDist < minGasBombDist)
                    minGasBombDist = bombDist;
            }

            if (newCloudDist > cloudDist && minGasBombDist >= GAS_BOMB_SAFE_DIST &&
                bot->IsWithinLOS(testX, testY, testZ))
            {
                // Strict corner avoidance - test movement freedom thoroughly
                int freeDirections = 0;
                static const int CHECK_DIRS = 16;      // More directions for better detection
                static const float CHECK_DIST = 8.0f;  // Longer distance to detect walls/corners early

                for (int j = 0; j < CHECK_DIRS; j++)
                {
                    float checkAngle = (2 * M_PI * j) / CHECK_DIRS;
                    float checkX = testX + cos(checkAngle) * CHECK_DIST;
                    float checkY = testY + sin(checkAngle) * CHECK_DIST;
                    if (bot->IsWithinLOS(checkX, checkY, testZ))
                        freeDirections++;
                }

                float freedomScore = (float)freeDirections / CHECK_DIRS;

                // STRICT: Reject any position that looks like a corner or restricted area
                // Require at least 80% of directions to be free (13 out of 16)
                if (freedomScore < 0.8f)
                    continue;  // Skip this position entirely

                // Also check for "dead ends" - positions that might lead to corners
                // Test if we can move further in the same direction
                bool canContinueMoving = true;
                float continueX = testX + rotatedDx * MOVEMENT_INCREMENT;
                float continueY = testY + rotatedDy * MOVEMENT_INCREMENT;
                if (!bot->IsWithinLOS(continueX, continueY, testZ))
                    canContinueMoving = false;

                // Bonus for positions that allow continued movement in the same direction
                float continuityBonus = canContinueMoving ? 5.0f : 0.0f;

                float combinedScore = newCloudDist + (freedomScore * 15.0f) + minGasBombDist + continuityBonus;

                if (!foundPath || combinedScore > bestScore)
                {
                    bestPos = testPos;
                    bestScore = combinedScore;
                    foundPath = true;
                }
            }
        }
    }

    if (foundPath)
    {
        botAI->Reset();
        return MoveTo(bot->GetMapId(), bestPos.GetPositionX(), bestPos.GetPositionY(), bestPos.GetPositionZ(), false,
                      false, false, false, MovementPriority::MOVEMENT_COMBAT);
    }
    // Emergency movement - only when very close to cloud
    if (cloudDist < EMERGENCY_DISTANCE)
    {
        Position emergencyPos = CalculateEmergencyPosition(botPos, dx, dy);

        // Even for emergency, avoid corners
        float minEmergencyGasBombDist = FLT_MAX;
        for (Unit* bomb : gasBombs)
        {
            float bombDist = bomb->GetDistance2d(emergencyPos.GetPositionX(), emergencyPos.GetPositionY());
            if (bombDist < minEmergencyGasBombDist)
                minEmergencyGasBombDist = bombDist;
        }

        if (minEmergencyGasBombDist >= GAS_BOMB_SAFE_DIST &&
            bot->IsWithinLOS(emergencyPos.GetPositionX(), emergencyPos.GetPositionY(), emergencyPos.GetPositionZ()))
        {
            botAI->Reset();
            return MoveTo(bot->GetMapId(), emergencyPos.GetPositionX(), emergencyPos.GetPositionY(),
                          emergencyPos.GetPositionZ(), false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
        }
    }

    return false;
}

bool IccPutricideGasCloudAction::FindSafeMovementPosition(const Position& botPos, const Position& cloudPos, float dx,
                                                          float dy, int numAngles, Position& resultPos)
{
    float bestScore = 0.0f;
    bool foundPath = false;
    resultPos = botPos;
    static const float MOVEMENT_INCREMENT = 5.0f;  // Fixed movement increment

    for (int i = 0; i < numAngles; i++)
    {
        float angle = (2 * M_PI * i) / numAngles;
        float rotatedDx = dx * cos(angle) - dy * sin(angle);
        float rotatedDy = dx * sin(angle) + dy * cos(angle);

        for (float testDist = MOVEMENT_INCREMENT; testDist <= 20.0f; testDist += MOVEMENT_INCREMENT)
        {
            Position testPos(botPos.GetPositionX() + rotatedDx * testDist, botPos.GetPositionY() + rotatedDy * testDist,
                             botPos.GetPositionZ());

            float newCloudDist = cloudPos.GetExactDist2d(testPos);
            if (newCloudDist > cloudPos.GetExactDist2d(botPos) &&
                bot->IsWithinLOS(testPos.GetPositionX(), testPos.GetPositionY(), testPos.GetPositionZ()))
            {
                // Strict corner prevention - test movement freedom thoroughly
                int freeDirections = 0;
                static const int CHECK_DIRECTIONS = 16;
                static const float CHECK_DISTANCE = 8.0f;

                for (int j = 0; j < CHECK_DIRECTIONS; j++)
                {
                    float checkAngle = (2 * M_PI * j) / CHECK_DIRECTIONS;
                    float checkX = testPos.GetPositionX() + cos(checkAngle) * CHECK_DISTANCE;
                    float checkY = testPos.GetPositionY() + sin(checkAngle) * CHECK_DISTANCE;
                    if (bot->IsWithinLOS(checkX, checkY, testPos.GetPositionZ()))
                        freeDirections++;
                }

                float freedomScore = (float)freeDirections / CHECK_DIRECTIONS;

                // REJECT positions that could lead to corners - require high freedom
                if (freedomScore < 0.8f)  // 80% of directions must be free
                    continue;

                // Check if we can continue moving in the same direction (avoid dead ends)
                float continueX = testPos.GetPositionX() + rotatedDx * MOVEMENT_INCREMENT;
                float continueY = testPos.GetPositionY() + rotatedDy * MOVEMENT_INCREMENT;
                bool canContinue = bot->IsWithinLOS(continueX, continueY, testPos.GetPositionZ());

                float continuityBonus = canContinue ? 3.0f : 0.0f;
                float combinedScore = newCloudDist + (freedomScore * 12.0f) + continuityBonus;

                if (!foundPath || combinedScore > bestScore)
                {
                    resultPos = testPos;
                    bestScore = combinedScore;
                    foundPath = true;
                }
            }
        }
    }

    return foundPath;
}

Position IccPutricideGasCloudAction::CalculateEmergencyPosition(const Position& botPos, float dx, float dy)
{
    // For emergency, still try to avoid corners but prioritize getting away from immediate danger
    Position bestPos =
        Position(botPos.GetPositionX() + dx * 15.0f, botPos.GetPositionY() + dy * 15.0f, botPos.GetPositionZ());
    float bestFreedom = 0.0f;

    // Try fewer directions for emergency but still avoid corners
    for (int i = 0; i < 8; i++)
    {
        float angle = (2 * M_PI * i) / 8;
        float rotatedDx = dx * cos(angle) - dy * sin(angle);
        float rotatedDy = dx * sin(angle) + dy * cos(angle);

        Position testPos(botPos.GetPositionX() + rotatedDx * 15.0f, botPos.GetPositionY() + rotatedDy * 15.0f,
                         botPos.GetPositionZ());

        if (bot->IsWithinLOS(testPos.GetPositionX(), testPos.GetPositionY(), testPos.GetPositionZ()))
        {
            // Quick freedom check for emergency
            int freeDirections = 0;
            static const int EMERGENCY_CHECK_DIRS = 8;
            static const float EMERGENCY_CHECK_DIST = 6.0f;

            for (int j = 0; j < EMERGENCY_CHECK_DIRS; j++)
            {
                float checkAngle = (2 * M_PI * j) / EMERGENCY_CHECK_DIRS;
                float checkX = testPos.GetPositionX() + cos(checkAngle) * EMERGENCY_CHECK_DIST;
                float checkY = testPos.GetPositionY() + sin(checkAngle) * EMERGENCY_CHECK_DIST;
                if (bot->IsWithinLOS(checkX, checkY, testPos.GetPositionZ()))
                    freeDirections++;
            }

            float freedom = (float)freeDirections / EMERGENCY_CHECK_DIRS;

            // For emergency, accept positions with at least 60% freedom
            if (freedom >= 0.6f && freedom > bestFreedom)
            {
                bestPos = testPos;
                bestFreedom = freedom;
            }
        }
    }

    return bestPos;
}

bool IccPutricideGasCloudAction::HandleGroupAuraSituation(Unit* gasCloud)
{
    Group* group = bot->GetGroup();
    if (!group || botAI->IsHeal(bot))
        return false;

    // Mark gas cloud with skull if no volatile ooze is present or alive
    Unit* volatileOoze = AI_VALUE2(Unit*, "find target", "volatile ooze");
    if ((!volatileOoze || !volatileOoze->IsAlive()) && gasCloud && gasCloud->IsAlive())
    {
        Group* group = bot->GetGroup();
        if (group)
        {
            constexpr uint8_t skullIconId = 7;
            ObjectGuid currentSkull = group->GetTargetIcon(skullIconId);
            Unit* markedUnit = botAI->GetUnit(currentSkull);
            if (!markedUnit || !markedUnit->IsAlive() || markedUnit != gasCloud)
                group->SetTargetIcon(skullIconId, bot->GetGUID(), gasCloud->GetGUID());
        }
    }

    float currentDist = gasCloud ? bot->GetDistance(gasCloud) : 0.0f;
    const float MIN_SAFE_DISTANCE = 15.0f;

    // Always maintain minimum distance when group doesn't have bloat
    if (!GroupHasGaseousBloat(group) && gasCloud && currentDist < MIN_SAFE_DISTANCE)
    {
        // Move away from gas cloud
        float angle = bot->GetAngle(gasCloud);
        float x = bot->GetPositionX() + cos(angle) * -MIN_SAFE_DISTANCE;
        float y = bot->GetPositionY() + sin(angle) * -MIN_SAFE_DISTANCE;
        return MoveTo(bot->GetMapId(), x, y, bot->GetPositionZ(), false, false, false, false,
                      MovementPriority::MOVEMENT_COMBAT);
    }

    if (GroupHasGaseousBloat(group) && gasCloud)
    {
        bot->SetTarget(gasCloud->GetGUID());
        bot->SetFacingToObject(gasCloud);

        // Attack logic for group with Gaseous Bloat
        if (botAI->IsRanged(bot))
        {
            // For ranged attackers, maintain optimal distance (15-25 yards)
            if (currentDist > 25.0f)
            {
                // Move closer if too far
                return MoveTo(bot->GetMapId(), gasCloud->GetPositionX(), gasCloud->GetPositionY(),
                              gasCloud->GetPositionZ(), false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
            }
            else if (currentDist < MIN_SAFE_DISTANCE)
            {
                // Move away if too close (but stay closer than normal safe distance since we need to attack)
                float angle = bot->GetAngle(gasCloud);
                float x = bot->GetPositionX() + cos(angle) * -12.0f;
                float y = bot->GetPositionY() + sin(angle) * -12.0f;
                return MoveTo(bot->GetMapId(), x, y, bot->GetPositionZ(), false, false, false, false,
                              MovementPriority::MOVEMENT_COMBAT);
            }
            else
            {
                // Attack if at good range
                return Attack(gasCloud);
            }
        }
        else if (botAI->IsMelee(bot) && !botAI->IsTank(bot))
        {
            // For melee attackers, move to attack range (0-5 yards)
            if (currentDist > 5.0f)
            {
                return MoveTo(bot->GetMapId(), gasCloud->GetPositionX(), gasCloud->GetPositionY(),
                              gasCloud->GetPositionZ(), false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
            }
            else
                return Attack(gasCloud);
        }
    }

    return false;
}

bool IccPutricideGasCloudAction::GroupHasGaseousBloat(Group* group)
{
    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (member && botAI->HasAura("Gaseous Bloat", member))
            return true;
    }
    return false;
}

bool IccPutricideAvoidMalleableGooAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "professor putricide");
    if (!boss)
        return false;

    // Tank handling for choking gas bomb
    if (HandleTankPositioning(boss))
        return true;

    // Skip if volatile ooze or gas cloud exists
    if (AI_VALUE2(Unit*, "find target", "volatile ooze") || AI_VALUE2(Unit*, "find target", "gas cloud"))
        return false;

    // Handle unbound plague movement
    if (HandleUnboundPlague(boss))
        return true;

    // Handle ranged/melee positioning
    return HandleBossPositioning(boss);
}

bool IccPutricideAvoidMalleableGooAction::HandleTankPositioning(Unit*)
{
    if (!botAI->IsTank(bot))
        return false;

    Unit* bomb = bot->FindNearestCreature(NPC_CHOKING_GAS_BOMB, 100.0f);
    if (!bomb)
        return false;

    const float safeDistance = 15.0f;
    float currentDistance = bot->GetDistance2d(bomb);

    if (currentDistance < safeDistance)
        return MoveAway(bomb, safeDistance - currentDistance);

    return false;
}

bool IccPutricideAvoidMalleableGooAction::HandleUnboundPlague(Unit* boss)
{
    if (boss && boss->HealthBelowPct(35))
        return false;

    if (!botAI->HasAura("Unbound Plague", bot))
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    const float UNBOUND_PLAGUE_DISTANCE = 20.0f;
    float closestDistance = UNBOUND_PLAGUE_DISTANCE;
    Unit* closestPlayer = nullptr;

    // Find closest player
    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || !member->IsAlive() || member == bot)
            continue;

        float dist = bot->GetDistance2d(member);
        if (dist < closestDistance)
        {
            closestDistance = dist;
            closestPlayer = member;
        }
    }

    if (!closestPlayer || closestDistance >= UNBOUND_PLAGUE_DISTANCE)
    {
        bot->Kill(bot, bot);
        return true;
    }

    // Calculate move away position
    float dx = bot->GetPositionX() - closestPlayer->GetPositionX();
    float dy = bot->GetPositionY() - closestPlayer->GetPositionY();
    float dist = sqrt(dx * dx + dy * dy);

    if (dist <= 0)
        return false;

    dx /= dist;
    dy /= dist;
    float moveDistance = UNBOUND_PLAGUE_DISTANCE - closestDistance + 2.0f;

    float moveX = bot->GetPositionX() + dx * moveDistance;
    float moveY = bot->GetPositionY() + dy * moveDistance;

    if (bot->IsWithinLOS(moveX, moveY, bot->GetPositionZ()))
    {
        return MoveTo(bot->GetMapId(), moveX, moveY, bot->GetPositionZ(), false, false, false, false,
                      MovementPriority::MOVEMENT_COMBAT);
    }

    return false;
}

bool IccPutricideAvoidMalleableGooAction::HandleBossPositioning(Unit* boss)
{
    if (botAI->IsTank(bot))
        return false;

    // If boss is close to putricide_bad_position, all non-tank bots should be 1f in front of boss
    const float BAD_POS_THRESHOLD = 10.0f;
    const float IN_FRONT_DISTANCE = 1.0f;
    float bossToBadPos = boss->GetExactDist2d(ICC_PUTRICIDE_BAD_POSITION.GetPositionX(), ICC_PUTRICIDE_BAD_POSITION.GetPositionY());

    if (bossToBadPos <= BAD_POS_THRESHOLD)
    {
        // Move to 1f in front of boss
        float bossOrientation = boss->GetOrientation();
        float targetX = boss->GetPositionX() + cos(bossOrientation) * IN_FRONT_DISTANCE;
        float targetY = boss->GetPositionY() + sin(bossOrientation) * IN_FRONT_DISTANCE;
        float targetZ = boss->GetPositionZ();

        // Only move if not already close enough
        if (bot->GetExactDist2d(targetX, targetY) > 0.5f)
        {
            bot->SetFacingToObject(boss);
            return MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, botAI->IsRanged(bot),
                          MovementPriority::MOVEMENT_COMBAT);
        }
        return false;
    }

    float distToBoss = bot->GetExactDist2d(boss);
    bool isRanged = botAI->IsRanged(bot);

    // Calculate desired position in front of boss
    float desiredDistance =
        isRanged ? ((bot->getClass() == CLASS_HUNTER) ? 14.0f : 6.0f) : (distToBoss < 2.0f ? 3.0f : 5.0f);

    // Check if we need to move
    if ((std::abs(distToBoss - desiredDistance) > 0.5f || !boss->isInFront(bot)) &&
        (!isRanged || (isRanged && !botAI->IsTank(bot))))
    {
        Position targetPos = CalculateBossPosition(boss, desiredDistance);

        // Check for obstacles
        if (HasObstacleBetween(bot->GetPosition(), targetPos))
        {
            // Use arc movement to navigate around obstacles
            Position arcPoint = CalculateArcPoint(bot->GetPosition(), targetPos, boss->GetPosition());

            if (bot->GetExactDist2d(arcPoint) > 1.0f)
            {
                bot->SetFacingToObject(boss);
                return MoveTo(bot->GetMapId(), arcPoint.GetPositionX(), arcPoint.GetPositionY(),
                              arcPoint.GetPositionZ(), false, false, false, isRanged,
                              MovementPriority::MOVEMENT_COMBAT);
            }
        }
        else
        {
            // No obstacles, move in increments directly toward target
            Position adjustedTarget = CalculateIncrementalMove(bot->GetPosition(), targetPos, 2.0f);
            bot->SetFacingToObject(boss);
            return MoveTo(bot->GetMapId(), adjustedTarget.GetPositionX(), adjustedTarget.GetPositionY(),
                          adjustedTarget.GetPositionZ(), false, false, false, isRanged,
                          MovementPriority::MOVEMENT_COMBAT);
        }
    }

    return false;
}

Position IccPutricideAvoidMalleableGooAction::CalculateBossPosition(Unit* boss, float distance)
{
    float bossOrientation = boss->GetOrientation();
    return Position(boss->GetPositionX() + cos(bossOrientation) * distance,
                    boss->GetPositionY() + sin(bossOrientation) * distance, boss->GetPositionZ());
}

bool IccPutricideAvoidMalleableGooAction::HasObstacleBetween(const Position& from, const Position& to)
{
    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto const& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (!unit || !unit->IsAlive())
            continue;

        if (unit->GetEntry() == NPC_GROWING_OOZE_PUDDLE || unit->GetEntry() == NPC_CHOKING_GAS_BOMB)
        {
            if (IsOnPath(from, to, unit->GetPosition(), 3.0f))
                return true;
        }
    }
    return false;
}

bool IccPutricideAvoidMalleableGooAction::IsOnPath(const Position& from, const Position& to, const Position& point,
                                                   float threshold)
{
    float pathX = to.GetPositionX() - from.GetPositionX();
    float pathY = to.GetPositionY() - from.GetPositionY();
    float pathLen = std::sqrt(pathX * pathX + pathY * pathY);

    if (pathLen < 0.1f)
        return false;

    float normX = pathX / pathLen;
    float normY = pathY / pathLen;

    float toPointX = point.GetPositionX() - from.GetPositionX();
    float toPointY = point.GetPositionY() - from.GetPositionY();
    float proj = toPointX * normX + toPointY * normY;

    if (proj < 0 || proj > pathLen)
        return false;

    float closestX = from.GetPositionX() + normX * proj;
    float closestY = from.GetPositionY() + normY * proj;
    float distToPath = std::sqrt((point.GetPositionX() - closestX) * (point.GetPositionX() - closestX) +
                                 (point.GetPositionY() - closestY) * (point.GetPositionY() - closestY));

    return distToPath < threshold;
}

Position IccPutricideAvoidMalleableGooAction::CalculateArcPoint(const Position& current, const Position& target, const Position& center)
{
    // Calculate vectors from center to current position and target
    float currentX = current.GetPositionX() - center.GetPositionX();
    float currentY = current.GetPositionY() - center.GetPositionY();
    float targetX = target.GetPositionX() - center.GetPositionX();
    float targetY = target.GetPositionY() - center.GetPositionY();

    // Calculate distances
    float currentDist = std::sqrt(currentX * currentX + currentY * currentY);
    float targetDist = std::sqrt(targetX * targetX + targetY * targetY);

    // Normalize vectors
    currentX /= currentDist;
    currentY /= currentDist;
    targetX /= targetDist;
    targetY /= targetDist;

    // Calculate dot product to find the angle between vectors
    float dotProduct = currentX * targetX + currentY * targetY;
    dotProduct = std::max(-1.0f, std::min(1.0f, dotProduct));  // Clamp to [-1, 1]
    float angle = std::acos(dotProduct);

    // Determine rotation direction (clockwise or counterclockwise)
    float crossProduct = currentX * targetY - currentY * targetX;
    float stepAngle = angle * 0.25f;  // Move 25% along the arc

    if (crossProduct < 0)
        stepAngle = -stepAngle;  // Clockwise

    // Calculate rotation matrix components
    float cos_a = std::cos(stepAngle);
    float sin_a = std::sin(stepAngle);

    // Rotate current vector
    float rotatedX = currentX * cos_a - currentY * sin_a;
    float rotatedY = currentX * sin_a + currentY * cos_a;

    // Scale to match the target distance for smoother approach
    float desiredDist = currentDist * 0.9f + targetDist * 0.1f;

    // Calculate the new position
    return Position(center.GetPositionX() + rotatedX * desiredDist, center.GetPositionY() + rotatedY * desiredDist,
                    current.GetPositionZ());
}

Position IccPutricideAvoidMalleableGooAction::CalculateIncrementalMove(const Position& current, const Position& target,
                                                                       float maxDistance)
{
    float dx = target.GetPositionX() - current.GetPositionX();
    float dy = target.GetPositionY() - current.GetPositionY();
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance <= maxDistance)
        return target;

    dx /= distance;
    dy /= distance;

    return Position(current.GetPositionX() + dx * maxDistance, current.GetPositionY() + dy * maxDistance,
                    target.GetPositionZ());
}

//BPC
bool IccBpcKelesethTankAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "prince keleseth");
    if (!boss)
        return false;

    static float CENTER_MOVE_THRESHOLD = 40.0f;  // distance threshold (yards) to trigger movement toward center
    static float MOVE_INCREMENT = 5.0f;          // movement increment (yards)
    static float Z_TELEPORT_THRESHOLD = 6.0f;    // Z-axis tolerance (yards) to trigger teleport fix

    // Teleport Z-axis back to center-level if the bot is significantly off in Z
    float centerZ = ICC_BPC_CENTER_POSITION.GetPositionZ();
    if (std::abs(bot->GetPositionZ() - centerZ) > Z_TELEPORT_THRESHOLD)
    {
        // Preserve X/Y and orientation, only fix Z to avoid getting stuck below/above arena level
        bot->TeleportTo(bot->GetMapId(), bot->GetPositionX(), bot->GetPositionY(), centerZ, bot->GetOrientation());
        return false;
    }

    // If bot is too far from the center position, move toward it in increments until within threshold
    float distToCenter = bot->GetExactDist2d(ICC_BPC_CENTER_POSITION);
    if (distToCenter > CENTER_MOVE_THRESHOLD)
    {
        // Calculate direction vector toward center
        float dirX = ICC_BPC_CENTER_POSITION.GetPositionX() - bot->GetPositionX();
        float dirY = ICC_BPC_CENTER_POSITION.GetPositionY() - bot->GetPositionY();
        float length = std::sqrt(dirX * dirX + dirY * dirY);

        if (length > 0.001f)
        {
            dirX /= length;
            dirY /= length;

            // Move only as far as needed to get within threshold or up to MOVE_INCREMENT
            float needed = distToCenter - CENTER_MOVE_THRESHOLD;
            float step = std::min(MOVE_INCREMENT, needed);

            float moveX = bot->GetPositionX() + dirX * step;
            float moveY = bot->GetPositionY() + dirY * step;
            float moveZ = bot->GetPositionZ();

            // Attempt incremental move toward center
            MoveTo(bot->GetMapId(), moveX, moveY, moveZ, false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
            return false;
        }
    }

    if (!botAI->IsAssistTank(bot))
        return false;

    bool isBossVictim = false;
    if (boss && boss->GetVictim() == bot)
        isBossVictim = true;

    // If not actively tanking, attack the boss
    if (boss->GetVictim() != bot)
    {
        bot->SetTarget(boss->GetGUID());
        bot->SetFacingToObject(boss);
        Attack(boss);
    }

    // If tanking boss, check for Dark Nucleus logic - collect any nucleus not targeting us
    if (boss->GetVictim() == bot)
    {
        GuidVector targets = AI_VALUE(GuidVector, "possible targets");
        for (auto const& targetGuid : targets)
        {
            Unit* nucleus = botAI->GetUnit(targetGuid);
            if (nucleus && nucleus->IsAlive() && nucleus->GetEntry() == NPC_DARK_NUCLEUS)
            {
                // Attack nucleus that are NOT targeting us (to collect them)
                if (nucleus->GetVictim() != bot)
                {
                    bot->SetTarget(nucleus->GetGUID());
                    bot->SetFacingToObject(nucleus);
                    Attack(nucleus);
                    // Return early to focus on nucleus collection first
                    return false;
                }
            }
        }
    }

    // Positioning logic - only execute if no nucleus needs collecting
    if (botAI->HasAura("Invocation of Blood", boss) && bot->GetExactDist2d(ICC_BPC_MT_POSITION) > 15.0f && isBossVictim)
    {
        // Calculate direction vector
        float dirX = ICC_BPC_MT_POSITION.GetPositionX() - bot->GetPositionX();
        float dirY = ICC_BPC_MT_POSITION.GetPositionY() - bot->GetPositionY();

        // Calculate distance and normalize
        float length = std::sqrt(dirX * dirX + dirY * dirY);
        if (length > 0.001f)
        {
            dirX /= length;
            dirY /= length;

            // Calculate movement distance (max 3 yards at a time)
            float moveDist = std::min(3.0f, length);

            // Calculate target position
            float moveX = bot->GetPositionX() + dirX * moveDist;
            float moveY = bot->GetPositionY() + dirY * moveDist;
            float moveZ = bot->GetPositionZ();

            MoveTo(bot->GetMapId(), moveX, moveY, moveZ, false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
        }
    }

    // Always attack boss when tanking (if no nucleus was prioritized)
    if (boss->GetVictim() == bot)
    {
        bot->SetTarget(boss->GetGUID());
        bot->SetFacingToObject(boss);
        Attack(boss);
    }

    return false;
}

bool IccBpcMainTankAction::Execute(Event /*event*/)
{
    // Main tank specific behavior (higher priority)
    if (botAI->IsMainTank(bot))
    {
        // Get target princes
        auto* valanar = AI_VALUE2(Unit*, "find target", "prince valanar");
        auto* taldaram = AI_VALUE2(Unit*, "find target", "prince taldaram");

        // Check if we're the target of both princes
        bool isVictimOfValanar = false;
        if (valanar && valanar->GetVictim() == bot)
            isVictimOfValanar = true;

        bool isVictimOfTaldaram = false;
        if (taldaram && taldaram->GetVictim() == bot)
            isVictimOfTaldaram = true;

        // Move to MT position if targeted by both princes and not already close
        if (isVictimOfValanar && isVictimOfTaldaram && bot->GetExactDist2d(ICC_BPC_MT_POSITION) > 15.0f)
        {
            // Calculate direction vector
            float dirX = ICC_BPC_MT_POSITION.GetPositionX() - bot->GetPositionX();
            float dirY = ICC_BPC_MT_POSITION.GetPositionY() - bot->GetPositionY();

            // Calculate distance and normalize
            float length = std::sqrt(dirX * dirX + dirY * dirY);
            if (length > 0.001f)
            {
                dirX /= length;
                dirY /= length;

                // Calculate movement distance (max 3 yards at a time)
                float moveDist = std::min(3.0f, length);

                // Calculate target position
                float moveX = bot->GetPositionX() + dirX * moveDist;
                float moveY = bot->GetPositionY() + dirY * moveDist;
                float moveZ = bot->GetPositionZ();

                MoveTo(bot->GetMapId(), moveX, moveY, moveZ, false, false, false, false,
                       MovementPriority::MOVEMENT_COMBAT);
            }
        }

        // Attack prince that's not targeting us
        if (valanar && !isVictimOfValanar)
        {
            bot->SetTarget(valanar->GetGUID());
            bot->SetFacingToObject(valanar);
            Attack(valanar);
        }

        if (taldaram && !isVictimOfTaldaram)
        {
            bot->SetTarget(taldaram->GetGUID());
            bot->SetFacingToObject(taldaram);
            Attack(taldaram);
        }
    }

    // Target marking for all tanks, called after main tank priority actions
    if (botAI->IsTank(bot))
        MarkEmpoweredPrince();

    return false;
}

bool IccBpcMainTankAction::MarkEmpoweredPrince()
{
    static constexpr uint8_t SKULL_RAID_ICON = 7;

    // Find empowered prince (Invocation of Blood)
    Unit* empoweredPrince = nullptr;
    const GuidVector& targets = AI_VALUE(GuidVector, "possible targets");

    for (auto const& targetGuid : targets)
    {
        Unit* unit = botAI->GetUnit(targetGuid);
        if (!unit || !unit->IsAlive())
            continue;

        if (botAI->HasAura("Invocation of Blood", unit))
        {
            const uint32 entry = unit->GetEntry();
            if (entry == NPC_PRINCE_KELESETH || entry == NPC_PRINCE_VALANAR || entry == NPC_PRINCE_TALDARAM)
            {
                empoweredPrince = unit;
                break;
            }
        }
    }

    // Handle marking if we found an empowered prince
    if (empoweredPrince && empoweredPrince->IsAlive())
    {
        Group* group = bot->GetGroup();
        if (group)
        {
            const ObjectGuid currentSkullGuid = group->GetTargetIcon(SKULL_RAID_ICON);
            Unit* markedUnit = botAI->GetUnit(currentSkullGuid);

            // Clear dead marks or marks that are not on empowered prince
            if (markedUnit && (!markedUnit->IsAlive() || markedUnit != empoweredPrince))
            {
                group->SetTargetIcon(SKULL_RAID_ICON, bot->GetGUID(), ObjectGuid::Empty);
            }

            // Mark alive empowered prince if needed
            if (!currentSkullGuid || !markedUnit)
            {
                group->SetTargetIcon(SKULL_RAID_ICON, bot->GetGUID(), empoweredPrince->GetGUID());
            }
        }
    }

    return false;
}

bool IccBpcEmpoweredVortexAction::Execute(Event /*event*/)
{
    Unit* valanar = AI_VALUE2(Unit*, "find target", "prince valanar");
    if (!valanar)
        return false;

    // Check if boss is casting empowered vortex
    bool isCastingVortex = false;
    if (valanar && valanar->HasUnitState(UNIT_STATE_CASTING) &&
       (valanar->FindCurrentSpellBySpellId(SPELL_EMPOWERED_SHOCK_VORTEX1) ||
        valanar->FindCurrentSpellBySpellId(SPELL_EMPOWERED_SHOCK_VORTEX2) ||
        valanar->FindCurrentSpellBySpellId(SPELL_EMPOWERED_SHOCK_VORTEX3) ||
        valanar->FindCurrentSpellBySpellId(SPELL_EMPOWERED_SHOCK_VORTEX4)))
        isCastingVortex = true;

    if (isCastingVortex)
    {
        // Use complex positioning system for empowered vortex
        return HandleEmpoweredVortexSpread();
    }
    else
    {
        // Use simple ranged spreading for non-vortex situations
        return MaintainRangedSpacing();
    }
}

bool IccBpcEmpoweredVortexAction::MaintainRangedSpacing()
{
    const float safeSpacingRadius = 7.0f;
    const float moveIncrement = 2.0f;
    const float maxMoveDistance = 5.0f;
    const bool isRanged = botAI->IsRanged(bot) || botAI->IsHeal(bot);

    if (!isRanged)
        return false;

    // Get group members
    const GuidVector members = AI_VALUE(GuidVector, "group members");

    // Calculate a combined vector representing all nearby members' positions
    float totalX = 0.0f;
    float totalY = 0.0f;
    int nearbyCount = 0;

    for (auto const& memberGuid : members)
    {
        Unit* member = botAI->GetUnit(memberGuid);
        if (!member || !member->IsAlive() || member == bot)
        {
            continue;
        }

        const float distance = bot->GetExactDist2d(member);
        if (distance < safeSpacingRadius)
        {
            // Calculate vector from member to bot
            float dx = bot->GetPositionX() - member->GetPositionX();
            float dy = bot->GetPositionY() - member->GetPositionY();

            // Weight by inverse distance (closer members have more influence)
            float weight = (safeSpacingRadius - distance) / safeSpacingRadius;
            totalX += dx * weight;
            totalY += dy * weight;
            nearbyCount++;
        }
    }

    // If we have nearby members, move away in the combined direction
    if (nearbyCount > 0)
    {
        // Normalize the combined vector
        float magnitude = std::sqrt(totalX * totalX + totalY * totalY);
        if (magnitude > 0.001f)  // Avoid division by zero
        {
            totalX /= magnitude;
            totalY /= magnitude;

            // Calculate move distance
            float moveDistance = std::min(moveIncrement, maxMoveDistance);

            // Create target position in the combined direction
            float targetX = bot->GetPositionX() + totalX * moveDistance;
            float targetY = bot->GetPositionY() + totalY * moveDistance;
            float targetZ = bot->GetPositionZ();

            // Check if the target position is valid and move there
            if (bot->IsWithinLOS(targetX, targetY, targetZ))
            {
                MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, true,
                       MovementPriority::MOVEMENT_NORMAL);
            }
            else
            {
                // If LOS check fails, try shorter distance
                targetX = bot->GetPositionX() + totalX * (moveDistance * 0.5f);
                targetY = bot->GetPositionY() + totalY * (moveDistance * 0.5f);
                MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, true,
                       MovementPriority::MOVEMENT_NORMAL);
            }
        }
    }

    return false;  // Everyone is properly spaced
}

bool IccBpcEmpoweredVortexAction::HandleEmpoweredVortexSpread()
{
    const float safeSpacingRadius = 13.0f;
    const float moveIncrement = 2.0f;
    const float maxMoveDistance = 5.0f;
    const bool isTank = botAI->IsTank(bot);

    if (isTank)
        return false;

    // Get group members
    const GuidVector members = AI_VALUE(GuidVector, "group members");

    // Calculate a combined vector representing all nearby members' positions
    float totalX = 0.0f;
    float totalY = 0.0f;
    int nearbyCount = 0;

    for (auto const& memberGuid : members)
    {
        Unit* member = botAI->GetUnit(memberGuid);
        if (!member || !member->IsAlive() || member == bot)
        {
            continue;
        }

        const float distance = bot->GetExactDist2d(member);
        if (distance < safeSpacingRadius)
        {
            // Calculate vector from member to bot
            float dx = bot->GetPositionX() - member->GetPositionX();
            float dy = bot->GetPositionY() - member->GetPositionY();

            // Weight by inverse distance (closer members have more influence)
            float weight = (safeSpacingRadius - distance) / safeSpacingRadius;
            totalX += dx * weight;
            totalY += dy * weight;
            nearbyCount++;
        }
    }

    // If we have nearby members, move away in the combined direction
    if (nearbyCount > 0)
    {
        // Normalize the combined vector
        float magnitude = std::sqrt(totalX * totalX + totalY * totalY);
        if (magnitude > 0.001f)  // Avoid division by zero
        {
            totalX /= magnitude;
            totalY /= magnitude;

            // Calculate move distance
            float moveDistance = std::min(moveIncrement, maxMoveDistance);

            // Create target position in the combined direction
            float targetX = bot->GetPositionX() + totalX * moveDistance;
            float targetY = bot->GetPositionY() + totalY * moveDistance;
            float targetZ = bot->GetPositionZ();

            // Check if the target position is valid and move there
            if (bot->IsWithinLOS(targetX, targetY, targetZ))
            {
                MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, true,
                       MovementPriority::MOVEMENT_NORMAL);
            }
            else
            {
                // If LOS check fails, try shorter distance
                targetX = bot->GetPositionX() + totalX * (moveDistance * 0.5f);
                targetY = bot->GetPositionY() + totalY * (moveDistance * 0.5f);
                MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, true,
                       MovementPriority::MOVEMENT_NORMAL);
            }
        }
    }

    return false;  // Everyone is properly spaced
}

bool IccBpcKineticBombAction::Execute(Event /*event*/)
{
    // Early exit if not ranged DPS
    if (!botAI->IsRangedDps(bot))
        return false;

    // Static constants
    static constexpr float MAX_HEIGHT_DIFF = 20.0f;
    static constexpr float SAFE_HEIGHT = 371.16473f;
    static constexpr float TELEPORT_HEIGHT = 367.16473f;
    static constexpr std::array<uint32_t, 4> KINETIC_BOMB_ENTRIES = {NPC_KINETIC_BOMB1, NPC_KINETIC_BOMB2,
                                                                     NPC_KINETIC_BOMB3, NPC_KINETIC_BOMB4};

    // Handle edge case where bot is too high
    if (bot->GetPositionZ() > SAFE_HEIGHT)
    {
        bot->TeleportTo(bot->GetMapId(), bot->GetPositionX(), bot->GetPositionY(), TELEPORT_HEIGHT,
                        bot->GetOrientation());
    }

    // Check current target if valid
    if (Unit* currentTarget = AI_VALUE(Unit*, "current target"))
    {
        if (currentTarget->IsAlive() && std::find(KINETIC_BOMB_ENTRIES.begin(), KINETIC_BOMB_ENTRIES.end(),
                                                  currentTarget->GetEntry()) != KINETIC_BOMB_ENTRIES.end())
        {
            const float heightDiff = currentTarget->GetPositionZ() - 361.18222f;
            if (heightDiff < MAX_HEIGHT_DIFF)
                return false;  // Continue current attack
        }
    }

    // Find the best kinetic bomb to attack
    if (Unit* bestBomb = FindOptimalKineticBomb())
    {
        bot->SetTarget(bestBomb->GetGUID());
        bot->SetFacingToObject(bestBomb);
        //--------bypass for AC bpc bug-----------remove when fixed
        bot->Kill(bot, bestBomb);
        //--------bypass for AC bpc bug-----------remove when fixed
        Attack(bestBomb);
    }

    return false;
}

Unit* IccBpcKineticBombAction::FindOptimalKineticBomb()
{
    static constexpr std::array<uint32_t, 4> KINETIC_BOMB_ENTRIES = {NPC_KINETIC_BOMB1, NPC_KINETIC_BOMB2,
                                                                     NPC_KINETIC_BOMB3, NPC_KINETIC_BOMB4};

    const GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    if (targets.empty())
        return nullptr;

    const float botZ = 361.18222f;
    Group* group = bot->GetGroup();

    // Gather all valid kinetic bombs
    std::vector<Unit*> kineticBombs;
    for (auto const& guid : targets)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
            continue;
        if (std::find(KINETIC_BOMB_ENTRIES.begin(), KINETIC_BOMB_ENTRIES.end(), unit->GetEntry()) ==
            KINETIC_BOMB_ENTRIES.end())
            continue;
        kineticBombs.push_back(unit);
    }

    if (kineticBombs.empty())
        return nullptr;

    // Sort bombs by Z ascending (lowest first), then by heightDiff ascending (closest to ground)
    std::sort(kineticBombs.begin(), kineticBombs.end(),
              [botZ](Unit* a, Unit* b)
              {
                  if (a->GetPositionZ() != b->GetPositionZ())
                      return a->GetPositionZ() < b->GetPositionZ();
                  return std::abs(a->GetPositionZ() - botZ) < std::abs(b->GetPositionZ() - botZ);
              });

    // Assign each ranged DPS to a unique bomb (lowest Z first)
    std::vector<Player*> rangedDps;
    if (group)
    {
        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            Player* member = itr->GetSource();
            if (member && member->IsAlive() && botAI->IsRangedDps(member))
                rangedDps.push_back(member);
        }
        // Sort by GUID for deterministic assignment
        std::sort(rangedDps.begin(), rangedDps.end(), [](Player* a, Player* b) { return a->GetGUID() < b->GetGUID(); });
    }
    else
    {
        rangedDps.push_back(bot);
    }

    // Find this bot's index among ranged DPS
    auto it = std::find(rangedDps.begin(), rangedDps.end(), bot);
    if (it == rangedDps.end())
        return nullptr;
    size_t botIndex = std::distance(rangedDps.begin(), it);

    // Assign bombs in order, skip bombs already handled by other ranged DPS
    size_t bombCount = kineticBombs.size();
    for (size_t i = 0, assigned = 0; i < bombCount; ++i)
    {
        Unit* bomb = kineticBombs[i];
        // Check if bomb is already handled by another ranged DPS closer than this bot
        if (IsBombAlreadyHandled(bomb, group))
            continue;
        if (assigned == botIndex)
            return bomb;
        ++assigned;
    }

    // Fallback: pick the lowest bomb not already handled
    for (Unit* bomb : kineticBombs)
    {
        if (!IsBombAlreadyHandled(bomb, group))
            return bomb;
    }

    return nullptr;
}

bool IccBpcKineticBombAction::IsBombAlreadyHandled(Unit* bomb, Group* group)
{
    if (!group)
        return false;

    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || member == bot || !member->IsAlive() || !botAI->IsRangedDps(member))
            continue;

        if (member->GetTarget() == bomb->GetGUID() && member->GetDistance(bomb) < bot->GetDistance(bomb))
            return true;
    }

    return false;
}

bool IccBpcBallOfFlameAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "prince taldaram");
    if (!boss)
        return false;

    Unit* flame1 = bot->FindNearestCreature(NPC_BALL_OF_FLAME, 100.0f);
    Unit* flame2 = bot->FindNearestCreature(NPC_BALL_OF_INFERNO_FLAME, 100.0f);

    bool ballOfFlame = flame1 && (flame1->GetVictim() == bot);
    bool infernoFlame = flame2 && (flame2->GetVictim() == bot);

    //--------bypass for AC BPC bug-----------remove when fixed
    if (flame2 && flame2->IsAlive())
        bot->Kill(bot, flame2);
    //--------bypass for AC BPC bug-----------remove when fixed

    if (flame2 && (flame2->GetDistance2d(boss) > 2.0f) && !(flame2->GetDistance2d(boss) > 10.0f) && !infernoFlame &&
        bot->getClass() != CLASS_HUNTER)
        {
            if (!botAI->IsTank(bot) && !(flame2->GetVictim() == bot))
            {
                float targetX = flame2->GetPositionX();
                float targetY = flame2->GetPositionY();
                float currentX = bot->GetPositionX();
                float currentY = bot->GetPositionY();

                // Calculate direction vector
                float dx = targetX - currentX;
                float dy = targetY - currentY;
                float distance = sqrt(dx * dx + dy * dy);

                // Normalize and scale to 5 units (or remaining distance if less than 5)
                float step = std::min(5.0f, distance);
                if (distance > 0.1)
                {
                    dx = dx / distance * step;
                    dy = dy / distance * step;
                }

                // Calculate intermediate position
                float newX = currentX + dx;
                float newY = currentY + dy;

                MoveTo(flame2->GetMapId(), newX, newY, bot->GetPositionZ(), false, false, false, true,
                       MovementPriority::MOVEMENT_COMBAT);
            }
        }

    // If victim of ball of flame, keep at least 15f from other party members
    if (ballOfFlame || infernoFlame)
    {
        const float SAFE_DIST = 15.0f;
        GuidVector members = AI_VALUE(GuidVector, "group members");
        for (auto const& memberGuid : members)
        {
            Unit* member = botAI->GetUnit(memberGuid);
            if (!member || !member->IsAlive() || member == bot)
                continue;
            float dist = bot->GetExactDist2d(member);
            if (dist < SAFE_DIST)
            {
                // Move away from this member
                float dx = bot->GetPositionX() - member->GetPositionX();
                float dy = bot->GetPositionY() - member->GetPositionY();
                float len = std::sqrt(dx * dx + dy * dy);
                if (len < 0.01f)
                    continue;
                dx /= len;
                dy /= len;
                float moveX = bot->GetPositionX() + dx * (SAFE_DIST - dist + 1.0f);
                float moveY = bot->GetPositionY() + dy * (SAFE_DIST - dist + 1.0f);
                float moveZ = bot->GetPositionZ();
                if (bot->IsWithinLOS(moveX, moveY, moveZ))
                {
                    MoveTo(bot->GetMapId(), moveX, moveY, moveZ, false, false, false, true,
                           MovementPriority::MOVEMENT_FORCED);
                }
            }
        }
    }

    return false;
}

// Blood Queen Lana'thel
bool IccBqlGroupPositionAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "blood-queen lana'thel");
    if (!boss)
        return false;

    Aura* frenzyAura = botAI->GetAura("Frenzied Bloodthirst", bot);
    Aura* shadowAura = botAI->GetAura("Swarming Shadows", bot);
    bool isTank = botAI->IsTank(bot);
    // Handle tank positioning
    if (isTank && HandleTankPosition(boss, frenzyAura, shadowAura))
        return true;

    // Handle swarming shadows movement
    if (shadowAura && HandleShadowsMovement())
        return true;

    // Handle group positioning
    if (!frenzyAura && !shadowAura && HandleGroupPosition(boss, frenzyAura, shadowAura))
        return true;

    return false;
}

bool IccBqlGroupPositionAction::HandleTankPosition(Unit* boss, Aura* frenzyAura, Aura* shadowAura)
{
    if (frenzyAura || shadowAura)
        return false;

    // Main tank positioning
    if (botAI->IsMainTank(bot) && botAI->HasAggro(boss))
    {
        if (bot->GetExactDist2d(ICC_BQL_TANK_POSITION) > 3.0f)
        {
            MoveTo(bot->GetMapId(), ICC_BQL_TANK_POSITION.GetPositionX(), ICC_BQL_TANK_POSITION.GetPositionY(),
                   ICC_BQL_TANK_POSITION.GetPositionZ(), false, false, false, true,
                   MovementPriority::MOVEMENT_COMBAT);
        }
    }

    // Assist tank positioning
    if (botAI->IsAssistTank(bot) && !botAI->GetAura("Blood Mirror", bot))
    {
        if (Unit* mainTank = AI_VALUE(Unit*, "main tank"))
        {
            MoveTo(bot->GetMapId(), mainTank->GetPositionX(), mainTank->GetPositionY(), mainTank->GetPositionZ(),
                   false, false, false, true, MovementPriority::MOVEMENT_COMBAT);
        }
    }

    return false;
}

bool IccBqlGroupPositionAction::HandleShadowsMovement()
{
    const float SAFE_SHADOW_DIST = 4.0f;
    const float ARC_STEP = 0.05f;
    const float CURVE_SPACING = 15.0f;
    const int MAX_CURVES = 3;
    const float maxClosestDist = botAI->IsMelee(bot) ? 25.0f : 20.0f;
    const Position& center = ICC_BQL_CENTER_POSITION;
    const float OUTER_CURVE_PREFERENCE = 200.0f;   // Strong preference for outer curves
    const float CURVE_SWITCH_PENALTY = 50.0f;      // Penalty for switching curves
    const float DISTANCE_PENALTY_FACTOR = 100.0f;  // Penalty per yard moved from current position
    const float MAX_CURVE_JUMP_DIST = 5.0f;        // Maximum distance for jumping between curves

    // Track current curve to avoid unnecessary switching
    static std::map<ObjectGuid, int> botCurrentCurve;
    int currentCurve = botCurrentCurve.count(bot->GetGUID()) ? botCurrentCurve[bot->GetGUID()] : 0;

    // Find closest wall path
    Position lwall[4] = {ICC_BQL_LWALL1_POSITION, AdjustControlPoint(ICC_BQL_LWALL2_POSITION, center, 1.30f),
                         AdjustControlPoint(ICC_BQL_LWALL3_POSITION, center, 1.30f), ICC_BQL_LRWALL4_POSITION};
    Position rwall[4] = {ICC_BQL_RWALL1_POSITION, AdjustControlPoint(ICC_BQL_RWALL2_POSITION, center, 1.30f),
                         AdjustControlPoint(ICC_BQL_RWALL3_POSITION, center, 1.30f), ICC_BQL_LRWALL4_POSITION};
    Position* basePath = (bot->GetExactDist2d(lwall[0]) < bot->GetExactDist2d(rwall[0])) ? lwall : rwall;

    // Find all swarming shadows
    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    Unit* shadows[100]{};  // Reasonable max estimate
    int shadowCount = 0;
    for (int i = 0; i < npcs.size() && shadowCount < 100; i++)
    {
        Unit* unit = botAI->GetUnit(npcs[i]);
        if (unit && unit->IsAlive() && unit->GetEntry() == NPC_SWARMING_SHADOWS)
            shadows[shadowCount++] = unit;
    }

    // Helper lambda to check if a position is inside a shadow
    auto IsPositionInShadow = [&](const Position& pos) -> bool
    {
        for (int i = 0; i < shadowCount; ++i)
        {
            if (pos.GetExactDist2d(shadows[i]) < SAFE_SHADOW_DIST)
                return true;
        }
        return false;
    };

    // If bot is at the 4th position (end of the wall), move towards 3rd position or center to avoid getting stuck
    float distToL4 = bot->GetExactDist2d(lwall[3]);
    float distToR4 = bot->GetExactDist2d(rwall[3]);
    const float STUCK_DIST = 2.0f;  // within 2 yards is considered stuck at the end

    if (distToL4 < STUCK_DIST || distToR4 < STUCK_DIST)
    {
        // Move towards 3rd position of the same wall, or towards center if blocked
        Position target;
        if (distToL4 < distToR4)
        {
            target = lwall[2];
        }
        else
        {
            target = rwall[2];
        }

        float tx = target.GetPositionX();
        float ty = target.GetPositionY();
        float tz = target.GetPositionZ();
        bot->UpdateAllowedPositionZ(tx, ty, tz);
        if (!bot->IsWithinLOS(tx, ty, tz) || IsPositionInShadow(Position(tx, ty, tz)))
        {
            tx = center.GetPositionX();
            ty = center.GetPositionY();
            tz = center.GetPositionZ();
        }

        if (bot->GetExactDist2d(tx, ty) > 1.0f)
        {
            MoveTo(bot->GetMapId(), tx, ty, tz, false, false, false, true, MovementPriority::MOVEMENT_FORCED,
                   true, false);
        }
        return false;
    }

    CurveInfo bestCurve;
    bestCurve.foundSafe = false;
    bestCurve.score = FLT_MAX;
    bool foundCurve = false;

    // Keep track of information about all curves for possible fallback
    CurveInfo curveInfos[MAX_CURVES];
    for (int i = 0; i < MAX_CURVES; i++)
    {
        curveInfos[i].foundSafe = false;
        curveInfos[i].score = FLT_MAX;
    }

    // Evaluate all curves starting from outermost (lowest index)
    for (int curveIdx = 0; curveIdx < MAX_CURVES; curveIdx++)
    {
        float curveShrink = float(curveIdx) * CURVE_SPACING;
        float shrinkFactor = 1.30f - (curveShrink / 30.0f);
        if (shrinkFactor < 1.0f)
            shrinkFactor = 1.0f;

        Position path[4] = {basePath[0], AdjustControlPoint(basePath[1], center, shrinkFactor / 1.30f),
                            AdjustControlPoint(basePath[2], center, shrinkFactor / 1.30f), basePath[3]};

        // Find closest point on curve
        float minDist = 9999.0f;
        float t_closest = 0.0f;
        Position closestPoint = path[0];

        for (float t = 0.0f; t <= 1.0f; t += ARC_STEP)
        {
            Position pt = CalculateBezierPoint(t, path);
            float dist = bot->GetExactDist2d(pt);
            if (dist < minDist)
            {
                minDist = dist;
                t_closest = t;
                closestPoint = pt;
            }
        }

        // Check if the closest point is safe
        bool closestIsSafe = !IsPositionInShadow(closestPoint);

        // Find closest safe point by searching in both directions from closest point
        Position safeMoveTarget = closestPoint;
        bool foundSafe = closestIsSafe;

        // Only search for safe spots if the closest point isn't already safe
        if (!closestIsSafe)
        {
            // Find the nearest safe point along the curve, not by direct distance
            // but by distance along the curve from the closest point

            // Search forward on curve from closest point
            float forwardT = -1.0f;
            Position forwardPt;
            for (float t = t_closest + ARC_STEP; t <= 1.0f; t += ARC_STEP)
            {
                Position pt = CalculateBezierPoint(t, path);
                if (!IsPositionInShadow(pt))
                {
                    forwardT = t;
                    forwardPt = pt;
                    break;
                }
            }

            // Search backward on curve from closest point
            float backwardT = -1.0f;
            Position backwardPt;
            for (float t = t_closest - ARC_STEP; t >= 0.0f; t -= ARC_STEP)
            {
                Position pt = CalculateBezierPoint(t, path);
                if (!IsPositionInShadow(pt))
                {
                    backwardT = t;
                    backwardPt = pt;
                    break;
                }
            }

            // Choose the closest safe point based on curve distance, not direct distance
            if (forwardT >= 0 && backwardT >= 0)
            {
                // Both directions have safe points, choose the closer one by curve distance
                if (std::abs(forwardT - t_closest) < std::abs(backwardT - t_closest))
                {
                    safeMoveTarget = forwardPt;
                    foundSafe = true;
                }
                else
                {
                    safeMoveTarget = backwardPt;
                    foundSafe = true;
                }
            }
            else if (forwardT >= 0)
            {
                safeMoveTarget = forwardPt;
                foundSafe = true;
            }
            else if (backwardT >= 0)
            {
                safeMoveTarget = backwardPt;
                foundSafe = true;
            }
        }

        // Score this curve
        float distancePenalty = 0.0f;
        float score = 0.0f;

        if (foundSafe)
        {
            // If we found a safe point, penalize based on travel distance along the curve to reach it
            float safeDist = bot->GetExactDist2d(safeMoveTarget);

            // Add distance penalty based on how far we need to move along the curve
            distancePenalty = safeDist * (1.0f / DISTANCE_PENALTY_FACTOR);
            score = safeDist + distancePenalty;
        }
        else
        {
            // No safe point found, assign a high score
            distancePenalty = minDist * (1.0f / DISTANCE_PENALTY_FACTOR);
            score = minDist + distancePenalty + 1000.0f;  // Penalty for unsafe position
        }

        // Apply strong penalty for curves that are too far
        if (minDist > maxClosestDist)
            score += 500.0f;

        // Apply penalty for unsafe curves
        if (!foundSafe)
            score += 1000.0f;

        // Apply curve index preference (strongly prefer outer curves)
        score += curveIdx * OUTER_CURVE_PREFERENCE;

        // Apply curve switching penalty
        if (curveIdx != currentCurve && currentCurve != 0)
            score += CURVE_SWITCH_PENALTY;

        // MORE IMPORTANT: Apply additional curve switching penalty if the bot is far away
        // from the target curve (prevent jumping between curves when far away)
        if (curveIdx != currentCurve && minDist > MAX_CURVE_JUMP_DIST)
            score += 2000.0f;  // Strong penalty to prevent jumping between curves

        // Store this curve's info
        curveInfos[curveIdx].moveTarget = foundSafe ? safeMoveTarget : closestPoint;
        curveInfos[curveIdx].foundSafe = foundSafe;
        curveInfos[curveIdx].minDist = minDist;
        curveInfos[curveIdx].curveIdx = curveIdx;
        curveInfos[curveIdx].score = score;
        curveInfos[curveIdx].closestPoint = closestPoint;
        curveInfos[curveIdx].t_closest = t_closest;

        // Only update if this curve is better than our current best
        if (!foundCurve || score < bestCurve.score)
        {
            bestCurve = curveInfos[curveIdx];
            foundCurve = true;
        }
    }

    // Fallback: If we're trying to switch to a far curve and we're not near any curve,
    // find and use the closest curve instead of making a direct beeline
    if (foundCurve && bestCurve.minDist > MAX_CURVE_JUMP_DIST && bestCurve.curveIdx != currentCurve)
    {
        // Look for the closest curve first
        float closestDist = FLT_MAX;
        int closestCurveIdx = -1;

        for (int i = 0; i < MAX_CURVES; i++)
        {
            if (curveInfos[i].minDist < closestDist)
            {
                closestDist = curveInfos[i].minDist;
                closestCurveIdx = i;
            }
        }

        // If we found a closer curve, use that instead
        if (closestCurveIdx >= 0 && closestCurveIdx != bestCurve.curveIdx)
        {
            bestCurve = curveInfos[closestCurveIdx];
        }
    }

    // Remember the selected curve for next time
    if (foundCurve)
    {
        botCurrentCurve[bot->GetGUID()] = bestCurve.curveIdx;
    }

    // Create a move plan to guide the bot along the curve if necessary
    if (foundCurve && bot->GetExactDist2d(bestCurve.moveTarget) > 1.0f)
    {
        // Final check: ensure we're not moving into a shadow
        if (!IsPositionInShadow(bestCurve.moveTarget))
        {
            // Get the curve
            float curveShrink = float(bestCurve.curveIdx) * CURVE_SPACING;
            float shrinkFactor = 1.30f - (curveShrink / 30.0f);
            if (shrinkFactor < 1.0f)
                shrinkFactor = 1.0f;

            Position path[4] = {basePath[0], AdjustControlPoint(basePath[1], center, shrinkFactor / 1.30f),
                                AdjustControlPoint(basePath[2], center, shrinkFactor / 1.30f), basePath[3]};

            // CRITICAL CHANGE: First check if we need to move to the curve
            float distToClosestPoint = bot->GetExactDist2d(bestCurve.closestPoint);

            // If we're not on the curve yet, first move to the closest point on the curve
            if (distToClosestPoint > 2.0f)
            {
                botAI->Reset();
                return MoveTo(bot->GetMapId(), bestCurve.closestPoint.GetPositionX(),
                              bestCurve.closestPoint.GetPositionY(), bestCurve.closestPoint.GetPositionZ(), false,
                              false, false, true, MovementPriority::MOVEMENT_FORCED, true, false);
            }

            // Now we know we're on or very close to the curve, so we'll follow it properly

            // Find target point on curve (t_target parameter)
            float t_target = 0.0f;
            float targetMinDist = 9999.0f;

            for (float t = 0.0f; t <= 1.0f; t += ARC_STEP)
            {
                Position pt = CalculateBezierPoint(t, path);
                float dist = bestCurve.moveTarget.GetExactDist2d(pt);
                if (dist < targetMinDist)
                {
                    targetMinDist = dist;
                    t_target = t;
                }
            }

            // Find an intermediate point along the curve between closest and target
            float t_step = (t_target > bestCurve.t_closest) ? ARC_STEP : -ARC_STEP;
            float t_intermediate = bestCurve.t_closest + t_step;
            Position intermediateTarget;
            bool foundValidIntermediate = false;

            // Limit the distance we move along the curve in one step
            const float MAX_CURVE_MOVEMENT = 7.0f;  // Max yards to move along curve
            float curveDistanceMoved = 0.0f;
            Position lastPos = bestCurve.closestPoint;

            while ((t_step > 0 && t_intermediate <= t_target) || (t_step < 0 && t_intermediate >= t_target))
            {
                Position pt = CalculateBezierPoint(t_intermediate, path);

                // Check if this point is safe
                if (!IsPositionInShadow(pt))
                {
                    // Calculate distance moved along curve so far
                    curveDistanceMoved += lastPos.GetExactDist2d(pt);
                    lastPos = pt;

                    // If we've moved the maximum allowed distance, use this position
                    if (curveDistanceMoved >= MAX_CURVE_MOVEMENT)
                    {
                        intermediateTarget = pt;
                        foundValidIntermediate = true;
                        break;
                    }

                    // Otherwise, continue moving along the curve
                    intermediateTarget = pt;
                    foundValidIntermediate = true;
                }
                else
                {
                    // We've hit a shadow, stop here
                    break;
                }

                t_intermediate += t_step;
            }

            // If we found a valid intermediate point, use it
            if (foundValidIntermediate)
            {
                botAI->Reset();
                MoveTo(bot->GetMapId(), intermediateTarget.GetPositionX(), intermediateTarget.GetPositionY(),
                       intermediateTarget.GetPositionZ(), false, false, false, true,
                       MovementPriority::MOVEMENT_FORCED, true, false);
            }

            botAI->Reset();
            // Fallback to direct movement to the target point on the curve
            MoveTo(bot->GetMapId(), bestCurve.moveTarget.GetPositionX(), bestCurve.moveTarget.GetPositionY(),
                   bestCurve.moveTarget.GetPositionZ(), false, false, false, true,
                   MovementPriority::MOVEMENT_FORCED, true, false);
        }
    }

    return false;
}

Position IccBqlGroupPositionAction::AdjustControlPoint(const Position& wall, const Position& center, float factor)
{
    float dx = wall.GetPositionX() - center.GetPositionX();
    float dy = wall.GetPositionY() - center.GetPositionY();
    float dz = wall.GetPositionZ() - center.GetPositionZ();
    return Position(center.GetPositionX() + dx * factor, center.GetPositionY() + dy * factor,
                    center.GetPositionZ() + dz * factor);
}

Position IccBqlGroupPositionAction::CalculateBezierPoint(float t, const Position path[4])
{
    float omt = 1 - t;
    float omt2 = omt * omt;
    float omt3 = omt2 * omt;
    float t2 = t * t;
    float t3 = t2 * t;

    float x = omt3 * path[0].GetPositionX() + 3 * omt2 * t * path[1].GetPositionX() +
              3 * omt * t2 * path[2].GetPositionX() + t3 * path[3].GetPositionX();

    float y = omt3 * path[0].GetPositionY() + 3 * omt2 * t * path[1].GetPositionY() +
              3 * omt * t2 * path[2].GetPositionY() + t3 * path[3].GetPositionY();

    float z = omt3 * path[0].GetPositionZ() + 3 * omt2 * t * path[1].GetPositionZ() +
              3 * omt * t2 * path[2].GetPositionZ() + t3 * path[3].GetPositionZ();

    return Position(x, y, z);
}

bool IccBqlGroupPositionAction::HandleGroupPosition(Unit* boss, Aura* frenzyAura, Aura* shadowAura)
{
    if (frenzyAura || shadowAura)
        return false;

    GuidVector members = AI_VALUE(GuidVector, "group members");
    bool isRanged = botAI->IsRanged(bot);
    bool isMelee = botAI->IsMelee(bot);

    if (isRanged && bot->GetExactDist2d(boss->GetPositionX(), boss->GetPositionY()) > 35.0f)
        MoveTo(boss, 5.0f, MovementPriority::MOVEMENT_FORCED);

    if ((boss->GetExactDist2d(ICC_BQL_TANK_POSITION.GetPositionX(), ICC_BQL_TANK_POSITION.GetPositionY()) > 10.0f) &&
        isRanged && !((boss->GetPositionZ() - ICC_BQL_CENTER_POSITION.GetPositionZ()) > 5.0f) &&
        (bot->GetExactDist2d(ICC_BQL_CENTER_POSITION.GetPositionX(), ICC_BQL_CENTER_POSITION.GetPositionY()) > 10.0f))
        MoveTo(bot->GetMapId(), ICC_BQL_CENTER_POSITION.GetPositionX(), ICC_BQL_CENTER_POSITION.GetPositionY(),
               ICC_BQL_CENTER_POSITION.GetPositionZ(), false, false, false, true,
               MovementPriority::MOVEMENT_COMBAT, true, false);

    if (isRanged)
    {
        // Gather all ranged and healers, sort by GUID for deterministic assignment
        std::vector<Player*> rangedBots;
        std::vector<Player*> healers;
        for (auto const& guid : members)
        {
            Unit* member = botAI->GetUnit(guid);
            if (!member || !member->IsAlive())
                continue;
            Player* player = member->ToPlayer();
            if (!player)
                continue;
            if (botAI->IsRanged(player))
                rangedBots.push_back(player);
            if (botAI->IsHeal(player))
                healers.push_back(player);
        }
        // Remove duplicates (healer can be ranged)
        std::sort(rangedBots.begin(), rangedBots.end(),
                  [](Player* a, Player* b) { return a->GetGUID() < b->GetGUID(); });
        std::sort(healers.begin(), healers.end(), [](Player* a, Player* b) { return a->GetGUID() < b->GetGUID(); });

        // Assign at least one healer to each side, then balance the rest
        std::vector<Player*> leftSide, rightSide;
        Position leftPos = ICC_BQL_LWALL2_POSITION;
        Position rightPos = ICC_BQL_RWALL2_POSITION;

        // Assign healers first
        if (!healers.empty())
        {
            leftSide.push_back(healers[0]);
            if (healers.size() > 1)
                rightSide.push_back(healers[1]);
        }
        // If only one healer, assign to left, right will be filled by ranged DPS

        // Remove assigned healers from rangedBots
        for (Player* h : leftSide)
            rangedBots.erase(std::remove(rangedBots.begin(), rangedBots.end(), h), rangedBots.end());
        for (Player* h : rightSide)
            rangedBots.erase(std::remove(rangedBots.begin(), rangedBots.end(), h), rangedBots.end());

        // Distribute remaining ranged evenly
        size_t leftCount = leftSide.size();
        size_t rightCount = rightSide.size();
        for (Player* p : rangedBots)
        {
            if (leftCount <= rightCount)
            {
                leftSide.push_back(p);
                leftCount++;
            }
            else
            {
                rightSide.push_back(p);
                rightCount++;
            }
        }

        // Determine which side this bot is assigned to
        bool isLeft = std::find(leftSide.begin(), leftSide.end(), bot) != leftSide.end();
        bool isRight = std::find(rightSide.begin(), rightSide.end(), bot) != rightSide.end();

        // Move to assigned wall position if not already close
        const float MAX_WALL_DIST = 30.0f;
        const float MOVE_INCREMENT = 2.0f;
        const float MAX_MOVE_DISTANCE = 7.0f;
        const float SAFE_SPACING_RADIUS = 7.0f;
        const float MIN_CENTER_DISTANCE = 10.0f;

        Position targetWall = isLeft ? leftPos : (isRight ? rightPos : Position());
        if (isLeft || isRight)
        {
            float distToWall = bot->GetExactDist2d(targetWall.GetPositionX(), targetWall.GetPositionY());
            if (distToWall > MAX_WALL_DIST)
            {
                // Move in increments toward wall
                float dx = targetWall.GetPositionX() - bot->GetPositionX();
                float dy = targetWall.GetPositionY() - bot->GetPositionY();
                float len = std::sqrt(dx * dx + dy * dy);
                if (len > 0.001f)
                {
                    dx /= len;
                    dy /= len;
                    float moveDist = std::min(MOVE_INCREMENT, distToWall);
                    float targetX = bot->GetPositionX() + dx * moveDist;
                    float targetY = bot->GetPositionY() + dy * moveDist;
                    float targetZ = bot->GetPositionZ();
                    if (!bot->IsWithinLOS(targetX, targetY, targetZ))
                    {
                        targetX = bot->GetPositionX() + dx * (moveDist * 0.5f);
                        targetY = bot->GetPositionY() + dy * (moveDist * 0.5f);
                    }
                    botAI->Reset();
                    MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, true,
                           MovementPriority::MOVEMENT_COMBAT, true, false);
                }
            }
            // Spread from other assigned members on the same side and from swarming shadows
            float totalX = 0.0f, totalY = 0.0f;
            int nearbyCount = 0;
            const std::vector<Player*>& mySide = isLeft ? leftSide : rightSide;
            for (Player* member : mySide)
            {
                if (!member || !member->IsAlive() || member == bot)
                    continue;
                float distance = bot->GetExactDist2d(member);
                if (distance < SAFE_SPACING_RADIUS)
                {
                    float dx = bot->GetPositionX() - member->GetPositionX();
                    float dy = bot->GetPositionY() - member->GetPositionY();
                    float weight = (SAFE_SPACING_RADIUS - distance) / SAFE_SPACING_RADIUS;
                    totalX += dx * weight;
                    totalY += dy * weight;
                    nearbyCount++;
                }
            }
            // Also spread from swarming shadows
            GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
            for (auto const& npcGuid : npcs)
            {
                Unit* unit = botAI->GetUnit(npcGuid);
                if (unit && unit->IsAlive() && unit->GetEntry() == NPC_SWARMING_SHADOWS)
                {
                    float distance = bot->GetExactDist2d(unit);
                    if (distance < SAFE_SPACING_RADIUS)
                    {
                        float dx = bot->GetPositionX() - unit->GetPositionX();
                        float dy = bot->GetPositionY() - unit->GetPositionY();
                        float weight = (SAFE_SPACING_RADIUS - distance) / SAFE_SPACING_RADIUS;
                        totalX += dx * weight;
                        totalY += dy * weight;
                        nearbyCount++;
                    }
                }
            }
            if (nearbyCount > 0)
            {
                float magnitude = sqrt(totalX * totalX + totalY * totalY);
                if (magnitude > 0.001f)
                {
                    totalX /= magnitude;
                    totalY /= magnitude;
                    float moveDistance = std::min(MOVE_INCREMENT, MAX_MOVE_DISTANCE);
                    float targetX = bot->GetPositionX() + totalX * moveDistance;
                    float targetY = bot->GetPositionY() + totalY * moveDistance;
                    float targetZ = bot->GetPositionZ();
                    if (!bot->IsWithinLOS(targetX, targetY, targetZ))
                    {
                        targetX = bot->GetPositionX() + totalX * (moveDistance * 0.5f);
                        targetY = bot->GetPositionY() + totalY * (moveDistance * 0.5f);
                    }
                    botAI->Reset();
                    MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, true,
                           MovementPriority::MOVEMENT_COMBAT, true, false);
                }
            }
            // Maintain minimum distance from center position (if too close to center, move out)
            float centerX = ICC_BQL_CENTER_POSITION.GetPositionX();
            float centerY = ICC_BQL_CENTER_POSITION.GetPositionY();
            float centerDist = bot->GetDistance2d(centerX, centerY);
            if (centerDist < MIN_CENTER_DISTANCE && !((boss->GetPositionZ() - bot->GetPositionZ()) > 5.0f))
            {
                float dx = bot->GetPositionX() - centerX;
                float dy = bot->GetPositionY() - centerY;
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist > 0.001f)
                {
                    dx /= dist;
                    dy /= dist;
                    float moveDistance = std::min(MIN_CENTER_DISTANCE - centerDist + 1.0f, MAX_MOVE_DISTANCE);
                    float targetX = bot->GetPositionX() + dx * moveDistance;
                    float targetY = bot->GetPositionY() + dy * moveDistance;
                    float targetZ = bot->GetPositionZ();
                    if (!bot->IsWithinLOS(targetX, targetY, targetZ))
                    {
                        targetX = bot->GetPositionX() + dx * (moveDistance * 0.5f);
                        targetY = bot->GetPositionY() + dy * (moveDistance * 0.5f);
                    }
                    botAI->Reset();
                    MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, true,
                           MovementPriority::MOVEMENT_COMBAT, true, false);
                }
            }
        }
    }

    if (isMelee && ((boss->GetPositionZ() - ICC_BQL_CENTER_POSITION.GetPositionZ()) > 5.0f))
    {
        const float SAFE_SPACING_RADIUS = 7.0f;
        const float MOVE_INCREMENT = 2.0f;
        const float MAX_MOVE_DISTANCE = 7.0f;

        float totalX = 0.0f;
        float totalY = 0.0f;
        int nearbyCount = 0;

        // Find all swarming shadows
        GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
        std::vector<Unit*> swarmingShadows;
        for (int i = 0; i < npcs.size(); ++i)
        {
            Unit* unit = botAI->GetUnit(npcs[i]);
            if (unit && unit->IsAlive() && unit->GetEntry() == NPC_SWARMING_SHADOWS)
                swarmingShadows.push_back(unit);
        }

        for (int i = 0; i < members.size(); i++)
        {
            Unit* member = botAI->GetUnit(members[i]);
            if (!member || !member->IsAlive() || member == bot || botAI->GetAura("Frenzied Bloodthirst", member) ||
                botAI->GetAura("Uncontrollable Frenzy", member))
                continue;

            float distance = bot->GetExactDist2d(member);
            if (distance < SAFE_SPACING_RADIUS)
            {
                float dx = bot->GetPositionX() - member->GetPositionX();
                float dy = bot->GetPositionY() - member->GetPositionY();
                float weight = (SAFE_SPACING_RADIUS - distance) / SAFE_SPACING_RADIUS;
                totalX += dx * weight;
                totalY += dy * weight;
                nearbyCount++;
            }
        }

        // Also spread from swarming shadows
        for (Unit* shadow : swarmingShadows)
        {
            float distance = bot->GetExactDist2d(shadow);
            if (distance < SAFE_SPACING_RADIUS)
            {
                float dx = bot->GetPositionX() - shadow->GetPositionX();
                float dy = bot->GetPositionY() - shadow->GetPositionY();
                float weight = (SAFE_SPACING_RADIUS - distance) / SAFE_SPACING_RADIUS;
                totalX += dx * weight;
                totalY += dy * weight;
                nearbyCount++;
            }
        }

        if (nearbyCount > 0)
        {
            float magnitude = sqrt(totalX * totalX + totalY * totalY);
            if (magnitude > 0.001f)
            {
                totalX /= magnitude;
                totalY /= magnitude;
                float moveDistance = MOVE_INCREMENT < MAX_MOVE_DISTANCE ? MOVE_INCREMENT : MAX_MOVE_DISTANCE;
                float targetX = bot->GetPositionX() + totalX * moveDistance;
                float targetY = bot->GetPositionY() + totalY * moveDistance;
                float targetZ = bot->GetPositionZ();

                if (!bot->IsWithinLOS(targetX, targetY, targetZ))
                {
                    targetX = bot->GetPositionX() + totalX * (moveDistance * 0.5f);
                    targetY = bot->GetPositionY() + totalY * (moveDistance * 0.5f);
                }

                MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, true,
                       MovementPriority::MOVEMENT_COMBAT, true, false);
            }
        }
        return true;
    }

    return false;
}

bool IccBqlPactOfDarkfallenAction::Execute(Event /*event*/)
{
    // Check if bot has Pact of the Darkfallen
    if (!botAI->GetAura("Pact of the Darkfallen", bot))
        return false;
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    // Find other players with Pact of the Darkfallen
    Player* tankWithAura = nullptr;
    std::vector<Player*> playersWithAura;

    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || member == bot)
            continue;
        if (botAI->GetAura("Pact of the Darkfallen", member))
        {
            playersWithAura.push_back(member);
            if (botAI->IsTank(member))
                tankWithAura = member;
        }
    }

    if (playersWithAura.empty())
        return false;

    // Determine target position
    Position targetPos;
    if (tankWithAura)
    {
        // If there's a tank with aura, everyone moves to the tank (including the tank itself for center positioning)
        if (botAI->IsTank(bot))
        {
            // If current bot is the tank, stay put or move slightly for better positioning
            targetPos.Relocate(bot);
        }
        else
        {
            // Non-tank bots move to the tank
            targetPos.Relocate(tankWithAura);
        }
    }
    else if (playersWithAura.size() >= 2)
    {
        // Calculate center position of all players with aura (including bot)
        CalculateCenterPosition(targetPos, playersWithAura);
    }
    else if (playersWithAura.size() == 1)
    {
        // Move to the single other player with aura
        targetPos.Relocate(playersWithAura[0]);
    }
    else
    {
        // No valid movement case found
        return true;
    }

    // Move to target position if needed
    return MoveToTargetPosition(targetPos, playersWithAura.size() + 1);  // +1 to include the bot itself
}

bool IccBqlPactOfDarkfallenAction::CalculateCenterPosition(Position& targetPos, const std::vector<Player*>& playersWithAura)
{
    float sumX = bot->GetPositionX();
    float sumY = bot->GetPositionY();
    float sumZ = bot->GetPositionZ();

    // Add positions of all other players with aura
    for (Player* player : playersWithAura)
    {
        sumX += player->GetPositionX();
        sumY += player->GetPositionY();
        sumZ += player->GetPositionZ();
    }

    // Calculate average position (center)
    int totalPlayers = playersWithAura.size() + 1;  // +1 for the bot itself
    targetPos.Relocate(sumX / totalPlayers, sumY / totalPlayers, sumZ / totalPlayers);

    return false;
}

bool IccBqlPactOfDarkfallenAction::MoveToTargetPosition(const Position& targetPos, int auraCount)
{
    const float POSITION_TOLERANCE = 0.1f;
    float distance = bot->GetDistance(targetPos);
    if (distance <= POSITION_TOLERANCE)
        return true;

    // Calculate movement increment
    float dx = targetPos.GetPositionX() - bot->GetPositionX();
    float dy = targetPos.GetPositionY() - bot->GetPositionY();
    float dz = targetPos.GetPositionZ() - bot->GetPositionZ();
    float len = sqrt(dx * dx + dy * dy);

    float moveX, moveY, moveZ;
    if (len > 5.0f && auraCount <= 2)
    {
        dx /= len;
        dy /= len;
        moveX = bot->GetPositionX() + dx * 5.0f;
        moveY = bot->GetPositionY() + dy * 5.0f;
        moveZ = bot->GetPositionZ() + (dz / distance) * 5.0f;
    }
    else
    {
        moveX = targetPos.GetPositionX();
        moveY = targetPos.GetPositionY();
        moveZ = targetPos.GetPositionZ();
    }

    botAI->Reset();
    MoveTo(bot->GetMapId(), moveX, moveY, moveZ, false, false, false, true, MovementPriority::MOVEMENT_FORCED);
    return false;
}

bool IccBqlVampiricBiteAction::Execute(Event /*event*/)
{
    // Only act when bot has Frenzied Bloodthirst
    if (!botAI->GetAura("Frenzied Bloodthirst", bot))
        return false;

    const float BITE_RANGE = 2.0f;
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    // Find best target
    Player* target = FindBestBiteTarget(group);
    if (!target)
        return false;

    // Handle movement or casting
    float x = target->GetPositionX();
    float y = target->GetPositionY();
    float z = target->GetPositionZ();

    if (bot->GetExactDist2d(target) > BITE_RANGE)
    {
        return MoveTowardsTarget(target);
    }
    else if (bot->IsWithinLOS(x, y, z))
    {
        return CastVampiricBite(target);
    }

    return false;
}

Player* IccBqlVampiricBiteAction::FindBestBiteTarget(Group* group)
{
    std::set<ObjectGuid> currentlyTargetedPlayers;
    std::vector<std::pair<Player*, float>> dpsTargets;
    std::vector<std::pair<Player*, float>> healTargets;

    // Get currently targeted players
    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || !member->IsAlive() || member == bot)
            continue;

        if (botAI->GetAura("Frenzied Bloodthirst", member) && member->GetTarget())
        {
            currentlyTargetedPlayers.insert(member->GetTarget());
        }
    }

    // Evaluate potential targets
    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || member == bot || !member->IsAlive())
            continue;

        if (IsInvalidTarget(member) || currentlyTargetedPlayers.count(member->GetGUID()))
            continue;

        float distance = bot->GetDistance(member);
        if (botAI->IsDps(member))
            dpsTargets.push_back({member, distance});
        else if (botAI->IsHeal(member))
            healTargets.push_back({member, distance});
    }

    // Sort by distance
    auto sortByDistance = [](auto const& a, auto const& b) { return a.second < b.second; };
    std::sort(dpsTargets.begin(), dpsTargets.end(), sortByDistance);
    std::sort(healTargets.begin(), healTargets.end(), sortByDistance);

    // Return closest valid target
    if (!dpsTargets.empty())
        return dpsTargets[0].first;
    if (!healTargets.empty())
        return healTargets[0].first;
    return nullptr;
}

bool IccBqlVampiricBiteAction::IsInvalidTarget(Player* player)
{
    return botAI->GetAura("Frenzied Bloodthirst", player) || botAI->GetAura("Essence of the Blood Queen", player) ||
           botAI->GetAura("Uncontrollable Frenzy", player) || botAI->GetAura("Swarming Shadows", player) ||
           botAI->IsTank(player);
}

bool IccBqlVampiricBiteAction::MoveTowardsTarget(Player* target)
{
    if (IsInvalidTarget(target) || !target->IsAlive())
        return false;

    float x = target->GetPositionX();
    float y = target->GetPositionY();
    float z = target->GetPositionZ();

    if (!bot->IsWithinLOS(x, y, z))
        return false;

    float dx = x - bot->GetPositionX();
    float dy = y - bot->GetPositionY();
    float dz = z - bot->GetPositionZ();
    float len = sqrt(dx * dx + dy * dy);

    float moveX, moveY, moveZ;
    if (len > 5.0f)
    {
        dx /= len;
        dy /= len;
        moveX = bot->GetPositionX() + dx * 5.0f;
        moveY = bot->GetPositionY() + dy * 5.0f;
        moveZ = bot->GetPositionZ() + (dz / len) * 5.0f;
    }
    else
    {
        moveX = x;
        moveY = y;
        moveZ = z;
    }

    MoveTo(target->GetMapId(), moveX, moveY, moveZ, false, false, false, true,
           MovementPriority::MOVEMENT_FORCED);

    return false;
}

bool IccBqlVampiricBiteAction::CastVampiricBite(Player* target)
{
    if (IsInvalidTarget(target) || !target->IsAlive())
        return false;

    return botAI->CanCastSpell("Vampiric Bite", target) && botAI->CastSpell("Vampiric Bite", target);
}

// Sister Svalna
bool IccValkyreSpearAction::Execute(Event /*event*/)
{
    // Find the nearest spear
    Creature* spear = bot->FindNearestCreature(NPC_SPEAR, 100.0f);
    if (!spear)
        return false;

    // Move to the spear if not in range
    if (!spear->IsWithinDistInMap(bot, INTERACTION_DISTANCE))
        return MoveTo(spear, INTERACTION_DISTANCE);

    // Remove shapeshift forms
    botAI->RemoveShapeshift();

    // Stop movement and click the spear
    bot->GetMotionMaster()->Clear();
    bot->StopMoving();
    spear->HandleSpellClick(bot);

    // Dismount if mounted
    WorldPacket emptyPacket;
    bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);

    return false;
}

bool IccSisterSvalnaAction::Execute(Event /*event*/)
{
    Unit* svalna = AI_VALUE2(Unit*, "find target", "sister svalna");
    if (!svalna || !svalna->HasAura(SPELL_AETHER_SHIELD)) // Check for Aether Shield aura
        return false;

    // Check if bot has the spear item
    if (!botAI->HasItemInInventory(ITEM_SPEAR))
        return false;

    // Get all items from inventory
    std::vector<Item*> items = botAI->GetInventoryItems();
    for (Item* item : items)
    {
        if (item->GetEntry() == ITEM_SPEAR) // Spear ID
        {
            botAI->ImbueItem(item, svalna); // Use spear on Svalna
            return false;
        }
    }

    return false;
}

// VDW
static std::vector<Creature*> GetCreaturesByEntry(WorldObject* searcher, uint32 entry, float range)
{
    std::list<Creature*> raw;
    searcher->GetCreatureListWithEntryInGrid(raw, entry, range);

    std::vector<Creature*> out;
    out.reserve(raw.size());
    for (Creature* c : raw)
        if (c && c->IsAlive())
            out.push_back(c);

    return out;
}

static std::vector<Creature*> GetCreaturesByEntries(WorldObject* searcher, std::initializer_list<uint32> entries, float range)
{
    std::vector<Creature*> out;
    for (uint32 entry : entries)
    {
        auto part = GetCreaturesByEntry(searcher, entry, range);
        out.insert(out.end(), part.begin(), part.end());
    }
    std::sort(out.begin(), out.end(), [](Creature const* a, Creature const* b) { return a->GetGUID() < b->GetGUID(); });
    out.erase(std::unique(out.begin(), out.end()), out.end());

    return out;
}

bool IccValithriaGroupAction::Execute(Event /*event*/)
{
    Creature* portal =
        GetCreaturesByEntries(
            bot, {NPC_DREAM_PORTAL, NPC_DREAM_PORTAL_PRE_EFFECT, NPC_NIGHTMARE_PORTAL, NPC_NIGHTMARE_PORTAL_PRE_EFFECT},
            100.0f)
                .empty()
            ? nullptr
            : GetCreaturesByEntries(bot,
                                    {NPC_DREAM_PORTAL, NPC_DREAM_PORTAL_PRE_EFFECT, NPC_NIGHTMARE_PORTAL,
                                     NPC_NIGHTMARE_PORTAL_PRE_EFFECT},
                                    100.0f)
                  .front();

    Creature* worm = bot->FindNearestCreature(NPC_ROT_WORM, 100.0f);
    Creature* zombie = bot->FindNearestCreature(NPC_BLISTERING_ZOMBIE, 100.0f);
    Creature* manaVoid = bot->FindNearestCreature(NPC_MANA_VOID, 100.0f);

    // Column of Frost units – still hostile so the hostile list is fine here
    GuidVector const npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    std::vector<Unit*> frostColumns;
    for (ObjectGuid const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && unit->GetEntry() == NPC_COLUMN_OF_FROST)
            frostColumns.push_back(unit);
    }

    // Tanks aggro Gluttonous Abominations / Rot Worms not already on a tank
    if (botAI->IsTank(bot))
    {
        for (ObjectGuid const& guid : AI_VALUE(GuidVector, "possible targets"))
        {
            Unit* unit = botAI->GetUnit(guid);
            if (!unit || !unit->IsAlive())
                continue;
            if (unit->GetEntry() != NPC_GLUTTONOUS_ABOMINATION && unit->GetEntry() != NPC_ROT_WORM)
                continue;

            if (Unit* victim = unit->GetVictim())
                if (victim->IsPlayer() && botAI->IsTank(victim->ToPlayer()))
                    continue;

            if (unit->GetVictim() != bot)
            {
                bot->SetTarget(unit->GetGUID());
                bot->SetFacingToObject(unit);
                Attack(unit);
            }
        }
    }

    // Healers move toward the heal position when no portal is active
    if (botAI->IsHeal(bot) && !portal && bot->GetExactDist2d(ICC_VDW_HEAL_POSITION) > 30.0f)
    {
        return MoveTo(bot->GetMapId(), ICC_VDW_HEAL_POSITION.GetPositionX(), ICC_VDW_HEAL_POSITION.GetPositionY(),
                      ICC_VDW_HEAL_POSITION.GetPositionZ(), false, false, false, false,
                      MovementPriority::MOVEMENT_NORMAL);
    }

    // Avoidance
    if (manaVoid && bot->GetExactDist2d(manaVoid) < 10.0f && !botAI->GetAura("Twisted Nightmares", bot) &&
        !botAI->GetAura("Emerald Vigor", bot))
    {
        botAI->Reset();
        FleePosition(manaVoid->GetPosition(), 11.0f, 250U);
    }

    for (Unit* column : frostColumns)
    {
        if (column && bot->GetExactDist2d(column) < 7.0f)
        {
            botAI->Reset();
            FleePosition(column->GetPosition(), 8.0f, 250U);
        }
    }

    if (worm && worm->IsAlive() && worm->GetVictim() == bot && !botAI->IsTank(bot))
    {
        botAI->Reset();
        FleePosition(worm->GetPosition(), 10.0f, 250U);
    }

    if (zombie && zombie->IsAlive() && zombie->GetVictim() == bot && !botAI->IsTank(bot) &&
        bot->GetExactDist2d(zombie) < 20.0f)
    {
        botAI->Reset();
        FleePosition(zombie->GetPosition(), 21.0f, 250U);
    }

    if (zombie && !botAI->IsMainTank(bot) && !botAI->IsHeal(bot) && zombie->GetVictim() != bot)
        ApplyCrowdControl(zombie);

    Difficulty const diff = bot->GetRaidDifficulty();
    Group* group = bot->GetGroup();

    if (group && (diff == RAID_DIFFICULTY_25MAN_NORMAL || diff == RAID_DIFFICULTY_25MAN_HEROIC))
        return Handle25ManGroupLogic();

    return Handle10ManGroupLogic();
}

bool IccValithriaGroupAction::ApplyCrowdControl(Unit* zombie)
{
    switch (bot->getClass())
    {
        case CLASS_MAGE:
            if (!botAI->HasAura("Frost Nova", zombie))
                botAI->CastSpell("Frost Nova", zombie);
            break;
        case CLASS_DRUID:
            if (!botAI->HasAura("Entangling Roots", zombie))
                botAI->CastSpell("Entangling Roots", zombie);
            break;
        case CLASS_PALADIN:
            if (!botAI->HasAura("Hammer of Justice", zombie))
                botAI->CastSpell("Hammer of Justice", zombie);
            break;
        case CLASS_WARRIOR:
            if (!botAI->HasAura("Hamstring", zombie))
                botAI->CastSpell("Hamstring", zombie);
            break;
        case CLASS_HUNTER:
            if (!botAI->HasAura("Concussive Shot", zombie))
                botAI->CastSpell("Concussive Shot", zombie);
            break;
        case CLASS_ROGUE:
            if (!botAI->HasAura("Kidney Shot", zombie))
                botAI->CastSpell("Kidney Shot", zombie);
            break;
        case CLASS_SHAMAN:
            if (!botAI->HasAura("Frost Shock", zombie))
                botAI->CastSpell("Frost Shock", zombie);
            break;
        case CLASS_DEATH_KNIGHT:
            if (!botAI->HasAura("Chains of Ice", zombie))
                botAI->CastSpell("Chains of Ice", zombie);
            break;
        default:
            break;
    }

    return false;
}

bool IccValithriaGroupAction::MoveTowardsPosition(Position const& pos, float increment)
{
    float const dx = pos.GetPositionX() - bot->GetPositionX();
    float const dy = pos.GetPositionY() - bot->GetPositionY();
    float const dz = pos.GetPositionZ() - bot->GetPositionZ();
    float const dist = std::hypot(dx, dy);

    float moveX, moveY, moveZ;
    if (dist > increment)
    {
        moveX = bot->GetPositionX() + (dx / dist) * increment;
        moveY = bot->GetPositionY() + (dy / dist) * increment;
        moveZ = bot->GetPositionZ() + (dz / dist) * increment;
    }
    else
    {
        moveX = pos.GetPositionX();
        moveY = pos.GetPositionY();
        moveZ = pos.GetPositionZ();
    }

    MoveTo(bot->GetMapId(), moveX, moveY, moveZ, false, false, false, true, MovementPriority::MOVEMENT_COMBAT, true, false);

    return false;
}

bool IccValithriaGroupAction::Handle25ManGroupLogic()
{
    Position const group1Pos = ICC_VDW_GROUP1_POSITION;
    Position const group2Pos = ICC_VDW_GROUP2_POSITION;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    std::vector<Player*> tanks, dps;
    for (GroupReference* itr = group->GetFirstMember(); itr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || !member->IsAlive() || botAI->IsHeal(member))
            continue;

        if (botAI->IsTank(member))
            tanks.push_back(member);
        else
            dps.push_back(member);
    }

    std::vector<Player*> group1, group2;
    if (!tanks.empty())
        group1.push_back(tanks[0]);
    if (tanks.size() > 1)
        group2.push_back(tanks[1]);
    else if (tanks.size() == 1 && !dps.empty())
        group2.push_back(dps[0]);

    std::set<ObjectGuid> assigned;
    for (Player* p : group1)
        assigned.insert(p->GetGUID());
    for (Player* p : group2)
        assigned.insert(p->GetGUID());

    for (Player* p : dps)
    {
        if (assigned.count(p->GetGUID()))
            continue;
        (group1.size() <= group2.size() ? group1 : group2).push_back(p);
    }

    bool const inGroup1 = std::any_of(group1.begin(), group1.end(), [this](Player* p) { return p == bot; });
    bool const inGroup2 = std::any_of(group2.begin(), group2.end(), [this](Player* p) { return p == bot; });

    if (botAI->IsTank(bot) || botAI->IsDps(bot))
        HandleMarkingLogic(inGroup1, inGroup2, group1Pos, group2Pos);

    if (botAI->IsHeal(bot))
        return false;

    if (inGroup1 && bot->GetDistance(group1Pos) > 25.0f)
        MoveTowardsPosition(group1Pos, 5.0f);
    else if (inGroup2 && bot->GetDistance(group2Pos) > 25.0f)
        MoveTowardsPosition(group2Pos, 5.0f);

    return false;
}

bool IccValithriaGroupAction::HandleMarkingLogic(bool inGroup1, bool inGroup2, Position const& group1Pos,
                                                 Position const& group2Pos)
{
    static constexpr uint8_t SKULL_ICON = 7;
    static constexpr uint8_t CROSS_ICON = 6;
    static constexpr std::array<uint32, 6> ADD_PRIORITY = {NPC_BLAZING_SKELETON,       NPC_SUPPRESSER,
                                                           NPC_RISEN_ARCHMAGE,         NPC_BLISTERING_ZOMBIE,
                                                           NPC_GLUTTONOUS_ABOMINATION, NPC_ROT_WORM};

    uint8_t iconIndex;
    Position const* groupPos;
    std::string rtiValue;

    if (inGroup1)
    {
        iconIndex = SKULL_ICON;
        groupPos = &group1Pos;
        rtiValue = "skull";
    }
    else if (inGroup2)
    {
        iconIndex = CROSS_ICON;
        groupPos = &group2Pos;
        rtiValue = "cross";
    }
    else
        return false;

    context->GetValue<std::string>("rti")->Set(rtiValue);

    GuidVector const adds = AI_VALUE(GuidVector, "possible targets");
    Unit* priorityTarget = nullptr;

    for (uint32 entry : ADD_PRIORITY)
    {
        for (ObjectGuid const& guid : adds)
        {
            Unit* unit = botAI->GetUnit(guid);
            if (!unit || !unit->IsAlive() || unit->GetEntry() != entry)
                continue;
            if (unit->GetExactDist2d(groupPos->GetPositionX(), groupPos->GetPositionY()) <= 40.0f)
            {
                priorityTarget = unit;
                break;
            }
        }
        if (priorityTarget)
            break;
    }

    if (!priorityTarget)
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    bool hasOtherIcon = false;
    for (uint8 i = 0; i < 8; ++i)
    {
        if (i == iconIndex)
            continue;
        if (group->GetTargetIcon(i) == priorityTarget->GetGUID())
        {
            hasOtherIcon = true;
            break;
        }
    }

    if (!hasOtherIcon)
    {
        Unit* currentIconUnit = botAI->GetUnit(group->GetTargetIcon(iconIndex));
        if (!currentIconUnit || !currentIconUnit->IsAlive() || currentIconUnit != priorityTarget)
            group->SetTargetIcon(iconIndex, bot->GetGUID(), priorityTarget->GetGUID());
    }

    return false;
}

bool IccValithriaGroupAction::Handle10ManGroupLogic()
{
    static constexpr uint8_t DEFAULT_ICON = 7;
    static constexpr std::array<uint32, 6> ADD_PRIORITY = {NPC_BLAZING_SKELETON,       NPC_SUPPRESSER,
                                                           NPC_RISEN_ARCHMAGE,         NPC_BLISTERING_ZOMBIE,
                                                           NPC_GLUTTONOUS_ABOMINATION, NPC_ROT_WORM};

    Group* group = bot->GetGroup();
    if (group)
    {
        GuidVector const adds = AI_VALUE(GuidVector, "possible targets");
        Unit* priorityTarget = nullptr;

        for (uint32 entry : ADD_PRIORITY)
        {
            for (ObjectGuid const& guid : adds)
            {
                Unit* unit = botAI->GetUnit(guid);
                if (!unit || !unit->IsAlive() || unit->GetEntry() != entry)
                    continue;
                if (unit->GetExactDist2d(ICC_VDW_HEAL_POSITION.GetPositionX(), ICC_VDW_HEAL_POSITION.GetPositionY()) <=
                    50.0f)
                {
                    priorityTarget = unit;
                    break;
                }
            }
            if (priorityTarget)
                break;
        }

        if (priorityTarget)
        {
            Unit* currentIconUnit = botAI->GetUnit(group->GetTargetIcon(DEFAULT_ICON));
            if (!currentIconUnit || !currentIconUnit->IsAlive() || currentIconUnit != priorityTarget)
                group->SetTargetIcon(DEFAULT_ICON, bot->GetGUID(), priorityTarget->GetGUID());
        }
    }

    if (bot->GetExactDist2d(ICC_VDW_HEAL_POSITION.GetPositionX(), ICC_VDW_HEAL_POSITION.GetPositionY()) > 25.0f)
        MoveTowardsPosition(ICC_VDW_HEAL_POSITION, 5.0f);

    return false;
}

bool IccValithriaPortalAction::Execute(Event /*event*/)
{
    if (!botAI->IsHeal(bot) || bot->HasAura(SPELL_DREAM_STATE))
        return false;

    constexpr float SEARCH_RANGE = 200.0f;

    std::vector<Creature*> preEffectPortals =
        GetCreaturesByEntries(bot, {NPC_DREAM_PORTAL_PRE_EFFECT, NPC_NIGHTMARE_PORTAL_PRE_EFFECT}, SEARCH_RANGE);

    std::vector<Creature*> realPortals =
        GetCreaturesByEntries(bot, {NPC_DREAM_PORTAL, NPC_NIGHTMARE_PORTAL}, SEARCH_RANGE);

    if (preEffectPortals.empty() && realPortals.empty())
        return false;

    // Deterministic per-healer portal assignment by sorted GUID index
    Group* group = bot->GetGroup();
    std::vector<Player*> healers;

    if (group)
    {
        for (GroupReference* itr = group->GetFirstMember(); itr; itr = itr->next())
        {
            Player* member = itr->GetSource();
            if (member && member->IsAlive() && botAI->IsHeal(member))
                healers.push_back(member);
        }
        std::sort(healers.begin(), healers.end(),
                  [](Player const* a, Player const* b) { return a->GetGUID() < b->GetGUID(); });
    }
    else
        healers.push_back(bot);

    auto const it = std::find(healers.begin(), healers.end(), bot);
    if (it == healers.end())
        return false;

    size_t const healerIndex = std::distance(healers.begin(), it);

    if (!preEffectPortals.empty())
    {
        Creature* assigned = preEffectPortals[healerIndex % preEffectPortals.size()];

        if (bot->GetDistance2d(assigned->GetPositionX(), assigned->GetPositionY()) > 0.5f)
        {
            MoveTo(assigned->GetMapId(), assigned->GetPositionX(), assigned->GetPositionY(), assigned->GetPositionZ(),
                   false, false, false, true, MovementPriority::MOVEMENT_NORMAL);
        }

        botAI->RemoveShapeshift();

        // Click the real portal once it spawns within reach
        Creature* nearestReal = nullptr;
        float minDist = 9999.0f;
        for (Creature* portal : realPortals)
        {
            float const d = bot->GetDistance2d(portal);
            if (d < 3.0f && d < minDist)
            {
                nearestReal = portal;
                minDist = d;
            }
        }

        if (nearestReal)
        {
            botAI->RemoveShapeshift();
            bot->GetMotionMaster()->Clear();
            bot->StopMoving();
            bot->SetFacingToObject(nearestReal);
            nearestReal->HandleSpellClick(bot);
            return true;
        }
        return false;
    }

    // Fallback: only real portals visible
    Creature* nearestReal = nullptr;
    float minDist = 9999.0f;
    for (Creature* portal : realPortals)
    {
        float const d = bot->GetDistance2d(portal);
        if (d < 3.0f && d < minDist)
        {
            nearestReal = portal;
            minDist = d;
        }
    }

    if (nearestReal && minDist > 2.0f)
    {
        MoveTo(bot->GetMapId(), nearestReal->GetPositionX(), nearestReal->GetPositionY(), nearestReal->GetPositionZ(),
               false, false, false, true, MovementPriority::MOVEMENT_NORMAL);
    }

    if (nearestReal)
    {
        botAI->RemoveShapeshift();
        bot->GetMotionMaster()->Clear();
        bot->StopMoving();
        bot->SetFacingToObject(nearestReal);
        nearestReal->HandleSpellClick(bot);
        return true;
    }

    return false;
}

bool IccValithriaHealAction::Execute(Event /*event*/)
{
    if (!botAI->IsHeal(bot) || bot->GetHealthPct() < 50.0f)
        return false;

    if (!bot->HasAura(SPELL_DREAM_STATE))
    {
        constexpr float NORMAL_SPEED = 1.0f;
        bot->SetSpeed(MOVE_RUN, NORMAL_SPEED, true);
        bot->SetSpeed(MOVE_WALK, NORMAL_SPEED, true);
        bot->SetSpeed(MOVE_FLIGHT, NORMAL_SPEED, true);
    }

    constexpr float MAX_Z = 367.961f;
    constexpr float TARGET_Z = 365.0f;
    if (bot->GetPositionZ() > MAX_Z)
        bot->TeleportTo(bot->GetMapId(), bot->GetPositionX(), bot->GetPositionY(), TARGET_Z, bot->GetOrientation());

    Creature* valithria = bot->FindNearestCreature(NPC_VALITHRIA_DREAMWALKER, 100.0f);
    if (!valithria)
        return false;

    switch (bot->getClass())
    {
        case CLASS_DRUID:
        {
            constexpr uint32 SPELL_REJUVENATION = 48441;
            constexpr uint32 SPELL_REGROWTH = 48443;
            constexpr uint32 SPELL_LIFEBLOOM = 48451;
            constexpr uint32 SPELL_WILD_GROWTH = 53251;
            constexpr uint8 LIFEBLOOM_MAX = 3;

            if (!valithria->HasAura(SPELL_REJUVENATION, bot->GetGUID()))
                return botAI->CastSpell(SPELL_REJUVENATION, valithria);

            if (!valithria->HasAura(SPELL_REGROWTH, bot->GetGUID()))
                return botAI->CastSpell(SPELL_REGROWTH, valithria);

            Aura* lb = valithria->GetAura(SPELL_LIFEBLOOM, bot->GetGUID());
            if (!lb || lb->GetStackAmount() < LIFEBLOOM_MAX)
                return botAI->CastSpell(SPELL_LIFEBLOOM, valithria);

            return botAI->CastSpell(SPELL_WILD_GROWTH, valithria);
        }
        case CLASS_SHAMAN:
        {
            constexpr uint32 SPELL_RIPTIDE = 61301;
            constexpr uint32 SPELL_HEALING_WAVE = 49273;
            return valithria->HasAura(SPELL_RIPTIDE, bot->GetGUID()) ? botAI->CastSpell(SPELL_HEALING_WAVE, valithria)
                                                                     : botAI->CastSpell(SPELL_RIPTIDE, valithria);
        }
        case CLASS_PRIEST:
        {
            constexpr uint32 SPELL_RENEW = 48068;
            constexpr uint32 SPELL_GREATER_HEAL = 48063;
            return valithria->HasAura(SPELL_RENEW, bot->GetGUID()) ? botAI->CastSpell(SPELL_GREATER_HEAL, valithria)
                                                                   : botAI->CastSpell(SPELL_RENEW, valithria);
        }
        case CLASS_PALADIN:
        {
            constexpr uint32 SPELL_BEACON = 53563;
            constexpr uint32 SPELL_HOLY_LIGHT = 48782;
            return valithria->HasAura(SPELL_BEACON, bot->GetGUID()) ? botAI->CastSpell(SPELL_HOLY_LIGHT, valithria)
                                                                    : botAI->CastSpell(SPELL_BEACON, valithria);
        }
        default:
            return false;
    }
}

bool IccValithriaDreamCloudAction::Execute(Event /*event*/)
{
    if (!bot->HasAura(SPELL_DREAM_STATE))
        return false;

    bot->SetSpeed(MOVE_RUN, 2.0f, true);
    bot->SetSpeed(MOVE_WALK, 2.0f, true);
    bot->SetSpeed(MOVE_FLIGHT, 2.0f, true);

    std::vector<Player*> allDream;
    std::vector<Player*> realDream;

    Map::PlayerList const& playerList = bot->GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
    {
        Player* player = itr->GetSource();
        if (!player || !player->IsAlive() || !player->HasAura(SPELL_DREAM_STATE))
            continue;

        allDream.push_back(player);

        PlayerbotAI* playerBotAI = GET_PLAYERBOT_AI(player);
        if (!playerBotAI || playerBotAI->IsRealPlayer())
            realDream.push_back(player);
    }

    if (allDream.empty())
        return false;

    std::vector<Player*>& leaderPool = realDream.empty() ? allDream : realDream;

    std::sort(leaderPool.begin(), leaderPool.end(),
              [](Player const* a, Player const* b) { return a->GetGUID() < b->GetGUID(); });

    Player* leader = leaderPool.front();

    if (!realDream.empty())
    {
        constexpr float STACK_DIST = 1.0f;
        if (bot->GetDistance(leader) > STACK_DIST)
        {
            bot->TeleportTo(bot->GetMapId(), leader->GetPositionX(), leader->GetPositionY(), leader->GetPositionZ(),
                            bot->GetOrientation());
        }
        return false;
    }

    // No real players — bot-only logic unchanged
    std::vector<Creature*> dreamClouds = CollectClouds(NPC_DREAM_CLOUD, leader);
    std::vector<Creature*> nightmareClouds = CollectClouds(NPC_NIGHTMARE_CLOUD, leader);

    if (dreamClouds.empty() && nightmareClouds.empty())
        return false;

    Creature* target = SelectNextCloud(dreamClouds, nightmareClouds, leader);
    if (!target || leader->GetExactDist(target) <= 2.0f)
        return false;

    bot->TeleportTo(target->GetMapId(), target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(),
                    bot->GetOrientation());

    return false;
}

// Collect all live clouds sorted by distance from the given reference unit
std::vector<Creature*> IccValithriaDreamCloudAction::CollectClouds(uint32 entry, Unit* reference)
{
    constexpr float SEARCH_RANGE = 200.0f;

    std::list<Creature*> raw;
    bot->GetCreatureListWithEntryInGrid(raw, entry, SEARCH_RANGE);

    std::vector<Creature*> result;
    result.reserve(raw.size());
    for (Creature* c : raw)
        if (c && c->IsAlive())
            result.push_back(c);

    // Sort by reference unit's distance so all bots get the same ordering
    std::sort(result.begin(), result.end(), [reference](Creature const* a, Creature const* b)
              { return reference->GetExactDist(a) < reference->GetExactDist(b); });

    return result;
}

// Choose the next cloud target using the leader's position for all checks
Creature* IccValithriaDreamCloudAction::SelectNextCloud(std::vector<Creature*> const& dreamClouds, std::vector<Creature*> const& nightmareClouds, Unit* leader)
{
    bool const preferDream = leader->HasAura(SPELL_EMERALD_VIGOR) || (leader->GetGUID().GetCounter() % 2 == 0);

    std::vector<Creature*> const& pref = preferDream ? dreamClouds : nightmareClouds;
    std::vector<Creature*> const& sec = preferDream ? nightmareClouds : dreamClouds;

    bool const atPref = !pref.empty() && leader->GetExactDist(pref[0]) <= 2.0f;
    bool const atSec = !sec.empty() && leader->GetExactDist(sec[0]) <= 2.0f;

    return PickCloudTarget(pref, sec, atPref, atSec, leader);
}

Creature* IccValithriaDreamCloudAction::PickCloudTarget(std::vector<Creature*> const& preferred, std::vector<Creature*> const& secondary, bool atPreferred, bool atSecondary, Unit* leader)
{
    constexpr float AT_RADIUS = 2.0f;

    if (atPreferred)
    {
        if (preferred.size() >= 2 && leader->GetExactDist(preferred[1]) > AT_RADIUS)
            return preferred[1];
        if (!secondary.empty() && leader->GetExactDist(secondary[0]) > AT_RADIUS)
            return secondary[0];
    }
    else if (atSecondary)
    {
        if (!preferred.empty() && leader->GetExactDist(preferred[0]) > AT_RADIUS)
            return preferred[0];
        if (secondary.size() >= 2 && leader->GetExactDist(secondary[1]) > AT_RADIUS)
            return secondary[1];
    }
    else
    {
        if (!preferred.empty() && leader->GetExactDist(preferred[0]) > AT_RADIUS)
            return preferred[0];
        if (!secondary.empty() && leader->GetExactDist(secondary[0]) > AT_RADIUS)
            return secondary[0];
    }

    return nullptr;
}

// Sindragosa

bool IccSindragosaGroupPositionAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sindragosa");
    if (!boss)
        return false;

    // Block positioning only during the pre-combat intro fly-in.
    // Once Sindragosa is in combat the air phase is handled separately,
    // so ground bots must still be allowed to pre-position.
    if (boss->HasUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY) && !boss->IsInCombat())
        return false;

    Aura* aura = botAI->GetAura("mystic buffet", bot, false, true);
    if (aura && aura->GetStackAmount() >= 6 && botAI->IsMainTank(bot))
        return false;

    // Route tanks to tank positioning.
    // At pull GetVictim() may still be nullptr, so also check IsMainTank so the
    // tank is not incorrectly sent to the melee stack before aggro is established.
    bool const isTankingBoss = botAI->IsTank(bot) && (boss->GetVictim() == bot || botAI->IsMainTank(bot));
    if (isTankingBoss)
        return HandleTankPositioning(boss);

    // Everyone else: boss is not targeting this bot
    if (boss->GetVictim() != bot)
        return HandleNonTankPositioning();

    return false;
}

bool IccSindragosaGroupPositionAction::HandleTankPositioning(Unit* boss)
{
    float const distBossToCenter = boss->GetExactDist2d(ICC_SINDRAGOSA_CENTER_POSITION);
    float const distToTankPos = bot->GetExactDist2d(ICC_SINDRAGOSA_TANK_POSITION);

    // Compute how far the boss orientation deviates from east (PI/2)
    float const targetOrientation = fmod(float(M_PI) / 2.0f + 2.0f * float(M_PI), 2.0f * float(M_PI));
    float const currentOrientation = fmod(boss->GetOrientation() + 2.0f * float(M_PI), 2.0f * float(M_PI));
    float orientationDiff = currentOrientation - targetOrientation;

    // Clamp difference to [-PI, PI]
    if (orientationDiff > float(M_PI))
        orientationDiff -= 2.0f * float(M_PI);
    else if (orientationDiff < -float(M_PI))
        orientationDiff += 2.0f * float(M_PI);

    // Stage 1: Drag boss toward the arena centre when it has drifted too far
    if (distBossToCenter > 16.0f && distToTankPos <= 20.0f)
    {
        float const dirX = ICC_SINDRAGOSA_CENTER_POSITION.GetPositionX() - boss->GetPositionX();
        float const dirY = ICC_SINDRAGOSA_CENTER_POSITION.GetPositionY() - boss->GetPositionY();

        // Step 4 yards past centre to keep the boss moving through it
        float const moveX = ICC_SINDRAGOSA_CENTER_POSITION.GetPositionX() + (dirX / distBossToCenter) * 4.0f;
        float const moveY = ICC_SINDRAGOSA_CENTER_POSITION.GetPositionY() + (dirY / distBossToCenter) * 4.0f;

        return MoveTo(bot->GetMapId(), moveX, moveY, boss->GetPositionZ(), false, false, false, false,
                      MovementPriority::MOVEMENT_FORCED, true, false);
    }

    // Stage 2: Walk toward the designated tank position
    if (distToTankPos > 10.0f)
    {
        Position const& botPos = bot->GetPosition();
        Position const& tankPos = ICC_SINDRAGOSA_TANK_POSITION;

        float const dx = tankPos.GetPositionX() - botPos.GetPositionX();
        float const dy = tankPos.GetPositionY() - botPos.GetPositionY();
        float const distance = std::hypot(dx, dy);

        // Advance one yard at a time for smooth pathing
        float const scale = 1.0f / distance;
        float const targetX = botPos.GetPositionX() + dx * scale;
        float const targetY = botPos.GetPositionY() + dy * scale;

        return MoveTo(bot->GetMapId(), targetX, targetY, bot->GetPositionZ(), false, false, false, true,
                      MovementPriority::MOVEMENT_COMBAT, true, false);
    }

    // Stage 3: Arc around the boss to correct its facing toward east
    if (std::abs(orientationDiff) > 0.15f)
    {
        float const centerX = boss->GetPositionX();
        float const centerY = boss->GetPositionY();
        float const radius = std::max(2.0f, bot->GetExactDist2d(centerX, centerY));

        float angle = atan2(bot->GetPositionY() - centerY, bot->GetPositionX() - centerX);

        // Negative diff → step counterclockwise (north); positive → clockwise (south)
        static constexpr float ARC_STEP = 0.125f;
        angle += (orientationDiff < 0) ? ARC_STEP : -ARC_STEP;

        float const moveX = centerX + radius * cos(angle);
        float const moveY = centerY + radius * sin(angle);

        return MoveTo(bot->GetMapId(), moveX, moveY, bot->GetPositionZ(), false, false, false, false,
                      MovementPriority::MOVEMENT_FORCED, true, false);
    }

    // Stage 4: Fine-tune Y-axis alignment with the tank position
    float const yDiff = std::abs(bot->GetPositionY() - ICC_SINDRAGOSA_TANK_POSITION.GetPositionY());
    if (yDiff > 2.0f)
    {
        Position const& botPos = bot->GetPosition();
        Position const& tankPos = ICC_SINDRAGOSA_TANK_POSITION;

        float const newY = botPos.GetPositionY() + (tankPos.GetPositionY() > botPos.GetPositionY() ? 1.0f : -1.0f);

        return MoveTo(bot->GetMapId(), botPos.GetPositionX(), newY, botPos.GetPositionZ(), false, false, false, false,
                      MovementPriority::MOVEMENT_FORCED, true, false);
    }

    return false;
}

bool IccSindragosaGroupPositionAction::HandleNonTankPositioning()
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    // Collect all alive raid members
    std::vector<Player*> raidMembers;
    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || !member->IsAlive())
            continue;

        raidMembers.push_back(member);
    }

    uint32 const totalMembers = static_cast<uint32>(raidMembers.size());
    if (totalMembers == 0)
        return false;

    // Count members currently free of Mystic Buffet
    uint32 membersWithoutAura = 0;
    for (Player* member : raidMembers)
    {
        if (!botAI->GetAura("mystic buffet", member))
            ++membersWithoutAura;
    }

    // Raid is considered "clear" when 60 % or more lack the debuff stack
    float const percentageWithoutAura = static_cast<float>(membersWithoutAura) / static_cast<float>(totalMembers);
    bool const raidClear = (percentageWithoutAura >= 0.6f);

    if (raidClear && botAI->IsTank(bot))
    {
        static constexpr std::array<uint32, 4> TombEntries = {NPC_TOMB1, NPC_TOMB2, NPC_TOMB3, NPC_TOMB4};
        GuidVector const tombGuids = AI_VALUE(GuidVector, "possible targets no los");

        Unit* nearestTomb = nullptr;
        float minDist = 150.0f;

        for (uint32 const entry : TombEntries)
        {
            for (auto const& guid : tombGuids)
            {
                Unit* unit = botAI->GetUnit(guid);
                if (!unit || unit->GetEntry() != entry || !unit->IsAlive())
                    continue;

                float const dist = bot->GetDistance(unit);
                if (dist < minDist)
                {
                    minDist = dist;
                    nearestTomb = unit;
                }
            }
        }

        static constexpr uint8 SKULL_ICON_INDEX = 7;

        // Prefer the nearest tomb; fall back to marking the boss itself
        Unit* targetToMark = nearestTomb;
        if (!targetToMark)
        {
            Unit* boss = AI_VALUE2(Unit*, "find target", "sindragosa");
            if (boss && boss->IsAlive())
                targetToMark = boss;
        }

        if (targetToMark)
        {
            ObjectGuid const currentSkull = group->GetTargetIcon(SKULL_ICON_INDEX);
            Unit* const currentSkullUnit = botAI->GetUnit(currentSkull);
            bool const needsUpdate =
                !currentSkullUnit || !currentSkullUnit->IsAlive() || currentSkullUnit != targetToMark;

            if (needsUpdate)
                group->SetTargetIcon(SKULL_ICON_INDEX, bot->GetGUID(), targetToMark->GetGUID());
        }
    }

    context->GetValue<std::string>("rti")->Set("skull");

    if (botAI->IsRanged(bot))
    {
        static constexpr float RANGED_TOLERANCE = 9.0f;
        static constexpr float RANGED_MAX_STEP = 5.0f;

        if (bot->GetExactDist2d(ICC_SINDRAGOSA_RANGED_POSITION) > RANGED_TOLERANCE)
            return MoveIncrementallyToPosition(ICC_SINDRAGOSA_RANGED_POSITION, RANGED_MAX_STEP);

        return false;
    }

    static constexpr float MELEE_TOLERANCE = 10.0f;
    static constexpr float MELEE_MAX_STEP = 5.0f;

    if (bot->GetExactDist2d(ICC_SINDRAGOSA_MELEE_POSITION) > MELEE_TOLERANCE)
        return MoveIncrementallyToPosition(ICC_SINDRAGOSA_MELEE_POSITION, MELEE_MAX_STEP);

    return false;
}

bool IccSindragosaGroupPositionAction::MoveIncrementallyToPosition(Position const& targetPos, float maxStep)
{
    float const dirX = targetPos.GetPositionX() - bot->GetPositionX();
    float const dirY = targetPos.GetPositionY() - bot->GetPositionY();
    float const length = std::hypot(dirX, dirY);

    float const stepSize = std::min(maxStep, bot->GetExactDist2d(targetPos));
    float const moveX = bot->GetPositionX() + (dirX / length) * stepSize;
    float const moveY = bot->GetPositionY() + (dirY / length) * stepSize;

    return MoveTo(bot->GetMapId(), moveX, moveY, targetPos.GetPositionZ(), false, false, false, false,
                  MovementPriority::MOVEMENT_COMBAT);
}

bool IccSindragosaTankSwapPositionAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sindragosa");
    if (!boss)
        return false;

    if (!botAI->IsAssistTank(bot))
        return false;

    // Keep the assist tank on the swap position with a tight tolerance
    if (bot->GetExactDist2d(ICC_SINDRAGOSA_TANK_POSITION) > 3.0f)
    {
        return MoveTo(bot->GetMapId(), ICC_SINDRAGOSA_TANK_POSITION.GetPositionX(),
                      ICC_SINDRAGOSA_TANK_POSITION.GetPositionY(), ICC_SINDRAGOSA_TANK_POSITION.GetPositionZ(), false,
                      false, false, false, MovementPriority::MOVEMENT_FORCED, true, false);
    }

    return false;
}

bool IccSindragosaFrostBeaconAction::Execute(Event /*event*/)
{
    const Unit* boss = AI_VALUE2(Unit*, "find target", "sindragosa");
    if (!boss)
        return false;

    HandleSupportActions();

    if (bot->HasAura(FROST_BEACON_AURA_ID))
    {
        return HandleBeaconedPlayer(boss);
    }

    return HandleNonBeaconedPlayer(boss);
}

bool IccSindragosaFrostBeaconAction::HandleSupportActions()
{
    Group* group = bot->GetGroup();

    // Tank support - Paladin Hand of Freedom
    if (group && bot->getClass() == CLASS_PALADIN)
    {
        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            Player* member = itr->GetSource();
            if (!member || !member->IsAlive() || !botAI->IsTank(member))
            {
                continue;
            }

            if (botAI->GetAura("Frost Breath", member) && !member->HasAura(HAND_OF_FREEDOM_SPELL_ID))
            {
                botAI->CastSpell(HAND_OF_FREEDOM_SPELL_ID, member);
                break;
            }
        }
    }

    // Healer support - Apply HoTs to beaconed players
    if (botAI->IsHeal(bot) && !bot->HasAura(FROST_BEACON_AURA_ID))
    {
        const auto members = AI_VALUE(GuidVector, "group members");
        for (auto const& memberGuid : members)
        {
            Unit* member = botAI->GetUnit(memberGuid);
            if (!member || !member->IsAlive() || !member->HasAura(FROST_BEACON_AURA_ID))
            {
                continue;
            }

            // Apply class-specific HoT spells
            uint32 spellId = 0;
            switch (bot->getClass())
            {
                case CLASS_PRIEST:
                    spellId = 48068;
                    break;  // Renew
                case CLASS_SHAMAN:
                    spellId = 61301;
                    break;  // Riptide
                case CLASS_DRUID:
                    spellId = 48441;
                    break;  // Rejuvenation
                default:
                    continue;
            }

            if (!member->HasAura(spellId))
            {
                botAI->CastSpell(spellId, member);
            }
        }
    }

    return false;
}

bool IccSindragosaFrostBeaconAction::HandleBeaconedPlayer(const Unit* boss)
{
    // Phase 3 positioning (below 35% health, not flying)
    if (boss->HealthBelowPct(35) && !IsBossFlying(boss))
    {
        if (!bot->HasAura(SPELL_NITRO_BOOSTS))
            bot->AddAura(SPELL_NITRO_BOOSTS, bot);
        botAI->Reset();
        return MoveToPositionIfNeeded(ICC_SINDRAGOSA_THOMBMB2_POSITION, POSITION_TOLERANCE);
    }

    // Regular beacon positioning using tomb spots
    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }

    // Collect and sort beaconed players by GUID for deterministic assignment
    std::vector<Player*> beaconedPlayers;
    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (member && member->IsAlive() && member->HasAura(FROST_BEACON_AURA_ID))
        {
            beaconedPlayers.push_back(member);
        }
    }

    std::sort(beaconedPlayers.begin(), beaconedPlayers.end(),
              [](const Player* a, const Player* b) { return a->GetGUID() < b->GetGUID(); });

    // Find this bot's index
    const auto it = std::find(beaconedPlayers.begin(), beaconedPlayers.end(), bot);
    if (it == beaconedPlayers.end())
    {
        return false;
    }

    const size_t myIndex = std::distance(beaconedPlayers.begin(), it);
    const size_t beaconCount = beaconedPlayers.size();

    // Calculate tomb spot based on beacon count
    size_t spot = 0;
    switch (beaconCount)
    {
        case 2:
            spot = (myIndex == 0) ? 0 : 2;
            break;
        case 5:
            spot = (myIndex < 2) ? 0 : ((myIndex == 2) ? 1 : 2);
            break;
        case 6:
            spot = myIndex / 2;
            break;
        default:
            spot = myIndex % 3;
            break;
    }

    // Get tomb position and move if needed
    static constexpr std::array<const Position*, 3> tombPositions = {
        &ICC_SINDRAGOSA_THOMB1_POSITION, &ICC_SINDRAGOSA_THOMB2_POSITION, &ICC_SINDRAGOSA_THOMB3_POSITION};

    const Position& tombPosition = *tombPositions[std::min(spot, tombPositions.size() - 1)];
    return MoveToPositionIfNeeded(tombPosition, TOMB_POSITION_TOLERANCE);
}

bool IccSindragosaFrostBeaconAction::HandleNonBeaconedPlayer(const Unit* boss)
{
    // Collect beaconed players
    std::vector<Unit*> beaconedPlayers;
    const auto members = AI_VALUE(GuidVector, "group members");
    for (auto const& memberGuid : members)
    {
        Unit* player = botAI->GetUnit(memberGuid);
        if (player && player->GetGUID() != bot->GetGUID() && player->HasAura(FROST_BEACON_AURA_ID))
        {
            beaconedPlayers.push_back(player);
        }
    }

    if (beaconedPlayers.empty())
    {
        return false;
    }

    // Air phase positioning
    if (IsBossFlying(boss))
    {
        if (!bot->HasAura(FROST_BEACON_AURA_ID))
        {
            const Difficulty diff = bot->GetRaidDifficulty();
            bool is25Man = false;
            if (diff && (diff == RAID_DIFFICULTY_25MAN_NORMAL || diff == RAID_DIFFICULTY_25MAN_HEROIC))
                is25Man = true;

            const Position& safePosition = is25Man ? ICC_SINDRAGOSA_FBOMB_POSITION : ICC_SINDRAGOSA_FBOMB10_POSITION;

            const float dist = bot->GetExactDist2d(safePosition.GetPositionX(), safePosition.GetPositionY());
            if (dist > MOVE_TOLERANCE)
            {
                return MoveToPosition(safePosition);
            }
        }
        return botAI->IsHeal(bot);  // Continue for healers, wait for others
    }

    // Ground phase - position based on role and avoid beaconed players
    const bool isRanged = botAI->IsRanged(bot) || (bot->GetExactDist2d(ICC_SINDRAGOSA_RANGED_POSITION.GetPositionX(),ICC_SINDRAGOSA_RANGED_POSITION.GetPositionY()) <
                          bot->GetExactDist2d(ICC_SINDRAGOSA_MELEE_POSITION.GetPositionX(),ICC_SINDRAGOSA_MELEE_POSITION.GetPositionY()));

    const Position& targetPosition = isRanged ? ICC_SINDRAGOSA_RANGED_POSITION : ICC_SINDRAGOSA_MELEE_POSITION;

    const float deltaX = std::abs(targetPosition.GetPositionX() - bot->GetPositionX());
    const float deltaY = std::abs(targetPosition.GetPositionY() - bot->GetPositionY());
    if (boss && boss->GetVictim() != bot)
    {
        if ((deltaX > MOVE_TOLERANCE) || (deltaY > MOVE_TOLERANCE))
        {
            if (bot->HasUnitState(UNIT_STATE_CASTING))
            {
                botAI->Reset();
            }
            return MoveToPosition(targetPosition);
        }
    }
    return false;
}

bool IccSindragosaFrostBeaconAction::MoveToPositionIfNeeded(const Position& position, float tolerance)
{
    const float distance = bot->GetExactDist2d(position.GetPositionX(), position.GetPositionY());
    if (distance > tolerance)
    {
        return MoveToPosition(position);
    }
    return distance <= tolerance;
}

bool IccSindragosaFrostBeaconAction::MoveToPosition(const Position& position)
{
    float posX = position.GetPositionX();
    float posY = position.GetPositionY();
    float posZ = position.GetPositionZ();

    bot->UpdateAllowedPositionZ(posX, posY, posZ);

    return MoveTo(bot->GetMapId(), posX, posY, posZ, false, false, false, false, MovementPriority::MOVEMENT_FORCED,
                  true, false);
}

bool IccSindragosaFrostBeaconAction::IsBossFlying(const Unit* boss)
{
    return boss->GetExactDist2d(ICC_SINDRAGOSA_FLYING_POSITION.GetPositionX(),
                                ICC_SINDRAGOSA_FLYING_POSITION.GetPositionY()) < 30.0f;
}

bool IccSindragosaBlisteringColdAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sindragosa");
    if (!boss)
        return false;

    // Only non-tanks should move out
    if (botAI->IsMainTank(bot))
        return false;

   float dist = bot->GetExactDist2d(boss->GetPositionX(), boss->GetPositionY());

    if (dist >= 33.0f)
        return false;

    Position const& targetPos = ICC_SINDRAGOSA_BLISTERING_COLD_POSITION;

    // Only move if we're too close to the boss (< 30 yards)
    if (dist < 33.0f)
    {

        float const STEP_SIZE = 15.0f;
        float distToTarget = bot->GetDistance2d(targetPos.GetPositionX(), targetPos.GetPositionY());

        if (distToTarget > 0.1f)  // Avoid division by zero
        {
            if (!bot->HasAura(SPELL_NITRO_BOOSTS))
                bot->AddAura(SPELL_NITRO_BOOSTS, bot);
            // Calculate direction vector
            float dirX = targetPos.GetPositionX() - bot->GetPositionX();
            float dirY = targetPos.GetPositionY() - bot->GetPositionY();

            // Normalize direction vector
            float length = sqrt(dirX * dirX + dirY * dirY);
            dirX /= length;
            dirY /= length;

            // Move STEP_SIZE yards in that direction
            float moveX = bot->GetPositionX() + dirX * STEP_SIZE;
            float moveY = bot->GetPositionY() + dirY * STEP_SIZE;

            return MoveTo(bot->GetMapId(), moveX, moveY, bot->GetPositionZ(),
                         false, false, false, true, MovementPriority::MOVEMENT_FORCED, true, false);
        }
    }
    return false;
}

bool IccSindragosaUnchainedMagicAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sindragosa");
    if (!boss)
        return false;

    Aura* aura = botAI->GetAura("Unchained Magic", bot, false, true);
    if (!aura)
        return false;

    Aura* aura1 = botAI->GetAura("Instability", bot, false, true);

    Difficulty diff = bot->GetRaidDifficulty();
    if (aura && (diff == RAID_DIFFICULTY_10MAN_NORMAL || diff == RAID_DIFFICULTY_25MAN_NORMAL))
    {
        if (aura1 && aura1->GetStackAmount() >= 6)
            return true;  // Stop casting spells
    }

    return false;
}

bool IccSindragosaChilledToTheBoneAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sindragosa");
    if (!boss)
        return false;

    Aura* aura = botAI->GetAura("Chilled to the Bone", bot, false, true);
    if (!aura)
        return false;

    if (aura) // Chilled to the Bone
    {
        if (aura->GetStackAmount() >= 6)
        {
            botAI->Reset();
            bot->AttackStop();
            return true;
        }
    }

    return false;
}

bool IccSindragosaMysticBuffetAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sindragosa");
    if (!boss || !bot || !bot->IsAlive())
        return false;

    // Check if we have Mystic Buffet
    Aura* aura = botAI->GetAura("mystic buffet", bot, false, true);
    if (!aura)
        return false;

    if (boss->GetVictim() == bot)
        return false;

    // Skip if we have Frost Beacon
    if (bot->HasAura(SPELL_FROST_BEACON))
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    static const std::array<uint32, 4> tombEntries = {NPC_TOMB1, NPC_TOMB2, NPC_TOMB3, NPC_TOMB4};
    const GuidVector tombGuids = AI_VALUE(GuidVector, "possible targets no los");

    Unit* nearestTomb = nullptr;
    float minDist = 150.0f;

    for (const auto entry : tombEntries)
    {
        for (auto const& guid : tombGuids)
        {
            if (Unit* unit = botAI->GetUnit(guid))
            {
                if (unit->GetEntry() == entry && unit->IsAlive())
                {
                    float dist = bot->GetDistance(unit);
                    if (dist < minDist)
                    {
                        minDist = dist;
                        nearestTomb = unit;
                    }
                }
            }
        }
    }

    // Check if anyone in group has Frost Beacon (SPELL_FROST_BEACON)
    bool anyoneHasFrostBeacon = false;
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (member && member->IsAlive() && member->HasAura(SPELL_FROST_BEACON))
        {
            anyoneHasFrostBeacon = true;
            break;
        }
    }

    bool tombPresent = nearestTomb != nullptr;
    bool atLOS2 = bot->GetExactDist2d(ICC_SINDRAGOSA_LOS2_POSITION.GetPositionX(),
                                      ICC_SINDRAGOSA_LOS2_POSITION.GetPositionY()) <= 2.0f;

    // Move to LOS2 position if: tomb is present and no one has Frost Beacon
    bool shouldMoveLOS2 = tombPresent && !anyoneHasFrostBeacon;

    if (shouldMoveLOS2)
    {
        // If already at LOS2 and have 3+ stacks, stay still
        if (atLOS2 && aura && !botAI->IsHeal(bot))
        {
            return true;
        }

        botAI->Reset();
        // Move to LOS2 position
        return MoveTo(bot->GetMapId(), ICC_SINDRAGOSA_LOS2_POSITION.GetPositionX(),
                      ICC_SINDRAGOSA_LOS2_POSITION.GetPositionY(), ICC_SINDRAGOSA_LOS2_POSITION.GetPositionZ(), false,
                      false, false, true, MovementPriority::MOVEMENT_FORCED);
    }
    return false;
}

std::map<ObjectGuid, int> IccSindragosaFrostBombAction::s_groupAssignments;
std::vector<ObjectGuid> IccSindragosaFrostBombAction::s_allGroupGuids;

bool IccSindragosaFrostBombAction::Execute(Event /*event*/)
{
    if (!bot || !bot->IsAlive() || bot->HasAura(SPELL_ICE_TOMB))
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    FrostBombContext ctx;
    if (!CollectContext(ctx))
    {
        bot->AttackStop();
        return true;
    }

    int const groupIndex = ResolveGroupIndex(group);
    if (groupIndex < 0)
        return false;

    Difficulty const diff = bot->GetRaidDifficulty();
    int const groupCount = (diff == RAID_DIFFICULTY_25MAN_NORMAL || diff == RAID_DIFFICULTY_25MAN_HEROIC) ? 3 : 2;

    std::vector<Position> groupPositions;
    groupPositions.reserve(groupCount);
    for (int i = 0; i < groupCount; ++i)
        groupPositions.push_back(i < static_cast<int>(ctx.tombs.size()) ? ctx.tombs[i]->GetPosition() : ctx.marker->GetPosition());

    std::vector<Unit*> myTombs = SelectTombs(ctx.tombs, groupIndex, groupPositions);
    if (myTombs.empty())
        return false;

    Unit* losTomb = SelectBestTomb(myTombs);
    if (!losTomb)
        return false;

    float const angle = ctx.marker->GetAngle(losTomb);
    float const posX = losTomb->GetPositionX() + std::cos(angle) * 6.5f;
    float const posY = losTomb->GetPositionY() + std::sin(angle) * 6.5f;
    float const posZ = losTomb->GetPositionZ();

    if (bot->GetDistance2d(posX, posY) > 0.01f)
    {
        botAI->Reset();
        bot->AttackStop();
        return MoveTo(bot->GetMapId(), posX, posY, posZ, false, false, false, true, MovementPriority::MOVEMENT_FORCED);
    }

    return HandleRtiMarking(group, groupIndex, myTombs);
}

bool IccSindragosaFrostBombAction::CollectContext(FrostBombContext& ctx) const
{
    constexpr uint32 tombEntries[] = {NPC_TOMB1, NPC_TOMB2, NPC_TOMB3, NPC_TOMB4};

    std::list<Unit*> units;
    float const range = 200.0f;
    Acore::AnyUnitInObjectRangeCheck check(bot, range);
    Acore::UnitListSearcher<Acore::AnyUnitInObjectRangeCheck> searcher(bot, units, check);
    Cell::VisitObjects(bot, searcher, range);

    for (Unit* unit : units)
    {
        if (!unit || !unit->IsAlive())
            continue;

        if (unit->HasAura(SPELL_FROST_BOMB_VISUAL))
            ctx.marker = unit;

        for (uint32 entry : tombEntries)
        {
            if (unit->GetEntry() == entry)
            {
                ctx.tombs.push_back(unit);
                break;
            }
        }
    }

    return ctx.marker && !ctx.tombs.empty();
}

int IccSindragosaFrostBombAction::ResolveGroupIndex(Group* group) const
{
    // Accumulate all member GUIDs (alive and dead)
    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member)
            continue;

        ObjectGuid const guid = member->GetGUID();
        if (std::find(s_allGroupGuids.begin(), s_allGroupGuids.end(), guid) == s_allGroupGuids.end())
            s_allGroupGuids.push_back(guid);
    }

    std::sort(s_allGroupGuids.begin(), s_allGroupGuids.end());

    Difficulty const diff = bot->GetRaidDifficulty();
    int const groupCount = (diff == RAID_DIFFICULTY_25MAN_NORMAL || diff == RAID_DIFFICULTY_25MAN_HEROIC) ? 3 : 2;

    for (std::size_t i = 0; i < s_allGroupGuids.size(); ++i)
    {
        ObjectGuid const& guid = s_allGroupGuids[i];
        if (s_groupAssignments.find(guid) == s_groupAssignments.end())
            s_groupAssignments[guid] = static_cast<int>(i) % groupCount;
    }

    auto it = s_groupAssignments.find(bot->GetGUID());
    return it != s_groupAssignments.end() ? it->second : -1;
}

std::vector<Unit*> IccSindragosaFrostBombAction::SelectTombs(std::vector<Unit*> const& tombs, int groupIndex, std::vector<Position> const& groupPositions) const
{
    // Priority 1: tombs within 8 yards of bot's current position
    std::vector<Unit*> nearby;
    for (Unit* tomb : tombs)
    {
        if (tomb->GetExactDist2d(bot) <= 8.0f)
            nearby.push_back(tomb);
    }
    if (!nearby.empty())
        return nearby;

    // Priority 2: tombs within 8 yards of this group's assigned position
    std::vector<Unit*> atGroupPos;
    for (Unit* tomb : tombs)
    {
        if (tomb->GetExactDist2d(groupPositions[groupIndex]) <= 8.0f)
            atGroupPos.push_back(tomb);
    }
    if (!atGroupPos.empty())
        return atGroupPos;

    // Priority 3: closest tomb as fallback
    Unit* closest = nullptr;
    float closestDist = 999.0f;
    for (Unit* tomb : tombs)
    {
        float const dist = tomb->GetExactDist2d(bot);
        if (dist < closestDist)
        {
            closestDist = dist;
            closest = tomb;
        }
    }

    std::vector<Unit*> fallback;
    if (closest)
        fallback.push_back(closest);
    return fallback;
}

Unit* IccSindragosaFrostBombAction::SelectBestTomb(std::vector<Unit*> const& candidates) const
{
    Unit* best = nullptr;
    float bestHp = 0.0f;
    for (Unit* tomb : candidates)
    {
        float const hp = tomb->GetHealthPct();
        if (!best || hp > bestHp)
        {
            bestHp = hp;
            best = tomb;
        }
    }
    return best;
}

bool IccSindragosaFrostBombAction::HandleRtiMarking(Group* group, int groupIndex, std::vector<Unit*> const& myTombs)
{
    constexpr uint8 SKULL_ICON = 7;
    constexpr uint8 CROSS_ICON = 6;
    constexpr uint8 STAR_ICON = 0;

    uint8 iconIndex = 0;
    std::string rtiValue;

    switch (groupIndex)
    {
        case 0:
            iconIndex = SKULL_ICON;
            rtiValue = "skull";
            break;
        case 1:
            iconIndex = CROSS_ICON;
            rtiValue = "cross";
            break;
        case 2:
            iconIndex = STAR_ICON;
            rtiValue = "star";
            break;
        default:
            return false;
    }

    context->GetValue<std::string>("rti")->Set(rtiValue);

    Unit* tombToMark = nullptr;
    for (Unit* tomb : myTombs)
    {
        if (tomb->IsAlive() && tomb->HealthAbovePct(45))
        {
            tombToMark = tomb;
            break;
        }
    }

    if (!tombToMark)
    {
        ObjectGuid const currentIcon = group->GetTargetIcon(iconIndex);
        if (!currentIcon.IsEmpty())
            group->SetTargetIcon(iconIndex, bot->GetGUID(), ObjectGuid::Empty);

        bot->AttackStop();
        return true;
    }

    Unit* currentIconUnit = botAI->GetUnit(group->GetTargetIcon(iconIndex));
    if (!currentIconUnit || !currentIconUnit->IsAlive() || currentIconUnit != tombToMark)
        group->SetTargetIcon(iconIndex, bot->GetGUID(), tombToMark->GetGUID());

    return false;
}

// The Lich King

static bool IsLkShambling(uint32 entry)
{
    return entry == NPC_SHAMBLING_HORROR1 || entry == NPC_SHAMBLING_HORROR2 || entry == NPC_SHAMBLING_HORROR3 ||
           entry == NPC_SHAMBLING_HORROR4;
}

static bool IsLkValkyr(Unit* unit)
{
    if (!unit)
        return false;
    uint32 const entry = unit->GetEntry();
    return entry == NPC_VALKYR_SHADOWGUARD1 || entry == NPC_VALKYR_SHADOWGUARD2 || entry == NPC_VALKYR_SHADOWGUARD3 ||
           entry == NPC_VALKYR_SHADOWGUARD4;
}

static bool IsLkVileSpirit(Unit* unit)
{
    if (!unit)
        return false;
    uint32 const entry = unit->GetEntry();
    return entry == NPC_VILE_SPIRIT1 || entry == NPC_VILE_SPIRIT2 || entry == NPC_VILE_SPIRIT3 ||
           entry == NPC_VILE_SPIRIT4;
}

static bool IsValidLkCollectibleAdd(Unit* unit)
{
    if (!unit || !unit->IsAlive())
        return false;

    uint32 const entry = unit->GetEntry();
    return entry == NPC_SHAMBLING_HORROR1 || entry == NPC_SHAMBLING_HORROR2 || entry == NPC_SHAMBLING_HORROR3 ||
           entry == NPC_SHAMBLING_HORROR4 || entry == NPC_RAGING_SPIRIT1 || entry == NPC_RAGING_SPIRIT2 ||
           entry == NPC_RAGING_SPIRIT3 || entry == NPC_RAGING_SPIRIT4 || entry == NPC_DRUDGE_GHOUL1 ||
           entry == NPC_DRUDGE_GHOUL2 || entry == NPC_DRUDGE_GHOUL3 || entry == NPC_DRUDGE_GHOUL4;
}

static float GetDefileEffectiveRadius(Unit const* defile, Difficulty diff)
{
    static constexpr float BASE_RADIUS = 6.0f;

    for (uint32 const auraId : DEFILE_AURAS)
    {
        Aura const* grow = defile->GetAura(auraId);
        if (!grow)
            continue;

        float const multiplier =
            (diff == RAID_DIFFICULTY_10MAN_HEROIC || diff == RAID_DIFFICULTY_10MAN_NORMAL) ? 1.4f : 0.95f;

        return BASE_RADIUS + grow->GetStackAmount() * multiplier;
    }
    return BASE_RADIUS;
}

static bool IsHeroicLk(Difficulty diff)
{
    return diff == RAID_DIFFICULTY_10MAN_HEROIC || diff == RAID_DIFFICULTY_25MAN_HEROIC;
}

bool IccLichKingShadowTrapAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "the lich king");
    if (!boss || !botAI->IsTank(bot))
        return false;

    Difficulty const diff = bot->GetRaidDifficulty();

    if (sPlayerbotAIConfig.EnableICCBuffs && diff &&
        (diff == RAID_DIFFICULTY_10MAN_HEROIC || diff == RAID_DIFFICULTY_25MAN_HEROIC))
    {
        if (!bot->HasAura(SPELL_EXPERIENCED))
            bot->AddAura(SPELL_EXPERIENCED, bot);

        if (!bot->HasAura(SPELL_AGEIS_OF_DALARAN))
            bot->AddAura(SPELL_AGEIS_OF_DALARAN, bot);

        if (!bot->HasAura(SPELL_PAIN_SUPPRESION))
            bot->AddAura(SPELL_PAIN_SUPPRESION, bot);
    }

    static constexpr float CIRCLE_RADIUS = 20.0f;
    static constexpr float SAFE_DISTANCE = 12.0f;
    static constexpr float SEARCH_DISTANCE = SAFE_DISTANCE + 5.0f;
    static constexpr int TEST_POSITIONS = 16;
    static constexpr float ANGLE_STEP = 2.0f * float(M_PI) / TEST_POSITIONS;

    float const centerX = ICC_LICH_KING_ASSISTHC_POSITION.GetPositionX();
    float const centerY = ICC_LICH_KING_ASSISTHC_POSITION.GetPositionY();
    float const centerZ = ICC_LICH_KING_ASSISTHC_POSITION.GetPositionZ();

    // Collect shadow traps within search range
    GuidVector const npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    std::vector<Unit*> nearbyTraps;

    for (ObjectGuid const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive() || unit->GetEntry() != NPC_SHADOW_TRAP)
            continue;

        if (bot->GetDistance(unit) < SEARCH_DISTANCE)
            nearbyTraps.push_back(unit);
    }

    if (nearbyTraps.empty())
        return false;

    // Check whether the current position is already safe
    auto isSafePosition = [&](float x, float y) -> bool
    {
        for (Unit const* trap : nearbyTraps)
            if (trap->GetDistance2d(x, y) < SAFE_DISTANCE)
                return false;
        return true;
    };

    if (isSafePosition(bot->GetPositionX(), bot->GetPositionY()))
        return false;

    // Walk clockwise around the centre circle to find the nearest safe spot
    float const currentAngle = std::atan2(bot->GetPositionY() - centerY, bot->GetPositionX() - centerX);

    for (int i = 1; i <= TEST_POSITIONS + 1; ++i)
    {
        // Last iteration tests the diametrically opposite point as a fallback
        float const testAngle = (i <= TEST_POSITIONS) ? currentAngle - ANGLE_STEP * i : currentAngle + float(M_PI);

        float testX = centerX + std::cos(testAngle) * CIRCLE_RADIUS;
        float testY = centerY + std::sin(testAngle) * CIRCLE_RADIUS;
        float testZ = centerZ;

        bot->UpdateAllowedPositionZ(testX, testY, testZ);

        if (!bot->IsWithinLOS(testX, testY, testZ))
            continue;

        if (!isSafePosition(testX, testY))
            continue;

        return MoveTo(bot->GetMapId(), testX, testY, testZ, false, false, false, true,
                      MovementPriority::MOVEMENT_FORCED, true, false);
    }

    return false;
}

bool IccLichKingNecroticPlagueAction::Execute(Event /*event*/)
{
    if (!botAI->HasAura("Necrotic Plague", bot))
        return false;

    static constexpr float DELIVER_RANGE = 3.0f;
    static constexpr std::array<uint32, 4> HorrorEntries = {NPC_SHAMBLING_HORROR1, NPC_SHAMBLING_HORROR2,
                                                            NPC_SHAMBLING_HORROR3, NPC_SHAMBLING_HORROR4};

    GuidVector const& npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    Unit* closestHorror = nullptr;
    float minDist = 150.0f;

    for (ObjectGuid const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
            continue;

        uint32 const entry = unit->GetEntry();
        bool const isHorror = entry == NPC_SHAMBLING_HORROR1 || entry == NPC_SHAMBLING_HORROR2 ||
                              entry == NPC_SHAMBLING_HORROR3 || entry == NPC_SHAMBLING_HORROR4;

        if (!isHorror)
            continue;

        float const dist = bot->GetDistance(unit);
        if (dist < minDist)
        {
            minDist = dist;
            closestHorror = unit;
        }
    }

    // No alive Horror exists — stop moving and hold position so healers
    // can dispel immediately. Keeping the plague active with no valid
    // delivery target risks a raid wipe via uncontrolled spread.
    if (!closestHorror)
    {
        bot->StopMoving();
        bot->AttackStop();
        bot->SetTarget(ObjectGuid::Empty);
        return true;
    }

    // Already in delivery range — hold position for dispel
    if (minDist <= DELIVER_RANGE)
    {
        bot->StopMoving();
        return true;
    }

    // Move toward the Horror, suppressing combat so we don't get sidetracked
    bot->AttackStop();
    bot->SetTarget(ObjectGuid::Empty);

    return MoveTo(bot->GetMapId(), closestHorror->GetPositionX(), closestHorror->GetPositionY(),
                  closestHorror->GetPositionZ(), false, false, false, false, MovementPriority::MOVEMENT_FORCED, true,
                  false);
}

static bool IsIceSphere(uint32 entry)
{
    return entry == NPC_ICE_SPHERE1 || entry == NPC_ICE_SPHERE2 || entry == NPC_ICE_SPHERE3 || entry == NPC_ICE_SPHERE4;
}

static Unit* FindWinterReferenceUnit(Player* bot)
{
    Group* group = bot->GetGroup();
    if (!group)
        return bot;

    Unit* reference = nullptr;
    ObjectGuid lowestGuid;

    for (GroupReference* itr = group->GetFirstMember(); itr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || !member->IsAlive() || !member->IsInWorld())
            continue;

        if (lowestGuid.IsEmpty() || member->GetGUID() < lowestGuid)
        {
            lowestGuid = member->GetGUID();
            reference = member;
        }
    }

    return reference ? reference : bot;
}

static Position const& SelectClosestOf3(Unit* ref, Position const& p1, Position const& p2, Position const& p3)
{
    float const d1 = ref->GetDistance2d(p1.GetPositionX(), p1.GetPositionY());
    float const d2 = ref->GetDistance2d(p2.GetPositionX(), p2.GetPositionY());
    float const d3 = ref->GetDistance2d(p3.GetPositionX(), p3.GetPositionY());

    if (d2 < d1 && d2 < d3)
        return p2;
    if (d3 < d1 && d3 < d2)
        return p3;
    return p1;
}

bool IccLichKingWinterAction::Execute(Event /*event*/)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "the lich king");
    if (!boss)
        return false;

    Difficulty const diff = bot->GetRaidDifficulty();
    if (sPlayerbotAIConfig.EnableICCBuffs && diff &&
        (diff == RAID_DIFFICULTY_10MAN_HEROIC || diff == RAID_DIFFICULTY_25MAN_HEROIC))
    {
        if (!bot->HasAura(SPELL_EXPERIENCED))
            bot->AddAura(SPELL_EXPERIENCED, bot);
        if (!bot->HasAura(SPELL_AGEIS_OF_DALARAN))
            bot->AddAura(SPELL_AGEIS_OF_DALARAN, bot);
        if (!botAI->IsTank(bot) && !bot->HasAura(SPELL_NO_THREAT))
            bot->AddAura(SPELL_NO_THREAT, bot);
        if (!bot->HasAura(SPELL_PAIN_SUPPRESION))
            bot->AddAura(SPELL_PAIN_SUPPRESION, bot);
    }

    // Speed boost to help escape the inward push
    if (bot->GetDistance2d(boss) < 35.0f && !bot->HasAura(SPELL_NITRO_BOOSTS))
        bot->AddAura(SPELL_NITRO_BOOSTS, bot);

    // Winter is not a marking phase — clear skull
    if (Group* group = bot->GetGroup())
    {
        if (!group->GetTargetIcon(7).IsEmpty())
            group->SetTargetIcon(7, bot->GetGUID(), ObjectGuid::Empty);
    }

    // Teleport back onto the platform if the engine pushed us through
    FixPlatformPosition();

    // Defile evacuation has absolute priority over everything else
    if (!IsPositionSafeFromDefile(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), 3.0f))
    {
        Position const& safePos = botAI->IsTank(bot) ? *GetMainTankPosition() : *GetMainTankRangedPosition();

        TryMoveToPosition(safePos.GetPositionX(), safePos.GetPositionY(), PLATFORM_Z, true);
        return true;
    }

    // If an ice sphere is chasing this bot, leave the group so nobody else gets hit
    Unit* iceSphere = AI_VALUE2(Unit*, "find target", "ice sphere");
    if (iceSphere && iceSphere->GetVictim() == bot && !botAI->IsTank(bot))
    {
        MoveFromGroup(6.0f);
        return false;
    }

    // Clear invalid combat targets without stopping movement
    ClearInvalidTarget();
    HandlePetManagement();

    if (botAI->IsTank(bot))
        HandleTankPositioning();
    else if (!botAI->IsRanged(bot))
        HandleMeleePositioning();
    else
        HandleRangedPositioning();

    return false;
}

bool IccLichKingWinterAction::FixPlatformPosition()
{
    if (std::abs(bot->GetPositionZ() - PLATFORM_Z) > 1.0f)
        bot->TeleportTo(bot->GetMapId(), bot->GetPositionX(), bot->GetPositionY(), PLATFORM_Z, bot->GetOrientation());

    return false;
}

bool IccLichKingWinterAction::ClearInvalidTarget()
{
    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    if (!currentTarget)
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "the lich king");
    bool doClear = false;

    // Bots must not fight the boss during Remorseless Winter — they should position
    if (boss && currentTarget == boss)
        doClear = true;

    // Tanks must not chase ice spheres — ranged DPS handle those
    if (botAI->IsTank(bot) && IsIceSphere(currentTarget->GetEntry()))
        doClear = true;

    if (doClear)
    {

        bot->AttackStop();
        bot->SetTarget(ObjectGuid::Empty);
    }

    return false;
}

Position const* IccLichKingWinterAction::GetMainTankPosition()
{
    Unit* mainTank = AI_VALUE(Unit*, "main tank");
    Unit* ref = mainTank ? mainTank : FindWinterReferenceUnit(bot);

    return &SelectClosestOf3(ref, ICC_LK_FROST1_POSITION, ICC_LK_FROST2_POSITION, ICC_LK_FROST3_POSITION);
}

Position const* IccLichKingWinterAction::GetMainTankRangedPosition()
{
    Position const* melee = GetMainTankPosition();

    if (melee == &ICC_LK_FROST1_POSITION)
        return &ICC_LK_FROSTR1_POSITION;
    if (melee == &ICC_LK_FROST2_POSITION)
        return &ICC_LK_FROSTR2_POSITION;
    return &ICC_LK_FROSTR3_POSITION;
}

bool IccLichKingWinterAction::IsPositionSafeFromDefile(float x, float y, float /*z*/, float minSafeDistance) const
{
    static constexpr float BASE_RADIUS = 6.0f;
    static constexpr float SAFETY_MARGIN = 3.0f;

    GuidVector const& npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    for (ObjectGuid const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive() || unit->GetEntry() != DEFILE_NPC_ID)
            continue;

        float currentRadius = BASE_RADIUS;
        for (uint32 const auraId : DEFILE_AURAS)
        {
            Aura* grow = unit->GetAura(auraId);
            if (!grow)
                continue;

            float const growthMultiplier = (bot->GetRaidDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC ||
                                            bot->GetRaidDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
                                               ? 1.4f
                                               : 0.95f;

            currentRadius = BASE_RADIUS + grow->GetStackAmount() * growthMultiplier;
            break;
        }

        float const dx = x - unit->GetPositionX();
        float const dy = y - unit->GetPositionY();

        if (std::sqrt(dx * dx + dy * dy) < currentRadius + SAFETY_MARGIN + minSafeDistance)
            return false;
    }
    return true;
}

bool IccLichKingWinterAction::TryMoveToPosition(float targetX, float targetY, float targetZ, bool forced)
{
    float const dx = targetX - bot->GetPositionX();
    float const dy = targetY - bot->GetPositionY();
    float const dist = std::hypot(dx, dy);

    if (dist < 0.1f)
        return true;

    MovementPriority const priority = forced ? MovementPriority::MOVEMENT_FORCED : MovementPriority::MOVEMENT_COMBAT;

    if (IsPositionSafeFromDefile(targetX, targetY, targetZ, 3.0f) && bot->IsWithinLOS(targetX, targetY, targetZ))
    {
        MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, true, priority, true, false);
        return true;
    }

    static constexpr int STEPS = 8;
    static constexpr float STEP_DIST = 8.0f;

    float const dirX = dx / dist;
    float const dirY = dy / dist;

    for (int i = 1; i <= STEPS; ++i)
    {
        for (int sign : {1, -1})
        {
            float const arc = float(M_PI) / STEPS * i * sign;
            float const cosA = std::cos(arc);
            float const sinA = std::sin(arc);

            float const testX = bot->GetPositionX() + (dirX * cosA - dirY * sinA) * STEP_DIST;
            float const testY = bot->GetPositionY() + (dirX * sinA + dirY * cosA) * STEP_DIST;

            if (!IsPositionSafeFromDefile(testX, testY, targetZ, 3.0f))
                continue;
            if (!bot->IsWithinLOS(testX, testY, targetZ))
                continue;

            MoveTo(bot->GetMapId(), testX, testY, targetZ, false, false, false, true, priority, true, false);
            return false;
        }
    }

    static constexpr float PROBE_STEP = 5.0f;
    static constexpr float MIN_PROBE = PROBE_STEP;  // always make at least one step

    for (float probeLen = dist - PROBE_STEP; probeLen >= MIN_PROBE; probeLen -= PROBE_STEP)
    {
        float probeX = bot->GetPositionX() + dirX * probeLen;
        float probeY = bot->GetPositionY() + dirY * probeLen;
        float probeZ = targetZ;
        bot->UpdateAllowedPositionZ(probeX, probeY, probeZ);

        if (!IsPositionSafeFromDefile(probeX, probeY, probeZ, 3.0f))
            continue;
        if (!bot->IsWithinLOS(probeX, probeY, probeZ))
            continue;

        MoveTo(bot->GetMapId(), probeX, probeY, probeZ, false, false, false, true, priority, true, false);
        return false;
    }

    {
        float nudgeX = bot->GetPositionX() + dirX * 2.0f;
        float nudgeY = bot->GetPositionY() + dirY * 2.0f;
        float nudgeZ = targetZ;
        bot->UpdateAllowedPositionZ(nudgeX, nudgeY, nudgeZ);

        if (bot->IsWithinLOS(nudgeX, nudgeY, nudgeZ))
        {
            MoveTo(bot->GetMapId(), nudgeX, nudgeY, nudgeZ, false, false, false, true,
                   MovementPriority::MOVEMENT_FORCED, true, false);
            return false;
        }
    }

    MoveTo(bot->GetMapId(), targetX, targetY, targetZ, false, false, false, true, MovementPriority::MOVEMENT_FORCED,
           true, false);
    return false;
}

bool IccLichKingWinterAction::IsValidCollectibleAdd(Unit* unit) const
{
    if (!unit || !unit->IsAlive())
        return false;

    uint32 const entry = unit->GetEntry();
    return entry == NPC_SHAMBLING_HORROR1 || entry == NPC_SHAMBLING_HORROR2 || entry == NPC_SHAMBLING_HORROR3 ||
           entry == NPC_SHAMBLING_HORROR4 || entry == NPC_RAGING_SPIRIT1 || entry == NPC_RAGING_SPIRIT2 ||
           entry == NPC_RAGING_SPIRIT3 || entry == NPC_RAGING_SPIRIT4 || entry == NPC_DRUDGE_GHOUL1 ||
           entry == NPC_DRUDGE_GHOUL2 || entry == NPC_DRUDGE_GHOUL3 || entry == NPC_DRUDGE_GHOUL4;
}

static bool HasFrontalAbility(uint32 entry)
{
    return entry == NPC_SHAMBLING_HORROR1 || entry == NPC_SHAMBLING_HORROR2 || entry == NPC_SHAMBLING_HORROR3 ||
           entry == NPC_SHAMBLING_HORROR4 || entry == NPC_RAGING_SPIRIT1 || entry == NPC_RAGING_SPIRIT2 ||
           entry == NPC_RAGING_SPIRIT3 || entry == NPC_RAGING_SPIRIT4;
}

bool IccLichKingWinterAction::HandleTankPositioning()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "the lich king");
    if (!boss)
        return false;

    // Both tanks converge on the same frost position chosen by GetMainTankPosition().
    // The frost position is one of ICC_LK_FROST1/2/3 (the closest to the main-tank
    // reference at the time the selection runs).
    Position const& frostPos = *GetMainTankPosition();
    static constexpr float FROST_AT_POS_TOLERANCE = 3.0f;

    if (botAI->IsMainTank(bot))
    {
        float const dist = bot->GetDistance2d(frostPos.GetPositionX(), frostPos.GetPositionY());

        // Step 1 – get to the frost position.
        if (dist > FROST_AT_POS_TOLERANCE)
        {
            TryMoveToPosition(frostPos.GetPositionX(), frostPos.GetPositionY(), PLATFORM_Z, true);
            return false;
        }

        // Step 2 – at position: engage nearby adds; never leave.
        HandleMainTankAddManagement(boss, &frostPos);
    }
    else if (botAI->IsAssistTank(bot))
    {
        // Step 1 is handled inside HandleAssistTankAddManagement.
        HandleAssistTankAddManagement(boss, &frostPos);
    }

    return false;
}

bool IccLichKingWinterAction::HandleMeleePositioning()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "the lich king");

    // Step 1 – ensure we are near the frost position area.
    Position const& tankPos = *GetMainTankPosition();
    float const distToPos = bot->GetDistance2d(tankPos.GetPositionX(), tankPos.GetPositionY());
    if (distToPos > 8.0f)
    {
        bool const latePhase = boss && !boss->HealthAbovePct(50);
        float const offsetX = latePhase ? 0.0f : -5.0f;
        float const offsetY = latePhase ? 0.0f : 5.0f;
        TryMoveToPosition(tankPos.GetPositionX() + offsetX, tankPos.GetPositionY() + offsetY, PLATFORM_Z, false);
    }

    // Step 2 – position behind the current melee target.
    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    if (!currentTarget || !currentTarget->IsAlive())
        return false;
    if (!IsValidCollectibleAdd(currentTarget) || IsIceSphere(currentTarget->GetEntry()))
        return false;

    // PRIMARY: mainTank→add vector gives us the reliable "facing" direction of
    // the add, since it should have aggro on the main tank at the frost position.
    Unit* mainTank = AI_VALUE(Unit*, "main tank");
    if (mainTank && mainTank->IsAlive())
    {
        // Vector from the main tank to the add.
        float const vecX = currentTarget->GetPositionX() - mainTank->GetPositionX();
        float const vecY = currentTarget->GetPositionY() - mainTank->GetPositionY();
        float const vecLen = std::hypot(vecX, vecY);

        if (vecLen > 0.1f)
        {
            float const dirX = vecX / vecLen;
            float const dirY = vecY / vecLen;

            // Try directly behind (0°) then slight left/right arcs (±20°, ±40°).
            static constexpr std::array<float, 5> Arcs = {0.0f, 0.35f, -0.35f, 0.70f, -0.70f};

            for (float const arc : Arcs)
            {
                float const cosA = std::cos(arc);
                float const sinA = std::sin(arc);
                float const rotX = dirX * cosA - dirY * sinA;
                float const rotY = dirX * sinA + dirY * cosA;

                float const destX = currentTarget->GetPositionX() + rotX * BEHIND_DISTANCE;
                float const destY = currentTarget->GetPositionY() + rotY * BEHIND_DISTANCE;

                float const dx = destX - bot->GetPositionX();
                float const dy = destY - bot->GetPositionY();
                float const dist = std::hypot(dx, dy);

                if (dist < 1.0f)
                    return false;  // already in position

                if (!IsPositionSafeFromDefile(destX, destY, bot->GetPositionZ(), 2.0f))
                    continue;
                if (!bot->IsWithinLOS(destX, destY, bot->GetPositionZ()))
                    continue;

                float const step = std::min(2.0f, dist);
                TryMoveToPosition(bot->GetPositionX() + (dx / dist) * step, bot->GetPositionY() + (dy / dist) * step,
                                  bot->GetPositionZ(), false);
                return false;
            }
        }
    }

    // FALLBACK: stored orientation-based angles.
    static constexpr std::array<float, 6> BehindAngles = {
        float(M_PI),           // 180° — directly behind
        float(M_PI) * 0.75f,   // 135° — left-rear flank
        float(M_PI) * 1.25f,   // 225° — right-rear flank
        float(M_PI) * 0.5f,    // 90°  — left flank
        float(M_PI) * 1.5f,    // 270° — right flank
        float(M_PI) * 0.875f,  // 157° — closer left-rear
    };

    for (float const angleOffset : BehindAngles)
    {
        float const angle = currentTarget->GetOrientation() + angleOffset;
        float const destX = currentTarget->GetPositionX() + std::cos(angle) * BEHIND_DISTANCE;
        float const destY = currentTarget->GetPositionY() + std::sin(angle) * BEHIND_DISTANCE;

        float const dx = destX - bot->GetPositionX();
        float const dy = destY - bot->GetPositionY();
        float const dist = std::hypot(dx, dy);

        if (dist < 1.0f)
            return false;
        if (!IsPositionSafeFromDefile(destX, destY, bot->GetPositionZ(), 2.0f))
            continue;
        if (!bot->IsWithinLOS(destX, destY, bot->GetPositionZ()))
            continue;

        float const step = std::min(2.0f, dist);
        TryMoveToPosition(bot->GetPositionX() + (dx / dist) * step, bot->GetPositionY() + (dy / dist) * step,
                          bot->GetPositionZ(), false);
        return false;
    }

    return false;
}

bool IccLichKingWinterAction::HandleRangedPositioning()
{
    Position const& targetPos = *GetMainTankRangedPosition();

    // Evacuate defile first.
    if (!IsPositionSafeFromDefile(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), 3.0f))
    {
        TryMoveToPosition(targetPos.GetPositionX(), targetPos.GetPositionY(), PLATFORM_Z, true);
        return false;
    }

    // Move to ranged frost position if not already there.
    if (bot->GetDistance2d(targetPos.GetPositionX(), targetPos.GetPositionY()) > 2.0f)
        TryMoveToPosition(targetPos.GetPositionX(), targetPos.GetPositionY(), PLATFORM_Z, false);

    if (!botAI->IsRangedDps(bot))
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    // Determine sphere slot count by difficulty.
    Difficulty const diff = bot->GetRaidDifficulty();
    bool const is25Man = (diff == RAID_DIFFICULTY_25MAN_NORMAL || diff == RAID_DIFFICULTY_25MAN_HEROIC);
    int const sphereSlots = is25Man ? 4 : 2;

    // Collect alive ranged DPS, split into hunters and others.
    std::vector<Player*> hunters;
    std::vector<Player*> otherRanged;

    for (GroupReference* itr = group->GetFirstMember(); itr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || !member->IsAlive() || !member->IsInWorld())
            continue;
        if (!botAI->IsRangedDps(member))
            continue;
        if (member->getClass() == CLASS_HUNTER)
            hunters.push_back(member);
        else
            otherRanged.push_back(member);
    }

    std::sort(hunters.begin(), hunters.end(), [](Player* a, Player* b) { return a->GetGUID() < b->GetGUID(); });
    std::sort(otherRanged.begin(), otherRanged.end(), [](Player* a, Player* b) { return a->GetGUID() < b->GetGUID(); });

    // Fill sphere slots: hunters first, then lowest-GUID non-hunters.
    std::vector<Player*> sphereAssignees;
    for (Player* p : hunters)
    {
        if (static_cast<int>(sphereAssignees.size()) >= sphereSlots)
            break;
        sphereAssignees.push_back(p);
    }
    for (Player* p : otherRanged)
    {
        if (static_cast<int>(sphereAssignees.size()) >= sphereSlots)
            break;
        sphereAssignees.push_back(p);
    }

    bool const isSphereAssignee =
        std::find(sphereAssignees.begin(), sphereAssignees.end(), bot) != sphereAssignees.end();

    if (!isSphereAssignee)
    {
        // Not assigned to spheres — drop any sphere target; let normal combat AI
        // pick up spirits / shambling horrors.
        Unit* cur = bot->GetVictim();
        if (cur && IsIceSphere(cur->GetEntry()))
        {
            bot->AttackStop();
            bot->SetTarget(ObjectGuid::Empty);
        }
        return false;
    }

    // Sphere assignee: target the lowest-HP sphere (most urgent to kill).

    // Keep current sphere target if still valid.
    Unit* cur = bot->GetVictim();
    if (cur && cur->IsAlive() && IsIceSphere(cur->GetEntry()))
    {
        bot->SetFacingToObject(cur);
        Attack(cur);
        return false;
    }

    // Scan for the most urgent sphere.
    GuidVector const& npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    Unit* bestSphere = nullptr;
    float lowestHpPct = 101.0f;

    for (ObjectGuid const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive() || !IsIceSphere(unit->GetEntry()))
            continue;

        float const hp = unit->GetHealthPct();
        if (hp < lowestHpPct)
        {
            lowestHpPct = hp;
            bestSphere = unit;
        }
    }

    if (bestSphere)
    {
        bot->SetTarget(bestSphere->GetGUID());
        bot->SetFacingToObject(bestSphere);
        Attack(bestSphere);
    }

    return false;
}

static bool CastSingleTargetTaunt(PlayerbotAI* botAI, Player* bot, Unit* target)
{
    if (!target || !target->IsAlive())
        return false;

    if (botAI->CastSpell("taunt", target))
        return true;

    switch (bot->getClass())
    {
        case CLASS_PALADIN:
            if (botAI->CastSpell("hand of reckoning", target))
                return true;
            break;
        case CLASS_DEATH_KNIGHT:
            if (botAI->CastSpell("dark command", target))
                return true;
            break;
        case CLASS_DRUID:
            if (botAI->CastSpell("growl", target))
                return true;
            break;
        default:
            break;
    }

    // Ranged poke – generates threat without moving
    if (botAI->CastSpell("shoot", target) || botAI->CastSpell("throw", target))
        return true;

    return false;
}

// Returns true if a spell was cast.
static bool CastAoeTaunt(PlayerbotAI* botAI, Player* bot)
{
    switch (bot->getClass())
    {
        case CLASS_WARRIOR:
            if (botAI->CastSpell("challenging shout", bot))
                return true;
            break;
        case CLASS_DRUID:
            if (botAI->CastSpell("challenging roar", bot))
                return true;
            break;
        default:
            break;
    }
    return false;
}

// HandleMainTankAddManagement
bool IccLichKingWinterAction::HandleMainTankAddManagement(Unit* boss, Position const* frostPos)
{
    static constexpr float ENGAGE_RADIUS = 12.0f;  // engage adds within this range
    static constexpr float TAUNT_RADIUS = 30.0f;   // taunt range (all taunts ≥ 25 yd)
    static constexpr int AOE_TAUNT_MIN = 2;        // fire AoE taunt when ≥ this many adds nearby

    bool hasAliveAssistTank = false;
    if (Group* group = bot->GetGroup())
    {
        for (GroupReference* itr = group->GetFirstMember(); itr; itr = itr->next())
        {
            Player* member = itr->GetSource();
            if (member && member->IsAlive() && member != bot && botAI->IsAssistTank(member))
            {
                hasAliveAssistTank = true;
                break;
            }
        }
    }

    GuidVector const& targets = AI_VALUE(GuidVector, "possible targets");

    // Bucket adds visible from the frost position
    Unit* priorityAdd = nullptr;   // attacking non-tank
    Unit* secondaryAdd = nullptr;  // not yet on MT
    Unit* fallbackAdd = nullptr;   // already on MT
    int nearbyCount = 0;           // for AoE taunt threshold

    for (ObjectGuid const& guid : targets)
    {
        Unit* add = botAI->GetUnit(guid);
        if (!IsValidCollectibleAdd(add))
            continue;

        if (!bot)
            return false;

        float addDist = 0.0f;
        addDist = bot->GetDistance(add);
        float const maxEngage = hasAliveAssistTank ? ENGAGE_RADIUS : 25.0f;

        if (addDist <= TAUNT_RADIUS)
            ++nearbyCount;

        Unit* victim = add->GetVictim();

        // Taunt pass: all adds in taunt range that are NOT already on MT
        bool onMT = victim && victim->IsPlayer() && botAI->IsMainTank(victim->ToPlayer());
        if (!onMT && addDist <= TAUNT_RADIUS)
            CastSingleTargetTaunt(botAI, bot, add);

        // AoE taunt
        if (nearbyCount >= AOE_TAUNT_MIN)
            CastAoeTaunt(botAI, bot);

        // Engagement bucketing 
        if (addDist > maxEngage)
            continue;

        if (victim && victim->IsPlayer() && !botAI->IsTank(victim->ToPlayer()))
        {
            if (!priorityAdd || addDist < bot->GetDistance(priorityAdd))
                priorityAdd = add;
        }
        else if (victim != bot)
        {
            if (!secondaryAdd || addDist < bot->GetDistance(secondaryAdd))
                secondaryAdd = add;
        }
        else
        {
            if (!fallbackAdd || addDist < bot->GetDistance(fallbackAdd))
                fallbackAdd = add;
        }
    }

    Unit* targetAdd = priorityAdd ? priorityAdd : secondaryAdd ? secondaryAdd : fallbackAdd;

    if (!targetAdd)
    {
        Unit* cur = bot->GetVictim();
        if (cur && !IsValidCollectibleAdd(cur))
            bot->SetTarget(ObjectGuid::Empty);

        return false;
    }

    float addDist = 0.0f;
    if (bot)
        addDist = bot->GetDistance(targetAdd);

    if (addDist <= ENGAGE_RADIUS || !hasAliveAssistTank)
    {
        // Pull toward frostPos if solo-tanking and add is far
        if (addDist > ENGAGE_RADIUS && !hasAliveAssistTank)
        {
            float const dx = targetAdd->GetPositionX() - frostPos->GetPositionX();
            float const dy = targetAdd->GetPositionY() - frostPos->GetPositionY();
            float const len = std::hypot(dx, dy);
            float const pullRatio = std::min(1.0f, 15.0f / (len > 0.1f ? len : 0.1f));
            TryMoveToPosition(frostPos->GetPositionX() + dx * pullRatio, frostPos->GetPositionY() + dy * pullRatio,
                              PLATFORM_Z, false);
        }

        bot->SetTarget(targetAdd->GetGUID());
        bot->SetFacingToObject(targetAdd);
        Attack(targetAdd);
    }
    else
    {
        // Add is still being herded by assist tank – face it and generate threat
        bot->SetTarget(targetAdd->GetGUID());
        bot->SetFacingToObject(targetAdd);
        Attack(targetAdd);
    }

    return false;
}

// HandleAssistTankAddManagement 
bool IccLichKingWinterAction::HandleAssistTankAddManagement(Unit* boss, Position const* frostPos)
{
    static constexpr float FROST_TOL = 3.0f;      // "at frost position" tolerance
    static constexpr float MELE_RANGE = 5.0f;    // engage in melee
    static constexpr float TAUNT_RADIUS = 30.0f;  // single-target taunt range

    GuidVector const& targets = AI_VALUE(GuidVector, "possible targets");

    // Identify the current main tank so we never steal its adds
    Unit* mainTank = AI_VALUE(Unit*, "main tank");

    std::vector<Unit*> addsOnUs;   // adds attacking the assist tank
    std::vector<Unit*> addsLoose;  // adds not controlled by main tank

    for (ObjectGuid const& guid : targets)
    {
        Unit* add = botAI->GetUnit(guid);
        if (!IsValidCollectibleAdd(add))
            continue;

        Unit* victim = add->GetVictim();

        // Skip adds already securely held by the main tank
        bool onMainTank = mainTank && victim && victim->GetGUID() == mainTank->GetGUID();
        if (onMainTank)
            continue;

        if (victim == bot)
            addsOnUs.push_back(add);
        else
            addsLoose.push_back(add);
    }

    // Phase 3 – herding: adds are on us, walk back to frost position
    if (!addsOnUs.empty())
    {
        // Taunt all adds that are following us to maintain aggro during the walk
        for (Unit* add : addsOnUs)
            CastSingleTargetTaunt(botAI, bot, add);

        // AoE taunt if ≥2 adds in tow
        if (addsOnUs.size() >= 2)
            CastAoeTaunt(botAI, bot);

        float const distToFrost = bot->GetExactDist2d(frostPos->GetPositionX(), frostPos->GetPositionY());

        if (distToFrost > FROST_TOL)
        {
            TryMoveToPosition(frostPos->GetPositionX(), frostPos->GetPositionY(), PLATFORM_Z, false);
        }

        // Maintain combat target on the way back
        Unit* currentTarget = bot->GetVictim();
        bool keepCurrent = false;
        if (currentTarget && currentTarget->IsAlive())
        {
            for (Unit* add : addsOnUs)
            {
                if (add->GetGUID() == currentTarget->GetGUID())
                {
                    keepCurrent = true;
                    break;
                }
            }
        }

        if (!keepCurrent)
        {
            currentTarget = nullptr;
            for (Unit* add : addsOnUs)
            {
                if (IsLkShambling(add->GetEntry()))
                {
                    currentTarget = add;
                    break;
                }
            }
            if (!currentTarget)
                currentTarget = addsOnUs.front();
        }

        if (currentTarget)
        {
            bot->SetTarget(currentTarget->GetGUID());
            bot->SetFacingToObject(currentTarget);
            Attack(currentTarget);
        }
        return false;
    }

    // Phase 1 – return to frost position if we drifted 
    float const distToFrost = bot->GetExactDist2d(frostPos->GetPositionX(), frostPos->GetPositionY());

    if (distToFrost > FROST_TOL)
    {
        TryMoveToPosition(frostPos->GetPositionX(), frostPos->GetPositionY(), PLATFORM_Z, false);

        Unit* cur = bot->GetVictim();
        if (cur && !IsValidCollectibleAdd(cur))
            bot->SetTarget(ObjectGuid::Empty);
        return false;
    }

    // Phase 2 – at frost position, collect the nearest loose add 
    Unit* targetAdd = nullptr;
    float closestDist = FLT_MAX;

    // Priority 1: rescue add attacking a non-tank
    for (Unit* add : addsLoose)
    {
        Unit* victim = add->GetVictim();
        if (victim && victim->IsPlayer() && !botAI->IsTank(victim->ToPlayer()))
        {
            float const dist = bot->GetDistance(add);
            if (dist < closestDist)
            {
                closestDist = dist;
                targetAdd = add;
            }
        }
    }

    // Priority 2: any loose add
    if (!targetAdd)
    {
        for (Unit* add : addsLoose)
        {
            float const dist = bot->GetDistance(add);
            if (dist < closestDist)
            {
                closestDist = dist;
                targetAdd = add;
            }
        }
    }

    if (!targetAdd)
    {
        // Nothing to collect – stand pat and clear invalid target
        Unit* cur = bot->GetVictim();
        if (cur && !IsValidCollectibleAdd(cur))
            bot->SetTarget(ObjectGuid::Empty);
        return false;
    }

    // Taunt the target add regardless of distance 
    // If the taunt lands the add walks toward us; if it misses/resists we move.
    CastSingleTargetTaunt(botAI, bot, targetAdd);

    // Also taunt other loose adds in range so they come along for free
    for (Unit* add : addsLoose)
    {
        if (add == targetAdd)
            continue;
        float const dist = bot->GetExactDist2d(add);
        if (dist <= TAUNT_RADIUS)
            CastSingleTargetTaunt(botAI, bot, add);
    }

    // AoE taunt if ≥2 loose adds are close enough
    {
        int nearbyLoose = 0;
        for (Unit* add : addsLoose)
            if (bot->GetExactDist2d(add) <= TAUNT_RADIUS)
                ++nearbyLoose;
        if (nearbyLoose >= 2)
            CastAoeTaunt(botAI, bot);
    }

    // Move toward the add only if it is still too far to engage
    bot->SetTarget(targetAdd->GetGUID());

    if (closestDist > MELE_RANGE)
    {
        // Step toward add – add should be walking toward us from the taunt,
        // so we will meet in the middle most of the time.
        TryMoveToPosition(targetAdd->GetPositionX(), targetAdd->GetPositionY(), PLATFORM_Z, false);
    }
    else
    {
        bot->SetFacingToObject(targetAdd);
        Attack(targetAdd);
    }

    return false;
}

bool IccLichKingWinterAction::HandlePetManagement()
{
    Pet* pet = bot->GetPet();
    if (!pet || !pet->IsAlive())
        return false;

    CharmInfo* ci = pet->GetCharmInfo();
    if (!ci)
        return false;

    // Healers: passive follow 
    if (botAI->IsHeal(bot))
    {
        // Only issue the command when the pet is not already following.
        if (ci->GetCommandState() != COMMAND_FOLLOW)
        {
            pet->AttackStop();
            pet->InterruptNonMeleeSpells(false);
            pet->GetMotionMaster()->MoveFollow(bot, PET_FOLLOW_DIST, pet->GetFollowAngle());
            ci->SetCommandState(COMMAND_FOLLOW);
            ci->SetIsCommandAttack(false);
            ci->SetIsAtStay(false);
            ci->SetIsReturning(true);
            ci->SetIsCommandFollow(true);
            ci->SetIsFollowing(false);
            ci->RemoveStayPosition();
        }
        return false;
    }

    GuidVector const& npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    Unit* bestTarget = nullptr;
    int bestPriority = -1;
    float bestHpPct = 101.0f;

    for (ObjectGuid const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
            continue;

        uint32 const entry = unit->GetEntry();
        int priority = -1;

        if (entry == NPC_SHAMBLING_HORROR1 || entry == NPC_SHAMBLING_HORROR2 || entry == NPC_SHAMBLING_HORROR3 ||
            entry == NPC_SHAMBLING_HORROR4)
            priority = 3;
        else if (entry == NPC_RAGING_SPIRIT1 || entry == NPC_RAGING_SPIRIT2 || entry == NPC_RAGING_SPIRIT3 ||
                 entry == NPC_RAGING_SPIRIT4)
            priority = 2;
        else if (IsIceSphere(entry))
            priority = 1;

        if (priority < 0)
            continue;

        float const hp = unit->GetHealthPct();

        if (priority > bestPriority || (priority == bestPriority && hp < bestHpPct))
        {
            bestPriority = priority;
            bestHpPct = hp;
            bestTarget = unit;
        }
    }

    // No valid add target exists yet (or all are dead).
    if (!bestTarget)
        return false;

    // Skip if the pet is already attacking the right unit.
    if (pet->GetVictim() == bestTarget)
        return false;

    // Command the pet to attack.
    ci->SetIsCommandAttack(true);
    ci->SetIsAtStay(false);
    ci->SetIsReturning(false);
    ci->SetIsCommandFollow(false);
    ci->SetIsFollowing(false);
    ci->SetCommandState(COMMAND_ATTACK);

    if (pet->IsAIEnabled)
    {
        pet->AI()->AttackStart(bestTarget);
        pet->GetMotionMaster()->MoveChase(bestTarget);
    }

    return false;
}

bool IccLichKingAddsAction::Execute(Event /*event*/)
{
    // Being carried by a Val'kyr — no actions possible
    if (bot->HasAura(SPELL_HARVEST_SOUL_VALKYR))
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "the lich king");
    Difficulty const diff = bot->GetRaidDifficulty();
    bool const hasPlague = botAI->HasAura("Necrotic Plague", bot);
    Unit* const terenas = bot->FindNearestCreature(NPC_TERENAS_MENETHIL_HC, 55.0f);

    // Heroic cheat buffs 
    if (sPlayerbotAIConfig.EnableICCBuffs && diff && IsHeroicLk(diff))
    {
        if (!bot->HasAura(SPELL_EXPERIENCED))
            bot->AddAura(SPELL_EXPERIENCED, bot);
        if (!bot->HasAura(SPELL_AGEIS_OF_DALARAN))
            bot->AddAura(SPELL_AGEIS_OF_DALARAN, bot);
        if (boss && boss->HealthBelowPct(60) && boss->HealthAbovePct(40) && !bot->HasAura(SPELL_EMPOWERED_BLOOD))
            bot->AddAura(SPELL_EMPOWERED_BLOOD, bot);
        if (!botAI->IsTank(bot) && !bot->HasAura(SPELL_NO_THREAT))
            bot->AddAura(SPELL_NO_THREAT, bot);
        if (!bot->HasAura(SPELL_PAIN_SUPPRESION))
            bot->AddAura(SPELL_PAIN_SUPPRESION, bot);
    }

    // Phase 1: mark boss with skull
    if (Group* group = bot->GetGroup())
    {
        if (boss && boss->HealthAbovePct(71) && group->GetTargetIcon(7) != boss->GetGUID())
            group->SetTargetIcon(7, bot->GetGUID(), boss->GetGUID());
    }

    // Val'kyr edge-dive (ongoing) 
    if (bot->HasAura(30440))
    {
        if (bot->GetPositionZ() > 779.0f)
            return JumpTo(bot->GetMapId(), bot->GetPositionX(), bot->GetPositionY(), 740.01f);

        bot->Kill(bot, bot);
        return true;
    }

    // Phase 2: detect bots fallen off edge and initiate dive
    auto const hasAnyWinterAura = [&]() -> bool
    {
        if (!boss)
            return false;
        return boss->HasAura(SPELL_REMORSELESS_WINTER1) || boss->HasAura(SPELL_REMORSELESS_WINTER2) ||
               boss->HasAura(SPELL_REMORSELESS_WINTER3) || boss->HasAura(SPELL_REMORSELESS_WINTER4) ||
               boss->HasAura(SPELL_REMORSELESS_WINTER5) || boss->HasAura(SPELL_REMORSELESS_WINTER6) ||
               boss->HasAura(SPELL_REMORSELESS_WINTER7) || boss->HasAura(SPELL_REMORSELESS_WINTER8);
    };

    if (boss && boss->GetHealthPct() < 70.0f && boss->GetHealthPct() > 40.0f && !hasAnyWinterAura())
    {
        float const dx = bot->GetPositionX() - 503.0f;
        float const dy = bot->GetPositionY() - (-2124.0f);
        float const platDist = std::hypot(dx, dy);

        if (platDist > 52.0f && platDist < 70.0f && bot->GetPositionZ() > 844.0f)
        {
            bot->AddAura(30440, bot);
            return JumpTo(bot->GetMapId(), bot->GetPositionX(), bot->GetPositionY(), 740.01f);
        }
    }

    // Teleport fixes
    HandleTeleportationFixes(diff, terenas);

    // Heroic: Terenas / spirit phase 
    if (HandleSpiritBombAvoidance(diff, terenas))
        return true;

    HandleHeroicNonTankPositioning(diff, terenas);
    HandleSpiritMarkingAndTargeting(diff, terenas);

    if (terenas)
        return false;

    // Normal encounter flow
    if (HandleQuakeMechanics(boss))
        return true;

    HandleShamblingHorrors(boss, hasPlague);
    HandleAssistTankAddManagement(boss, diff);
    HandleMeleePositioning(boss, hasPlague, diff);
    HandleMainTankTargeting(boss, diff);
    HandleNonTankHeroicPositioning(boss, diff, hasPlague);
    HandleRangedPositioning(boss, hasPlague, diff);
    HandleDefileMechanics(boss, diff);
    HandleValkyrMechanics(diff);
    HandleVileSpiritMechanics();

    return false;
}

bool IccLichKingAddsAction::HandleTeleportationFixes(Difficulty diff, Unit* terenas)
{
    // Normal mode: snap back if somehow teleported far outside the encounter area
    if (!IsHeroicLk(diff) && std::abs(bot->GetPositionY() - (-2095.7915f)) > 200.0f)
    {
        bot->TeleportTo(bot->GetMapId(), ICC_LICH_KING_ADDS_POSITION.GetPositionX(),
                        ICC_LICH_KING_ADDS_POSITION.GetPositionY(), ICC_LICH_KING_ADDS_POSITION.GetPositionZ(),
                        bot->GetOrientation());
        return false;
    }

    // Fix bots going underground (buggy ice-platform collisions)
    if (!botAI->GetAura("Harvest Soul", bot, false, false) && !botAI->GetAura("Harvest Souls", bot, false, false) &&
        std::abs(bot->GetPositionZ() - 840.857f) > 1.0f)
    {
        bot->TeleportTo(bot->GetMapId(), bot->GetPositionX(), bot->GetPositionY(), 840.857f, bot->GetOrientation());
        return false;
    }

    // Heroic: keep bot near Terenas during Harvest Soul
    if (terenas && botAI->GetAura("Harvest Soul", bot, false, false) &&
        std::abs(bot->GetPositionZ() - 1049.865f) > 5.0f)
    {
        bot->TeleportTo(bot->GetMapId(), terenas->GetPositionX(), terenas->GetPositionY(), 1049.865f,
                        bot->GetOrientation());
    }

    return false;
}

bool IccLichKingAddsAction::HandleSpiritBombAvoidance(Difficulty diff, Unit* terenas)
{
    if (!botAI->IsMainTank(bot) || !terenas || !diff || !IsHeroicLk(diff))
        return false;

    static constexpr float SAFE_DIST = 14.0f;
    static constexpr float SAFE_HEIGHT = 12.0f;
    static constexpr float DENSITY_RADIUS = 10.0f;
    static constexpr float DENSITY_PENALTY = 10.0f;
    static constexpr float MAX_HEIGHT_DIFF = 8.0f;

    GuidVector const& npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    std::vector<Unit*> bombs;
    for (ObjectGuid const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && unit->GetEntry() == NPC_SPIRIT_BOMB)
            bombs.push_back(unit);
    }

    if (bombs.empty())
        return false;

    // Check current position safety
    bool currentlySafe = true;
    for (Unit const* bomb : bombs)
    {
        float const hDist =
            std::hypot(bot->GetPositionX() - bomb->GetPositionX(), bot->GetPositionY() - bomb->GetPositionY());
        float const vDist = std::abs(bot->GetPositionZ() - bomb->GetPositionZ());

        if (hDist < SAFE_DIST && vDist <= SAFE_HEIGHT)
        {
            currentlySafe = false;
            break;
        }
    }

    if (currentlySafe)
        return true;

    // Search for the best nearby safe position
    static constexpr std::array<float, 5> SearchDistances = {6.0f, 10.0f, 15.0f, 20.0f, 25.0f};
    static constexpr int ANGLE_COUNT = 36;

    float bestScore = -std::numeric_limits<float>::max();
    float bestX = 0.0f;
    float bestY = 0.0f;
    float bestZ = 0.0f;
    bool found = false;

    for (float const radius : SearchDistances)
    {
        for (int i = 0; i < ANGLE_COUNT; ++i)
        {
            float const angle = i * 2.0f * float(M_PI) / ANGLE_COUNT;
            float const testX = bot->GetPositionX() + radius * std::cos(angle);
            float const testY = bot->GetPositionY() + radius * std::sin(angle);
            float testZ = bot->GetPositionZ();

            bot->UpdateAllowedPositionZ(testX, testY, testZ);

            if (std::abs(testZ - bot->GetPositionZ()) >= MAX_HEIGHT_DIFF)
                continue;
            if (!bot->IsWithinLOS(testX, testY, testZ))
                continue;

            bool posSafe = true;
            float minHDist = std::numeric_limits<float>::max();
            int nearbyCount = 0;

            for (Unit const* bomb : bombs)
            {
                float const hDist = std::hypot(testX - bomb->GetPositionX(), testY - bomb->GetPositionY());
                float const vDist = std::abs(testZ - bomb->GetPositionZ());

                minHDist = std::min(minHDist, hDist);

                if (hDist < DENSITY_RADIUS)
                    ++nearbyCount;

                if (hDist < SAFE_DIST && vDist <= SAFE_HEIGHT)
                {
                    posSafe = false;
                    break;
                }
            }

            if (!posSafe)
                continue;

            float const distBonus = std::max(0.0f, 30.0f - radius);
            float const score = minHDist - nearbyCount * DENSITY_PENALTY + distBonus;

            if (score > bestScore)
            {
                bestScore = score;
                bestX = testX;
                bestY = testY;
                bestZ = testZ;
                found = true;
            }
        }

        if (found && radius <= 15.0f)
            break;
    }

    if (found && bot->IsWithinLOS(bestX, bestY, bestZ) && std::abs(bestZ - bot->GetPositionZ()) <= MAX_HEIGHT_DIFF)
    {
        MoveTo(bot->GetMapId(), bestX, bestY, bestZ, false, false, false, true, MovementPriority::MOVEMENT_FORCED);
    }

    return true;
}

bool IccLichKingAddsAction::HandleHeroicNonTankPositioning(Difficulty diff, Unit* terenas)
{
    if (!terenas || botAI->IsMainTank(bot) || !diff || !IsHeroicLk(diff))
        return false;

    Unit* mainTank = AI_VALUE(Unit*, "main tank");
    if (!mainTank)
        return false;

    // Stack on main tank — 3-yard threshold avoids micro-jitter
    if (bot->GetExactDist2d(mainTank) > 3.0f)
        MoveTo(bot->GetMapId(), mainTank->GetPositionX(), mainTank->GetPositionY(), mainTank->GetPositionZ(), false,
               false, false, true, MovementPriority::MOVEMENT_FORCED);

    return false;
}

bool IccLichKingAddsAction::HandleSpiritMarkingAndTargeting(Difficulty diff, Unit* terenas)
{
    if (!terenas || botAI->IsMainTank(bot) || !diff || !IsHeroicLk(diff))
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    static constexpr uint8_t STAR_ICON = 0;
    static constexpr float MAX_Z_DIFF = 20.0f;

    auto const spiritTargetsGroup = [&](Unit* spirit) -> bool
    {
        Unit* victim = spirit->GetVictim();
        if (!victim || !victim->IsPlayer())
            return false;

        Group* g = victim->ToPlayer()->GetGroup();
        return g && g->GetGUID() == group->GetGUID();
    };

    Unit* currentMark = botAI->GetUnit(group->GetTargetIcon(STAR_ICON));
    bool needNewMark = !currentMark || !currentMark->IsAlive();
    bool markOnTarget = currentMark && currentMark->IsAlive() && spiritTargetsGroup(currentMark);

    if (needNewMark || !markOnTarget)
    {
        GuidVector const& npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

        Unit* prioritySpirit = nullptr;
        Unit* nearestSpirit = nullptr;
        float priorityDist = 100.0f;
        float nearestDist = 100.0f;

        for (ObjectGuid const& guid : npcs)
        {
            Unit* unit = botAI->GetUnit(guid);
            if (!unit || !unit->IsAlive() || !unit->isTargetableForAttack())
                continue;

            uint32 const entry = unit->GetEntry();
            if (entry != NPC_WICKED_SPIRIT1 && entry != NPC_WICKED_SPIRIT2 && entry != NPC_WICKED_SPIRIT3 &&
                entry != NPC_WICKED_SPIRIT4)
                continue;

            if (std::abs(unit->GetPositionZ() - bot->GetPositionZ()) > MAX_Z_DIFF)
                continue;

            float const dist = bot->GetDistance(unit);
            if (spiritTargetsGroup(unit) && dist < priorityDist)
            {
                prioritySpirit = unit;
                priorityDist = dist;
            }
            if (dist < nearestDist)
            {
                nearestSpirit = unit;
                nearestDist = dist;
            }
        }

        Unit* toMark = prioritySpirit ? prioritySpirit : nearestSpirit;
        if (toMark && (needNewMark || (prioritySpirit && !markOnTarget)))
            group->SetTargetIcon(STAR_ICON, bot->GetGUID(), toMark->GetGUID());
    }

    // Ranged DPS focus the star target; instant-kill is a temporary workaround
    // while bots can't reliably kill spirits before they detonate
    if (botAI->IsRangedDps(bot))
    {
        context->GetValue<std::string>("rti")->Set("star");

        Unit* starTarget = botAI->GetUnit(group->GetTargetIcon(STAR_ICON));
        if (starTarget && starTarget->IsAlive())
        {
            bot->SetTarget(starTarget->GetGUID());
            bot->SetFacingToObject(starTarget);
            Attack(starTarget);
            bot->Kill(bot, starTarget);
        }
    }

    return false;
}

bool IccLichKingAddsAction::HandleQuakeMechanics(Unit* boss)
{
    if (!boss || !boss->HasUnitState(UNIT_STATE_CASTING) || !boss->FindCurrentSpellBySpellId(SPELL_QUAKE))
        return false;

    static constexpr float QUAKE_MIN = 35.0f;
    static constexpr float QUAKE_MAX = 45.0f;
    static constexpr float QUAKE_TARGET = 40.0f;

    float const dist = bot->GetExactDist2d(boss);
    if (dist >= QUAKE_MIN && dist <= QUAKE_MAX)
        return false;

    // Interrupt active spell cast before repositioning
    if (bot->HasUnitState(UNIT_STATE_CASTING))
        bot->InterruptNonMeleeSpells(false);

    float const dx = bot->GetPositionX() - boss->GetPositionX();
    float const dy = bot->GetPositionY() - boss->GetPositionY();
    float const len = std::hypot(dx, dy);
    if (len < 0.01f)
        return false;

    // Move along the bot→boss axis to reach 40-yard safe band
    float const targetX = boss->GetPositionX() + (dx / len) * QUAKE_TARGET;
    float const targetY = boss->GetPositionY() + (dy / len) * QUAKE_TARGET;

    MoveTo(bot->GetMapId(), targetX, targetY, bot->GetPositionZ(), false, false, false, false,
           MovementPriority::MOVEMENT_COMBAT);
    return false;
}

bool IccLichKingAddsAction::HandleShamblingHorrors(Unit* /*boss*/, bool /*hasPlague*/)
{
    if (bot->getClass() != CLASS_HUNTER)
        return false;

    GuidVector const& npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (ObjectGuid const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive() || !IsLkShambling(unit->GetEntry()))
            continue;

        if (botAI->HasAura("Enrage", unit))
        {
            botAI->CastSpell("Tranquilizing Shot", unit);
            return false;
        }
    }

    return false;
}

bool IccLichKingAddsAction::HandleAssistTankAddManagement(Unit* boss, Difficulty diff)
{
    if (!botAI->IsAssistTank(bot) || !boss || boss->HealthBelowPct(71))
        return false;

    Position const& holdPos = IsHeroicLk(diff) ? ICC_LICH_KING_ASSISTHC_POSITION : ICC_LICH_KING_ADDS_POSITION;

    // Helper: class-specific single-target taunt
    // Returns true if any threat-generating ability was cast.
    // Priority: class taunt > generic "taunt" > ranged attack.
    auto CastClassTaunt = [&](Unit* target) -> bool
    {
        if (!target || !target->IsAlive())
            return false;
        
        // Explicit class-specific single-target taunts
        switch (bot->getClass())
        {
            case CLASS_PALADIN:
                if (botAI->CastSpell("hand of reckoning", target))
                    return true;
                break;
            case CLASS_DEATH_KNIGHT:
                if (botAI->CastSpell("dark command", target))
                    return true;
                break;
            case CLASS_DRUID:
                if (botAI->CastSpell("growl", target))
                    return true;
                break;
            case CLASS_WARRIOR:
                if (botAI->CastSpell("taunt", target))
                    return true;
                break;
            default:
                break;
        }

        // Last resort: ranged poke to generate threat without moving
        if (botAI->CastSpell("shoot", target) || botAI->CastSpell("throw", target))
            return true;

        return false;
    };

    // Categorise visible adds 
    GuidVector const& targets = AI_VALUE(GuidVector, "possible targets");

    std::vector<Unit*> addsOnUs;
    std::vector<Unit*> addsElsewhere;

    for (ObjectGuid const& guid : targets)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!IsValidLkCollectibleAdd(unit))
            continue;

        if (unit->GetVictim() == bot)
            addsOnUs.push_back(unit);
        else
            addsElsewhere.push_back(unit);
    }

    // Step 1: return to hold position if we have drifted
    float const distToHold = bot->GetExactDist2d(holdPos);
    if (distToHold > 5.0f)
    {
        MoveTo(bot->GetMapId(), holdPos.GetPositionX(), holdPos.GetPositionY(), holdPos.GetPositionZ(), false, false,
               false, true, MovementPriority::MOVEMENT_FORCED, true, false);
    }

    // Step 2: taunt every loose add in range (do NOT move toward them)
    // Shamblings get priority so their Enrage does not wipe the raid.
    Unit* priorityLoose = nullptr;

    for (Unit* add : addsElsewhere)
    {
        float const dist = bot->GetExactDist2d(add);

        if (IsLkShambling(add->GetEntry()) && !priorityLoose)
            priorityLoose = add;

        // Taunt if within 30 yd (standard taunt range).
        if (dist <= 30.0f)
            CastClassTaunt(add);
    }

    // Step 3: emergency rescue – add already at melee range on a non-tank
    if (priorityLoose == nullptr)
    {
        for (Unit* add : addsElsewhere)
        {
            Unit* victim = add->GetVictim();
            bool rescueNeeded = victim && victim->IsPlayer() && !botAI->IsTank(victim->ToPlayer());

            if (rescueNeeded && bot->GetExactDist2d(add) <= 5.0f)
            {
                bot->SetTarget(add->GetGUID());
                bot->SetFacingToObject(add);
                Attack(add);
                return false;
            }
        }
    }

    // Step 4: fight adds that are already on us (at the hold spot)
    if (addsOnUs.empty())
        return false;

    // Stable target selection: keep current target if still valid.
    Unit* currentTarget = bot->GetVictim();
    bool keepCurrent = false;

    if (currentTarget && currentTarget->IsAlive())
    {
        for (Unit* add : addsOnUs)
        {
            if (add->GetGUID() == currentTarget->GetGUID())
            {
                keepCurrent = true;
                break;
            }
        }
    }

    if (!keepCurrent)
    {
        currentTarget = nullptr;
        // Shambling Horror first, then any other add.
        for (Unit* add : addsOnUs)
        {
            if (IsLkShambling(add->GetEntry()))
            {
                currentTarget = add;
                break;
            }
        }
        if (!currentTarget && !addsOnUs.empty())
            currentTarget = addsOnUs.front();
    }

    if (currentTarget)
    {
        bot->SetTarget(currentTarget->GetGUID());
        bot->SetFacingToObject(currentTarget);
        Attack(currentTarget);
    }

    return false;
}

bool IccLichKingAddsAction::HandleMeleePositioning(Unit* boss, bool hasPlague, Difficulty diff)
{
    if (!boss || !botAI->IsMelee(bot) || botAI->IsAssistTank(bot) || boss->HealthBelowPct(71) || hasPlague ||
        IsHeroicLk(diff))
        return false;

    float const distToPos = bot->GetDistance(ICC_LICH_KING_MELEE_POSITION);
    if (distToPos <= 6.0f)
        return false;

    if (!botAI->IsMainTank(bot))
    {
        MoveTo(bot->GetMapId(), ICC_LICH_KING_MELEE_POSITION.GetPositionX(),
               ICC_LICH_KING_MELEE_POSITION.GetPositionY(), ICC_LICH_KING_MELEE_POSITION.GetPositionZ(), false, false,
               false, true, MovementPriority::MOVEMENT_FORCED, true, false);
        return false;
    }

    // Main tank: step toward position in 3-yard increments to avoid overshooting
    if (boss->GetVictim() != bot)
        return false;

    float const dx = ICC_LICH_KING_MELEE_POSITION.GetPositionX() - bot->GetPositionX();
    float const dy = ICC_LICH_KING_MELEE_POSITION.GetPositionY() - bot->GetPositionY();
    float const len = std::hypot(dx, dy);
    if (len < 0.1f)
        return false;

    float const step = std::min(3.0f, len - 1.0f);
    if (step <= 0.0f)
    {
        MoveTo(bot->GetMapId(), ICC_LICH_KING_MELEE_POSITION.GetPositionX(),
               ICC_LICH_KING_MELEE_POSITION.GetPositionY(), bot->GetPositionZ(), false, false, false, true,
               MovementPriority::MOVEMENT_FORCED, true, false);
        return false;
    }

    MoveTo(bot->GetMapId(), bot->GetPositionX() + (dx / len) * step, bot->GetPositionY() + (dy / len) * step,
           bot->GetPositionZ(), false, false, false, true, MovementPriority::MOVEMENT_FORCED, true, false);

    return false;
}

bool IccLichKingAddsAction::HandleMainTankTargeting(Unit* boss, Difficulty diff)
{
    if (!botAI->IsMainTank(bot) || !boss || !IsHeroicLk(diff))
        return false;

    if (boss->HealthBelowPct(71) || boss->GetVictim() == bot)
        return false;

    bot->SetTarget(boss->GetGUID());
    bot->SetFacingToObject(boss);
    Attack(boss);

    return false;
}

bool IccLichKingAddsAction::HandleNonTankHeroicPositioning(Unit* boss, Difficulty diff, bool hasPlague)
{
    if (botAI->IsTank(bot) || !boss || !IsHeroicLk(diff))
        return false;

    if (boss->HealthBelowPct(71) || hasPlague)
        return false;

    Unit* mainTank = AI_VALUE(Unit*, "main tank");
    if (!mainTank)
        return false;

    float const distToTank = bot->GetDistance2d(mainTank);

    if (bot->getClass() == CLASS_HUNTER)
    {
        // Hunters maintain a longer leash — move in 2-yard steps to abool jitter
        if (distToTank > 20.0f)
        {
            float const dx = mainTank->GetPositionX() - bot->GetPositionX();
            float const dy = mainTank->GetPositionY() - bot->GetPositionY();
            float const len = std::hypot(dx, dy);
            if (len > 0.01f)
            {
                MoveTo(bot->GetMapId(), bot->GetPositionX() + (dx / len) * 2.0f,
                       bot->GetPositionY() + (dy / len) * 2.0f, bot->GetPositionZ(), false, false, false, true,
                       MovementPriority::MOVEMENT_FORCED, true, false);
            }
        }
    }
    else
    {
        // Everyone else stacks on main tank; 3-yard threshold avoids micro-jitter
        if (distToTank > 3.0f)
            MoveTo(bot->GetMapId(), mainTank->GetPositionX(), mainTank->GetPositionY(), bot->GetPositionZ(), false,
                   false, false, true, MovementPriority::MOVEMENT_FORCED, true, false);
    }

    return false;
}

bool IccLichKingAddsAction::HandleRangedPositioning(Unit* boss, bool hasPlague, Difficulty diff)
{
    if (!boss || !botAI->IsRanged(bot) || boss->HealthBelowPct(71) || hasPlague || IsHeroicLk(diff))
        return false;

    if (bot->GetDistance(ICC_LICH_KING_RANGED_POSITION) > 2.0f)
        MoveTo(bot->GetMapId(), ICC_LICH_KING_RANGED_POSITION.GetPositionX(),
               ICC_LICH_KING_RANGED_POSITION.GetPositionY(), ICC_LICH_KING_RANGED_POSITION.GetPositionZ(), false, false,
               false, true, MovementPriority::MOVEMENT_FORCED, true, false);

    return false;
}

bool IccLichKingAddsAction::HandleDefileMechanics(Unit* boss, Difficulty diff)
{
    if (!boss)
        return false;

    static constexpr float SAFETY_MARGIN = 3.0f;
    static constexpr float MOVE_DISTANCE = 5.0f;
    static constexpr float SPREAD_DISTANCE = 12.0f;
    static constexpr float FIXED_Z = 840.857f;
    static constexpr float MAX_HEIGHT_DIFF = 5.0f;
    static constexpr float MIN_SPACING = 5.0f;
    static constexpr float MAX_BOSS_DIST = 40.0f;
    static constexpr int ANGLE_TESTS = 16;
    static constexpr int MAX_OFFSETS = 8;

    GuidVector const& npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    std::vector<Unit*> defiles;
    for (ObjectGuid const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (unit && unit->IsAlive() && unit->GetEntry() == DEFILE_NPC_ID)
            defiles.push_back(unit);
    }

    if (!defiles.empty())
    {
        float const botX = bot->GetPositionX();
        float const botY = bot->GetPositionY();

        bool needToMove = false;
        for (Unit const* defile : defiles)
        {
            float const radius = GetDefileEffectiveRadius(defile, diff);
            float const dist = std::hypot(botX - defile->GetPositionX(), botY - defile->GetPositionY());
            if (dist < radius + SAFETY_MARGIN)
            {
                needToMove = true;
                break;
            }
        }

        if (needToMove)
        {
            float bestScore = 0.0f;
            float bestAngle = 0.0f;
            bool found = false;

            for (int i = 0; i < ANGLE_TESTS; ++i)
            {
                float const angle = i * float(M_PI) / 8.0f;
                float const testX = botX + MOVE_DISTANCE * std::cos(angle);
                float const testY = botY + MOVE_DISTANCE * std::sin(angle);
                float testZ = FIXED_Z;

                bot->UpdateAllowedPositionZ(testX, testY, testZ);

                if (!bot->IsWithinLOS(testX, testY, testZ) || std::abs(testZ - bot->GetPositionZ()) >= MAX_HEIGHT_DIFF)
                    continue;

                float minDefileDist = std::numeric_limits<float>::max();
                for (Unit const* defile : defiles)
                {
                    float const d = std::hypot(testX - defile->GetPositionX(), testY - defile->GetPositionY());
                    minDefileDist = std::min(minDefileDist, d);
                }

                float const bossProximity = 100.0f - std::min(100.0f, boss->GetDistance2d(testX, testY));
                float const score = minDefileDist + bossProximity * 0.5f;

                if (score > bestScore)
                {
                    bestScore = score;
                    bestAngle = angle;
                    found = true;
                }
            }

            if (found)
            {
                float moveX = botX + MOVE_DISTANCE * std::cos(bestAngle);
                float moveY = botY + MOVE_DISTANCE * std::sin(bestAngle);
                float moveZ = FIXED_Z;

                // Interrupt spell cast rather than calling botAI->Reset() (avoids freeze)
                if (bot->HasUnitState(UNIT_STATE_CASTING))
                    bot->InterruptNonMeleeSpells(false);

                bot->UpdateAllowedPositionZ(moveX, moveY, moveZ);
                MoveTo(bot->GetMapId(), moveX, moveY, moveZ, false, false, false, true,
                       MovementPriority::MOVEMENT_FORCED);
            }
        }
    }

    if (!boss->HasUnitState(UNIT_STATE_CASTING) || !boss->FindCurrentSpellBySpellId(DEFILE_CAST_ID))
        return false;

    Map::PlayerList const& players = bot->GetMap()->GetPlayers();
    uint32 playerCount = 0;
    uint32 botIndex = 0;
    uint32 idx = 0;

    float const botX = bot->GetPositionX();
    float const botY = bot->GetPositionY();

    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        Player* player = itr->GetSource();
        if (!player || !player->IsAlive())
            continue;

        if (player == bot)
            botIndex = idx;

        ++idx;
        ++playerCount;
    }

    if (playerCount == 0)
        return false;

    float const preferredAngle = (float(botIndex) / float(playerCount)) * 2.0f * float(M_PI);
    float bestAngle = preferredAngle;
    bool foundSpot = false;

    for (int offset = 0; offset <= MAX_OFFSETS && !foundSpot; ++offset)
    {
        for (int dir : {-1, 1})
        {
            if (offset == 0 && dir > 0)
                continue;

            float const testAngle = preferredAngle + dir * offset * float(M_PI) / 16.0f;
            float const testX = botX + SPREAD_DISTANCE * std::cos(testAngle);
            float const testY = botY + SPREAD_DISTANCE * std::sin(testAngle);
            float testZ = FIXED_Z;

            bot->UpdateAllowedPositionZ(testX, testY, testZ);

            if (!bot->IsWithinLOS(testX, testY, testZ) || std::abs(testZ - bot->GetPositionZ()) >= MAX_HEIGHT_DIFF ||
                boss->GetDistance2d(testX, testY) > MAX_BOSS_DIST)
                continue;

            // Check defile safety at spread position
            bool safeFromDefiles = true;
            for (Unit const* defile : defiles)
            {
                float const radius = GetDefileEffectiveRadius(defile, diff);
                float const dist = std::hypot(testX - defile->GetPositionX(), testY - defile->GetPositionY());
                if (dist < radius + SAFETY_MARGIN)
                {
                    safeFromDefiles = false;
                    break;
                }
            }
            if (!safeFromDefiles)
                continue;

            // Maintain spacing from other players
            bool tooClose = false;
            for (Map::PlayerList::const_iterator pItr = players.begin(); pItr != players.end(); ++pItr)
            {
                Player* player = pItr->GetSource();
                if (!player || !player->IsAlive() || player == bot)
                    continue;

                if (std::hypot(testX - player->GetPositionX(), testY - player->GetPositionY()) < MIN_SPACING)
                {
                    tooClose = true;
                    break;
                }
            }
            if (tooClose)
                continue;

            bestAngle = testAngle;
            foundSpot = true;
            break;
        }
    }

    if (foundSpot)
    {
        float spreadX = botX + SPREAD_DISTANCE * std::cos(bestAngle);
        float spreadY = botY + SPREAD_DISTANCE * std::sin(bestAngle);
        float spreadZ = FIXED_Z;

        bot->UpdateAllowedPositionZ(spreadX, spreadY, spreadZ);
        MoveTo(bot->GetMapId(), spreadX, spreadY, spreadZ, false, false, false, false,
               MovementPriority::MOVEMENT_COMBAT);
    }

    return false;
}

bool IccLichKingAddsAction::HandleValkyrMechanics(Difficulty diff)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "the lich king");
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    GuidVector const& npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    std::vector<Unit*> grabbingValkyrs;
    for (ObjectGuid const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive() || !IsLkValkyr(unit))
            continue;

        // Heroic: only target Val'kyrs above 49% health (they wont pickup anyone anymore and are 2 tanky to kill, better to focus LK
        bool const isGrabbing =
            unit->HasAura(SPELL_HARVEST_SOUL_VALKYR) && (!IsHeroicLk(diff) || unit->HealthAbovePct(49));

        if (isGrabbing)
            grabbingValkyrs.push_back(unit);
    }

    // No active Val'kyrs — mark the boss during the window between grabs
    if (grabbingValkyrs.empty() || (boss && boss->HealthBelowPct(43)))
    {
        for (ObjectGuid const& guid : npcs)
        {
            Unit* unit = botAI->GetUnit(guid);
            if (!unit || !unit->IsAlive() || unit->GetEntry() != NPC_THE_LICH_KING)
                continue;
            if (!unit->HealthBelowPct(68) || unit->HealthBelowPct(40))
                continue;

            if (group->GetTargetIcon(7) != unit->GetGUID())
                group->SetTargetIcon(7, bot->GetGUID(), unit->GetGUID());
            break;
        }
        return false;
    }

    if (botAI->IsMainTank(bot))
        return false;

    HandleValkyrMarking(grabbingValkyrs, diff);
    HandleValkyrAssignment(grabbingValkyrs);

    return false;
}

bool IccLichKingAddsAction::HandleValkyrMarking(std::vector<Unit*> const& grabbingValkyrs, Difficulty diff)
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    std::vector<Unit*> sorted = grabbingValkyrs;
    std::sort(sorted.begin(), sorted.end(), [](Unit* a, Unit* b) { return a->GetGUID() < b->GetGUID(); });

    static constexpr std::array<uint8_t, 3> Icons = {7, 6, 0};  // Skull, Cross, Star

    // Heroic: clear stale markers for Val'kyrs no longer grabbing or at wrong Z
    if (IsHeroicLk(diff))
    {
        for (uint8_t const iconIdx : Icons)
        {
            Unit* marked = botAI->GetUnit(group->GetTargetIcon(iconIdx));
            if (!marked || !IsLkValkyr(marked))
                continue;

            bool const stale = !marked->HasAura(SPELL_HARVEST_SOUL_VALKYR) ||
                               std::abs(marked->GetPositionZ() - bot->GetPositionZ()) > 5.0f;
            if (stale)
                group->SetTargetIcon(iconIdx, bot->GetGUID(), ObjectGuid::Empty);
        }
    }

    // Clear icon slots beyond the current Val'kyr count
    for (size_t i = sorted.size(); i < Icons.size(); ++i)
    {
        if (!group->GetTargetIcon(Icons[i]).IsEmpty())
            group->SetTargetIcon(Icons[i], bot->GetGUID(), ObjectGuid::Empty);
    }

    // Assign an icon to each active Val'kyr
    for (size_t i = 0; i < sorted.size() && i < Icons.size(); ++i)
    {
        Unit* marked = botAI->GetUnit(group->GetTargetIcon(Icons[i]));
        if (!marked || marked != sorted[i])
            group->SetTargetIcon(Icons[i], bot->GetGUID(), sorted[i]->GetGUID());
    }

    return false;
}

bool IccLichKingAddsAction::HandleValkyrAssignment(std::vector<Unit*> const& grabbingValkyrs)
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "the lich king");
    if (boss && boss->HealthBelowPct(40))
        return false;

    std::vector<Unit*> valid;
    for (Unit* valkyr : grabbingValkyrs)
    {
        if (valkyr && valkyr->IsAlive() && valkyr->HasAura(SPELL_HARVEST_SOUL_VALKYR))
            valid.push_back(valkyr);
    }
    if (valid.empty())
        return false;

    std::sort(valid.begin(), valid.end(), [](Unit* a, Unit* b) { return a->GetGUID() < b->GetGUID(); });

    // Build sorted list of non-main-tank members for deterministic assignment
    std::vector<Player*> assistMembers;
    for (GroupReference* itr = group->GetFirstMember(); itr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (member && !botAI->IsMainTank(member))
            assistMembers.push_back(member);
    }
    if (assistMembers.empty())
        return false;

    std::sort(assistMembers.begin(), assistMembers.end(),
              [](Player* a, Player* b) { return a->GetGUID() < b->GetGUID(); });

    auto const it = std::find(assistMembers.begin(), assistMembers.end(), bot);
    if (it == assistMembers.end())
        return false;

    size_t const myIndex = std::distance(assistMembers.begin(), it);
    auto const groupSizes = CalculateBalancedGroupSizes(assistMembers.size(), valid.size());
    size_t const valkyrIndex = GetAssignedValkyrIndex(myIndex, groupSizes);

    if (valkyrIndex >= valid.size())
        return false;

    Unit* myValkyr = valid[valkyrIndex];
    context->GetValue<std::string>("rti")->Set(GetRTIValueForValkyr(valkyrIndex));

    bot->SetTarget(myValkyr->GetGUID());
    bot->SetFacingToObject(myValkyr);

    Difficulty const diff = bot->GetRaidDifficulty();
    if (sPlayerbotAIConfig.EnableICCBuffs && diff && IsHeroicLk(diff) && !myValkyr->HasAura(SPELL_HAMMER_OF_JUSTICE))
        bot->AddAura(SPELL_HAMMER_OF_JUSTICE, myValkyr);

    ApplyCCToValkyr(myValkyr);

    return false;
}

bool IccLichKingAddsAction::HandleVileSpiritMechanics()
{
    static constexpr float SAFE_RADIUS = 12.0f;

    GuidVector const& npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (ObjectGuid const& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!IsLkVileSpirit(unit))
            continue;

        if (!unit->GetVictim() || unit->GetVictim()->GetGUID() != bot->GetGUID())
            continue;

        float const dist = bot->GetDistance2d(unit);
        if (dist < SAFE_RADIUS)
            MoveAway(unit, SAFE_RADIUS - dist);
    }

    return false;
}

bool IccLichKingAddsAction::IsValkyr(Unit* unit) { return IsLkValkyr(unit); }

std::vector<size_t> IccLichKingAddsAction::CalculateBalancedGroupSizes(size_t totalAssist, size_t numValkyrs)
{
    std::vector<size_t> groupSizes(numValkyrs, 0);
    if (numValkyrs == 0)
        return groupSizes;

    size_t const baseSize = totalAssist / numValkyrs;
    size_t const remainder = totalAssist % numValkyrs;

    for (size_t i = 0; i < numValkyrs; ++i)
    {
        groupSizes[i] = baseSize;
        if (i < remainder)
            ++groupSizes[i];
    }
    return groupSizes;
}

size_t IccLichKingAddsAction::GetAssignedValkyrIndex(size_t assistIndex, std::vector<size_t> const& groupSizes)
{
    size_t cursor = 0;
    for (size_t valkyrIndex = 0; valkyrIndex < groupSizes.size(); ++valkyrIndex)
    {
        if (assistIndex < cursor + groupSizes[valkyrIndex])
            return valkyrIndex;
        cursor += groupSizes[valkyrIndex];
    }
    return 0;  // fallback — should not be reached with correct input
}

std::string IccLichKingAddsAction::GetRTIValueForValkyr(size_t valkyrIndex)
{
    switch (valkyrIndex)
    {
        case 0:
            return "skull";
        case 1:
            return "cross";
        case 2:
            return "star";
        default:
            return "skull";
    }
}

bool IccLichKingAddsAction::ApplyCCToValkyr(Unit* valkyr)
{
    switch (bot->getClass())
    {
        case CLASS_MAGE:
            if (!botAI->HasAura("Frost Nova", valkyr))
                botAI->CastSpell("Frost Nova", valkyr);
            break;
        case CLASS_DRUID:
            if (!botAI->HasAura("Entangling Roots", valkyr))
                botAI->CastSpell("Entangling Roots", valkyr);
            break;
        case CLASS_PALADIN:
            if (!botAI->HasAura("Hammer of Justice", valkyr))
                botAI->CastSpell("Hammer of Justice", valkyr);
            break;
        case CLASS_WARRIOR:
            if (!botAI->HasAura("Hamstring", valkyr))
                botAI->CastSpell("Hamstring", valkyr);
            break;
        case CLASS_HUNTER:
            if (!botAI->HasAura("Concussive Shot", valkyr))
                botAI->CastSpell("Concussive Shot", valkyr);
            break;
        case CLASS_ROGUE:
            if (!botAI->HasAura("Kidney Shot", valkyr))
                botAI->CastSpell("Kidney Shot", valkyr);
            break;
        case CLASS_SHAMAN:
            if (!botAI->HasAura("Frost Shock", valkyr))
                botAI->CastSpell("Frost Shock", valkyr);
            break;
        case CLASS_DEATH_KNIGHT:
            if (!botAI->HasAura("Chains of Ice", valkyr))
                botAI->CastSpell("Chains of Ice", valkyr);
            break;
        case CLASS_PRIEST:
            if (!botAI->HasAura("Psychic Scream", valkyr))
                botAI->CastSpell("Psychic Scream", valkyr);
            break;
        case CLASS_WARLOCK:
            if (!botAI->HasAura("Fear", valkyr))
                botAI->CastSpell("Fear", valkyr);
            break;
        default:
            break;
    }

    return false;
}
