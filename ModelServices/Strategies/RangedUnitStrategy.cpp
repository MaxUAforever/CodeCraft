#include "RangedUnitStrategy.hpp"

RangedUnitStrategy::RangedUnitStrategy(const Entity& unit, const PlayerView& playerView)
    : _unit{unit}
    , _playerView{playerView}
{}

std::unique_ptr<AttackAction> RangedUnitStrategy::generateAttackAction() const
{
    const auto& properties = _playerView.entityProperties.at(MELEE_UNIT);
    auto autoAttackAction = std::make_unique<AutoAttack>(properties.sightRange, std::vector<EntityType>{});
    
    return std::make_unique<AttackAction>(nullptr, std::move(autoAttackAction));
}

std::unique_ptr<MoveAction> RangedUnitStrategy::generateMoveAction() const
{
    Vec2Int target{_playerView.mapSize - 1, _playerView.mapSize - 1};
    const auto findClosestPosition = true;
    const auto breakThrough = true;
    
    return std::make_unique<MoveAction>(std::move(target), findClosestPosition, breakThrough);
}

std::unique_ptr<BuildAction> RangedUnitStrategy::generateBuildAction() const
{
    return nullptr;
}
