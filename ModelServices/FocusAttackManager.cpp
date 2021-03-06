#include "FocusAttackManager.hpp"

#include "EntityDetector.hpp"

#include "../model/MapRange.hpp"

namespace
{
size_t getUnitTypeAttackPriority(const EntityType entityType)
{
    static const std::unordered_map<EntityType, size_t> PriorityType{{RANGED_UNIT, 0},
                                                                     {BUILDER_UNIT, 1},
                                                                     {TURRET, 2},
                                                                     {BUILDER_BASE, 3},
                                                                     {RANGED_BASE, 4},
                                                                     {MELEE_BASE, 5},
                                                                     {MELEE_UNIT, 6}};
    
    const auto priorityIt = PriorityType.find(entityType);
    if (priorityIt == PriorityType.cend())
    {
        return PriorityType.size();
    }
    
    return priorityIt->second;
}
} // namespace

FocusAttackManager::FocusAttackManager(const PlayerView& playerView, const EntityManager& entityManager)
    : _playerView{playerView}
{
    const auto rangedUnits = entityManager.getEntities({playerView.myId, RANGED_UNIT});
    
    const EntityDetector entityDetector{playerView, entityManager};
    for (const auto unitIndex : rangedUnits)
    {
        auto enemies = entityDetector.getEnemiesInEntityRange(unitIndex, {BUILDER_UNIT,
                                                                          MELEE_UNIT,
                                                                          RANGED_UNIT,
                                                                          TURRET,
                                                                          BUILDER_BASE,
                                                                          MELEE_BASE,
                                                                          RANGED_BASE});
        
        for (const auto enemyIndex : enemies)
        {
            auto enemyIt = _enemyAttackInfo.find(enemyIndex);
            if (enemyIt == _enemyAttackInfo.end())
            {
                _enemyAttackInfo.emplace(enemyIndex, AttackInfo{{unitIndex}, 0});
            }
            else
            {
                auto& unitAttackInfo = enemyIt->second;
                unitAttackInfo.alliesInEnemyRange.push_back(unitIndex);
            }
        }
        
        const auto& unit = playerView.entities[unitIndex];
        auto& enemiesInEntityRange = _enemiesInEntityRange[unit.id];
        enemiesInEntityRange.insert(enemiesInEntityRange.end(),
                                    std::make_move_iterator(enemies.begin()),
                                    std::make_move_iterator(enemies.end()));
    }
}

std::vector<EntityIndex> FocusAttackManager::getEnemyFocusAllies(const EntityIndex enemyIndex) const
{
    const auto enemyAttackInfoIt = _enemyAttackInfo.find(enemyIndex);
    if (enemyAttackInfoIt == _enemyAttackInfo.cend())
    {
        return {};
    }
    
    return enemyAttackInfoIt->second.alliesInEnemyRange;
}

std::optional<EntityID> FocusAttackManager::calculateEnemyToAttack(const Entity& entity) const
{
    const auto isAlliedUnit = entity.playerId != nullptr && *entity.playerId == _playerView.myId;
    const auto isFightUnit = entity.entityType == RANGED_UNIT || entity.entityType == MELEE_UNIT;
    
    if (!isAlliedUnit || !isFightUnit)
    {
        return std::nullopt;
    }
    
    const auto enemiesIt = _enemiesInEntityRange.find(entity.id);
    if (enemiesIt == _enemiesInEntityRange.cend())
    {
        return std::nullopt;
    }
    
    const auto& properties = _playerView.entityProperties.at(entity.entityType);
    const auto unitDamage = static_cast<size_t>(properties.attack->damage);
    
    const auto& enemies = enemiesIt->second;
    
    std::optional<EntityIndex> resultIndex = std::nullopt;
    std::optional<Entity> resultEntity = std::nullopt;
    
    for (auto enemyIndex : enemies)
    {
        if (enemyIndex >= _playerView.entities.size())
        {
            continue;
        }
        
        const auto& enemy = _playerView.entities[enemyIndex];
        
        if (_enemyAttackInfo.at(enemyIndex).potentialDamage >= enemy.health)
        {
            // If entity is already damaged on full health, it isn't needed to attack it.
            continue;
        }
        
        if (resultEntity == std::nullopt)
        {
            resultIndex = enemyIndex;
            resultEntity = enemy;
            continue;
        }
        
        const auto currentTypePriority = getUnitTypeAttackPriority(resultEntity->entityType);
        const auto newEnemyTypePriority = getUnitTypeAttackPriority(enemy.entityType);
        
        if (currentTypePriority < newEnemyTypePriority)
        {
            continue;
        }
        
        const auto currentTargetPotentialDamage = _enemyAttackInfo.at(*resultIndex).alliesInEnemyRange.size() * unitDamage;
        const auto newTargetPotentialDamage = _enemyAttackInfo.at(enemyIndex).alliesInEnemyRange.size() * unitDamage;
        
        const auto canBeCurrentTargetKilled = currentTargetPotentialDamage >= resultEntity->health;
        const auto canBeNewTargetKilled = currentTargetPotentialDamage >= resultEntity->health;
        
        if (canBeCurrentTargetKilled)
        {
            if (!canBeNewTargetKilled || currentTargetPotentialDamage < newTargetPotentialDamage)
            {
                // If new target can't be killed or we can deal more total damage on it - skip it.
                continue;
            }
        }
        else if (!canBeNewTargetKilled && currentTargetPotentialDamage > newTargetPotentialDamage)
        {
            // If both targets can't be kiled, but we can deal less total damage on new target - skip it.
            continue;
        }
        
        resultIndex = enemyIndex;
        resultEntity = enemy;
    }
    
    if (resultEntity && resultIndex)
    {
        return resultEntity->id;
    }
    
    return std::nullopt;
}

void FocusAttackManager::onAttackActionGenerated(const EntityIndex entityIndex, const EntityIndex enemyIndex)
{
    const auto& entity = _playerView.entities[entityIndex];
    const auto& properties = _playerView.entityProperties.at(entity.entityType);
    const auto unitDamage = static_cast<size_t>(properties.attack->damage);
    
    _enemyAttackInfo[enemyIndex].potentialDamage += unitDamage;
}
