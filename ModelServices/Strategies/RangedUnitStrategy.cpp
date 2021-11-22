#include "RangedUnitStrategy.hpp"

#include "../EntityDetector.hpp"

RangedUnitStrategy::RangedUnitStrategy(const EntityIndex unitIndex,
                                       const PlayerView& playerView,
                                       const EntityManager& entityManager,
                                       const FocusAttackManager& focusAttackManager,
                                       AttackActionObserversList&& attackObservers)
    : _unitIndex{unitIndex}
    , _playerView{playerView}
    , _entityManager{entityManager}
    , _focusAttackManager{focusAttackManager}
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
    Vec2Int target;
    if (_isInDanger)
    {
        target = {0, 0};
    }
    else
    {
        target = Vec2Int{_playerView.mapSize - 1, _playerView.mapSize - 1};
    }
    
    const auto findClosestPosition = true;
    const auto breakThrough = true;
    
    return std::make_unique<MoveAction>(std::move(target), findClosestPosition, breakThrough);
}

std::unique_ptr<BuildAction> RangedUnitStrategy::generateBuildAction() const
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
