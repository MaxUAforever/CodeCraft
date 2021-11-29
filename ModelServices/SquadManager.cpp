#include "SquadManager.hpp"
    
#include "EntityDetector.hpp"

void SquadManager::update(const PlayerView& playerView,const EntityManager& entityManager)
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
    
    std::erase_if(_defenders, eraseDeadEntities);
    std::erase_if(_saboteurs, eraseDeadEntities);
    
    updateDefenders(playerView, entityManager);
    updateSaboteurs(playerView, entityManager);
}

void SquadManager::updateDefenders(const PlayerView& playerView, const EntityManager& entityManager)
{
    const auto enemyID = playerView.players.at(0).id != playerView.myId ? playerView.players.at(0).id
                                                                        : playerView.players.at(1).id;
    
    const auto enemies = entityManager.getEntities({{enemyID, RANGED_UNIT}, {enemyID, MELEE_UNIT}});
    MapRange baseRange{0, 0, 25, 25};
    
    const auto isInBaseRange = [&](const EntityIndex entityIndex)
    {
        const auto& enemy = playerView.entities[entityIndex];
        return baseRange.contains(enemy.position);
    };
    
    const auto enemiesNearBaseCount = std::count_if(enemies.begin(), enemies.end(), isInBaseRange);
    if (!enemiesNearBaseCount)
    {
        _defenders.clear();
        return;
    }
   
    const auto allies = entityManager.getEntities({playerView.myId, RANGED_UNIT});
    
    if (allies.size() < enemiesNearBaseCount)
    {
        for (const auto& alliesIndex : allies)
        {
            const auto& unit = playerView.entities[alliesIndex];
            _saboteurs.erase(unit.id);
            _defenders.insert(unit.id);
        }
        
        return;
    }
    
    if (_defenders.size() == enemiesNearBaseCount || allies.empty())
    {
        return;
    }
    
    for (auto i = 0; i < allies.size(); ++i)
    {
        const auto unitIndex = allies[allies.size() - 1 - i];
        
        const auto& unit = playerView.entities[unitIndex];
        if (!_defenders.contains(unit.id) && !_saboteurs.contains(unit.id))
        {
            _defenders.insert(unit.id);
        }
        
        if (_defenders.size() == enemiesNearBaseCount)
        {
            break;
        }
    }
}

void SquadManager::updateSaboteurs(const PlayerView& playerView, const EntityManager& entityManager)
{
    const auto maxSaboutersCount = 3u;
    const auto enemyID = playerView.players.at(0).id != playerView.myId ? playerView.players.at(0).id
                                                                        : playerView.players.at(1).id;
    
    
    if (!_defenders.empty() || _saboteurs.size() >= maxSaboutersCount)
    {
        return;
    }
    
    const auto allies = entityManager.getEntities({playerView.myId, RANGED_UNIT});
    
    const auto enemies = entityManager.getEntities({{enemyID, RANGED_UNIT},
                                                    {enemyID, MELEE_UNIT}});
    
    
    if (allies.size() <= enemies.size())
    {
        return;
    }
    
    const auto freeAlliesCount = allies.size() - enemies.size();
    const auto neededAliiesCount = maxSaboutersCount - _saboteurs.size();
    
    const auto newSaboutersCount = std::min(freeAlliesCount, neededAliiesCount);
    
    auto insertedUnitsCount = 0u;
    for (auto i = 0; i < allies.size(); ++i)
    {
        const auto unitIndex = allies[allies.size() - 1 - i];
    
        const auto unit = playerView.entities[unitIndex];
        if (!_defenders.contains(unit.id) && !_saboteurs.contains(unit.id))
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

bool SquadManager::isDefender(const EntityID entityID) const
{
    return _defenders.contains(entityID);
}

bool SquadManager::isSaboteur(const EntityID entityID) const
{
    return _saboteurs.contains(entityID);
}

