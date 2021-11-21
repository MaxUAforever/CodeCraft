#include "RangedUnitStrategy.hpp"

RangedUnitStrategy::RangedUnitStrategy(const EntityIndex unitIndex,
                                       const PlayerView& playerView,
                                       const FocusAttackManager& focusAttackManager,
                                       AttackActionObserversList&& attackObservers)
    : _unitIndex{unitIndex}
    , _playerView{playerView}
    , _focusAttackManager{focusAttackManager}
    , _attackObservers{std::move(attackObservers)}
{}

std::unique_ptr<AttackAction> RangedUnitStrategy::generateAttackAction() const
{
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
    // TODO: check function call is needed or not.
    const auto attackTarget = _focusAttackManager.calculateEnemyToAttack(_playerView.entities.at(_unitIndex));
    if (attackTarget)
    {
        return nullptr;
    }
    
    Vec2Int target{_playerView.mapSize - 1, _playerView.mapSize - 1};
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
