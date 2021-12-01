#include "RangedUnitStrategy.hpp"

#include "../EntityDetector.hpp"

RangedUnitStrategy::RangedUnitStrategy(const EntityIndex unitIndex,
                                       const PlayerView& playerView,
                                       const EntityManager& entityManager,
                                       const FocusAttackManager& focusAttackManager,
                                       const SquadManager& squadManager,
                                       AttackActionObserversList&& attackObservers)
    : _unitIndex{unitIndex}
    , _playerView{playerView}
    , _entityManager{entityManager}
    , _focusAttackManager{focusAttackManager}
    , _squadManager{squadManager}
    , _attackObservers{std::move(attackObservers)}
{
    const EntityDetector entityDetector{_playerView, _entityManager};
    const auto unit = _playerView.entities.at(_unitIndex);
    const auto enemyID = _playerView.players.at(0).id != _playerView.myId ? _playerView.players.at(0).id
                                                                          : _playerView.players.at(1).id;
    
    auto rangedEnemies = entityDetector.getOnDistanse(unit.position, 0, 7, enemyID, {RANGED_UNIT});
    auto meleeEnemies = entityDetector.getOnDistanse(unit.position, 0, 2, enemyID, {MELEE_UNIT});

    std::vector<EntityIndex> enemies;

    enemies.insert(enemies.end(),
                   std::make_move_iterator(rangedEnemies.begin()),
                   std::make_move_iterator(rangedEnemies.end()));
    
    enemies.insert(enemies.end(),
                   std::make_move_iterator(meleeEnemies.begin()),
                   std::make_move_iterator(meleeEnemies.end()));
    
    auto allies = entityDetector.getOnDistanse(unit.position, 0, 3, _playerView.myId, {RANGED_UNIT, MELEE_UNIT});
    
    // Add allies, that already attack enemy.
    for (const auto& enemyIndex : enemies)
    {
        auto atackingAllies = _focusAttackManager.getEnemyFocusAllies(enemyIndex);
        allies.insert(std::make_move_iterator(atackingAllies.begin()), std::make_move_iterator(atackingAllies.end()));
    }
    
    _isInDanger = allies.size() < enemies.size();
}

std::unique_ptr<AttackAction> RangedUnitStrategy::generateAttackAction() const
{
    if (_isInDanger)
    {
        return nullptr;
    }
    
    const auto target = _focusAttackManager.calculateEnemyToAttack(_playerView.entities.at(_unitIndex));
    if (!target)
    {
        return nullptr;
    }
    
    notifyAttackActionGenerated(*target);
    
    return std::make_unique<AttackAction>(std::make_unique<int>(*target), nullptr);
}

std::unique_ptr<MoveAction> RangedUnitStrategy::generateMoveAction() const
{
    const auto& unit = _playerView.entities.at(_unitIndex);
    const auto enemyID = _playerView.players.at(0).id != _playerView.myId ? _playerView.players.at(0).id
                                                                          : _playerView.players.at(1).id;
    
    if (_isInDanger && !_squadManager.isPusher(unit.id))
    {
        const auto& baseIndixes = _entityManager.getEntities({_playerView.myId, BUILDER_BASE});
        if (!baseIndixes.empty())
        {
            const auto base = _playerView.entities[baseIndixes[0]];
            return std::make_unique<MoveAction>(base.position, true, false);
        }
    }
 
    if (_squadManager.isPusher(unit.id))
    {
        const auto enemyBuilderBases = _entityManager.getEntities({{enemyID, BUILDER_BASE},
                                                                   {enemyID, RANGED_BASE},
                                                                   {enemyID, MELEE_BASE}});
        
        auto target = enemyBuilderBases.empty() ? Vec2Int{_playerView.mapSize - 15, _playerView.mapSize - 15}
                                                : _playerView.entities[enemyBuilderBases[0]].position;
        
        return std::make_unique<MoveAction>(std::move(target), true, true);
    }
    
    
    
    if (_squadManager.isLeftDefender(unit.id) || _squadManager.isRightDefender(unit.id))
    {
        EntityDetector entityDetector{_playerView, _entityManager};
        
        const auto offset = 30;
        const MapRange leftSide{0, 0, offset, _playerView.mapSize - offset};
        const MapRange rightSide{0, 0, _playerView.mapSize - offset, offset};
        const auto range = _squadManager.isLeftDefender(unit.id) ? leftSide : rightSide;
        
        const auto enemies = entityDetector.getInRange(range, enemyID, {MELEE_UNIT, RANGED_UNIT});
        
        if (enemies.empty())
        {
            return;
        }
        
        Vec2Int closestEnemyPosition{_playerView.mapSize, _playerView.mapSize};
        auto minDistance = SIZE_T_MAX;
        
        for (const auto enemyIndex : enemies)
        {
            const auto& enemy = _playerView.entities[enemyIndex];
            
            const auto actualDistance = enemy.position.x + enemy.position.y;
            
            if (actualDistance < minDistance)
            {
                minDistance = actualDistance;
                closestEnemyPosition = enemy.position;
            }
        }
        
        const auto defenceDistanse = 23;
        if (closestEnemyPosition.x + closestEnemyPosition.y < defenceDistanse)
        {
            return std::make_unique<MoveAction>(closestEnemyPosition, true, true);
        }
        
        const auto target = _squadManager.isLeftDefender(unit.id) ? Vec2Int{closestEnemyPosition.x, defenceDistanse}
                                                                  : Vec2Int{defenceDistanse, closestEnemyPosition.y};
        
        return std::make_unique<MoveAction>(target, true, true);
    }
    
    if (_squadManager.isSaboteur(unit.id))
    {
        const auto target = unit.position.y < (_playerView.mapSize - 3) ? Vec2Int{unit.position.x, unit.position.y + 1}
                                                                        : Vec2Int{unit.position.x + 1, unit.position.y};
            
        return std::make_unique<MoveAction>(std::move(target), true, true);
    }
    
    const auto defenceDistance = 60;
    const auto units = _entityManager.getEntities({_playerView.myId,RANGED_UNIT});
    for (const auto& unitIndex : units)
    {
        const auto& unit = _playerView.entities.at(unitIndex);
        if (unitIndex == _unitIndex)
        {
            continue;
        }
        
        if (unit.position.x + unit.position.y == defenceDistance)
        {
            return std::make_unique<MoveAction>(unit.position, true, true);
        }
    }
    
    if (unit.position.x + unit.position.y >= defenceDistance)
    {
        return nullptr;
    }
    
    return std::make_unique<MoveAction>(Vec2Int{_playerView.mapSize, _playerView.mapSize}, true, true);
}

std::unique_ptr<BuildAction> RangedUnitStrategy::generateBuildAction() const
{
    return nullptr;
}

std::unique_ptr<RepairAction> RangedUnitStrategy::generateRepairAction() const
{
    return nullptr;
}

void RangedUnitStrategy::notifyAttackActionGenerated(const EntityIndex enemyIndex) const
{
    for (auto observer : _attackObservers)
    {
        observer->onAttackActionGenerated(_unitIndex, enemyIndex);
    }
}
