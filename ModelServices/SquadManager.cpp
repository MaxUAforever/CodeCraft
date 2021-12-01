#include "SquadManager.hpp"
    
#include "EntityDetector.hpp"

void SquadManager::update(const PlayerView& playerView, const EntityManager& entityManager)
{
    const auto& rangedEntities = entityManager.getEntities({playerView.myId, RANGED_UNIT});
    
    const auto eraseDeadEntities = [&](const EntityID entityID)
    {
        for (const auto& entityIndex : rangedEntities)
        {
            if (playerView.entities[entityIndex].id == entityID)
            {
                return false;
            }
        }
        
        return true;
    };
    
    std::erase_if(_builders, eraseDeadEntities);
    std::erase_if(_pushers, eraseDeadEntities);
    std::erase_if(_leftDefenders, eraseDeadEntities);
    std::erase_if(_rightDefenders, eraseDeadEntities);
    std::erase_if(_saboteurs, eraseDeadEntities);
    
    updateBuilders(playerView, entityManager);
    updatePushers(playerView, entityManager);
    updateDefenders(playerView, entityManager);
    //updateSaboteurs(playerView, entityManager);
}

void SquadManager::updateBuilders(const PlayerView& playerView, const EntityManager& entityManager)
{
    constexpr auto totalBuildersCount = 3;
    if (_builders.size() >= 3)
    {
        return;
    }
    
    const auto allBuilders = entityManager.getEntities({playerView.myId, BUILDER_UNIT});
    if (allBuilders.size() < 4)
    {
        _builders.clear();
    }
    
    const auto neededUnits = totalBuildersCount - _builders.size();
    
    auto insertedUnits = 0u;
    for (const auto& unitIndex : allBuilders)
    {
        if (insertedUnits == neededUnits)
        {
            break;
        }
        
        const auto unit = playerView.entities[unitIndex];
        
        _builders.insert(unit.id);
        ++insertedUnits;
    }
}

void SquadManager::updatePushers(const PlayerView& playerView, const EntityManager& entityManager)
{
    if (!_pushers.empty())
    {
        return;
    }
    
    const auto searchDistance = 60;
    const auto units = entityManager.getEntities({playerView.myId,RANGED_UNIT});
    for (const auto& unitIndex : units)
    {
        const auto& unit = playerView.entities.at(unitIndex);
        if (unit.position.x + unit.position.y != searchDistance)
        {
            continue;
        }
        
        EntityDetector detector{playerView, entityManager};
        const auto allies = detector.getOnDistanse(unit.position, 0, 6, playerView.myId, {RANGED_UNIT});
        if (allies.size() < 15)
        {
            continue;
        }
        
        for (const auto allyIndex : allies)
        {
            const auto& ally = playerView.entities.at(allyIndex);
            _pushers.insert(ally.id);
        }
        
        break;
    }
}

void SquadManager::updateDefenders(const PlayerView& playerView, const EntityManager& entityManager)
{
    const auto enemyID = playerView.players.at(0).id != playerView.myId ? playerView.players.at(0).id
                                                                        : playerView.players.at(1).id;
    
    const auto totalEnemies = entityManager.getEntities({{enemyID, RANGED_UNIT}, {enemyID, MELEE_UNIT}});

    const auto offset = 25;
    const MapRange leftSide{0, 0, offset, playerView.mapSize - offset};
    const MapRange rightSide{0, 0, playerView.mapSize - offset, offset};
    
    updateSideDefenders(_leftDefenders, playerView, entityManager, leftSide);
    updateSideDefenders(_rightDefenders, playerView, entityManager, rightSide);
}

void SquadManager::updateSideDefenders(std::unordered_set<EntityID>& sideDefenders,
                                       const PlayerView& playerView,
                                       const EntityManager& entityManager,
                                       const MapRange& sideRange)
{
    const auto enemyID = playerView.players.at(0).id != playerView.myId ? playerView.players.at(0).id
                                                                        : playerView.players.at(1).id;
    
    EntityDetector entityDetector{playerView, entityManager};
    
    const auto sideEnemies = entityDetector.getInRange(sideRange, enemyID, {RANGED_UNIT, MELEE_UNIT});
    if (sideEnemies.empty())
    {
        sideDefenders.clear();
        return;
    }
    
    if (sideDefenders.size() > sideEnemies.size() + 2)
    {
        return;
    }
    
    auto allies = entityManager.getEntities({playerView.myId, RANGED_UNIT});
    std::reverse(allies.begin(), allies.end());
    
    const auto neededUnits = sideEnemies.size() - sideDefenders.size() + 2;
    
    auto insertedUnits = 0u;
    for (const auto& unitIndex : allies)
    {
        if (insertedUnits == neededUnits)
        {
            break;
        }
        
        const auto unit = playerView.entities[unitIndex];
        if (_leftDefenders.contains(unit.id) || _rightDefenders.contains(unit.id) || _saboteurs.contains(unit.id))
        {
            continue;
        }
        
        sideDefenders.insert(unit.id);
        ++insertedUnits;
    }
}

void SquadManager::updateSaboteurs(const PlayerView& playerView, const EntityManager& entityManager)
{
    const auto maxSaboutersCount = 3u;
    const auto enemyID = playerView.players.at(0).id != playerView.myId ? playerView.players.at(0).id
                                                                        : playerView.players.at(1).id;
    
    
    if (_saboteurs.size() >= maxSaboutersCount)
    {
        return;
    }
    
    auto allies = entityManager.getEntities({playerView.myId, RANGED_UNIT});
    std::reverse(allies.begin(), allies.end());
    
    const auto enemies = entityManager.getEntities({{enemyID, RANGED_UNIT},
                                                    {enemyID, MELEE_UNIT}});
    
    
    if (allies.size() <= enemies.size() + 3 || allies.size() > enemies.size() + 10)
    {
        return;
    }
    
    const auto freeAlliesCount = allies.size() - enemies.size();
    const auto neededAliiesCount = maxSaboutersCount - _saboteurs.size();
    
    const auto newSaboutersCount = std::min(freeAlliesCount, neededAliiesCount);
    
    auto insertedUnitsCount = 0u;
    for (const auto& unitIndex : allies)
    {
        const auto unit = playerView.entities[unitIndex];
        if (!_leftDefenders.contains(unit.id) && !_rightDefenders.contains(unit.id) && !_saboteurs.contains(unit.id))
        {
            _saboteurs.insert(unit.id);
            ++insertedUnitsCount;
        }
    
        if (insertedUnitsCount == newSaboutersCount)
        {
            break;
        }
    }
}

bool SquadManager::isBuilder(const EntityID entityID) const
{
    return _builders.contains(entityID);
}

bool SquadManager::isPusher(const EntityID entityID) const
{
    return _pushers.contains(entityID);
}

bool SquadManager::isLeftDefender(const EntityID entityID) const
{
    return _leftDefenders.contains(entityID);
}

bool SquadManager::isRightDefender(const EntityID entityID) const
{
    return _rightDefenders.contains(entityID);
}

bool SquadManager::isSaboteur(const EntityID entityID) const
{
    return _saboteurs.contains(entityID);
}

