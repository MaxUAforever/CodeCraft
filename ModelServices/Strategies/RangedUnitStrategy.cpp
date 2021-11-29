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
    if (_isInDanger)
    {
        const auto& baseIndixes = _entityManager.getEntities({_playerView.myId, BUILDER_BASE});
        if (!baseIndixes.empty())
        {
            const auto base = _playerView.entities[baseIndixes[0]];
            return std::make_unique<MoveAction>(base.position, true, false);
        }
    }
 
    const auto enemyID = _playerView.players.at(0).id != _playerView.myId ? _playerView.players.at(0).id
                                                                          : _playerView.players.at(1).id;
    
    const auto& unit = _playerView.entities.at(_unitIndex);
    if (_squadManager.isDefender(unit.id))
    {
        const auto enemies = _entityManager.getEntities({{enemyID, MELEE_UNIT},
                                                         {enemyID, RANGED_UNIT}});
        
        if (enemies.empty())
        {
            return;
        }
        
        const auto baseIndixes = _entityManager.getEntities({_playerView.myId, BUILDER_BASE});
        const auto basePosition =  baseIndixes.empty() ? _playerView.entities[baseIndixes[0]].position
                                                       : Vec2Int{0, 0};
        
        Vec2Int closestEnemyPosition{0, 0};
        auto minDistance = SIZE_T_MAX;
        
        for (const auto enemyIndex : enemies)
        {
            const auto& enemy = _playerView.entities[enemyIndex];
            
            const auto distanceX = std::abs(basePosition.x - enemy.position.x);
            const auto distanceY = std::abs(basePosition.y - enemy.position.y);
            const auto actualDistance = distanceX + distanceY;
            
            if (actualDistance < minDistance)
            {
                minDistance = actualDistance;
                closestEnemyPosition = enemy.position;
            }
        }
        
        return std::make_unique<MoveAction>(closestEnemyPosition, true, true);
    }
    
    if (_squadManager.isSaboteur(unit.id))
    {
        const auto baseIndices = _entityManager.getEntities({_playerView.myId, BUILDER_BASE});
        const auto enemyBaseIndices = _entityManager.getEntities({enemyID, BUILDER_BASE});
        
        if (baseIndices.empty() || enemyBaseIndices.empty())
        {
            return std::make_unique<MoveAction>(Vec2Int{30, 30}, true, true);
        }
        
        const auto basePosition = _playerView.entities[baseIndices[0]].position;
        const auto enemyBasePosition = _playerView.entities[enemyBaseIndices[0]].position;
        
        if (enemyBasePosition.x > basePosition.x)
        {
            const auto target = unit.position.y < (_playerView.mapSize - 3) ? Vec2Int{unit.position.x, unit.position.y + 1}
                                                                            : Vec2Int{unit.position.x + 1, unit.position.y};
            
            return std::make_unique<MoveAction>(std::move(target), true, true);
        }
        else
        {
            const auto target = unit.position.y > 3 ? Vec2Int{unit.position.x, unit.position.y - 1}
                                                    : Vec2Int{unit.position.x - 1, unit.position.y};
            
            return std::make_unique<MoveAction>(std::move(target), true, true);
        }
    }
    
    return std::make_unique<MoveAction>(Vec2Int{30, 30}, true, true);
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
