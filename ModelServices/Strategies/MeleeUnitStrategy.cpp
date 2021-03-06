#include "MeleeUnitStrategy.hpp"

MeleeUnitStrategy::MeleeUnitStrategy(const Entity& unit, const PlayerView& playerView)
    : _unit{unit}
    , _playerView{playerView}
{}

std::unique_ptr<AttackAction> MeleeUnitStrategy::generateAttackAction() const
{
    const auto& properties = _playerView.entityProperties.at(MELEE_UNIT);
    auto autoAttackAction = std::make_unique<AutoAttack>(properties.sightRange, std::vector<EntityType>{});
    
    return std::make_unique<AttackAction>(nullptr, std::move(autoAttackAction));
}

std::unique_ptr<MoveAction> MeleeUnitStrategy::generateMoveAction() const
{
    Vec2Int target{_playerView.mapSize - 15, _playerView.mapSize - 15};
    const auto findClosestPosition = true;
    const auto breakThrough = true;
    
    return std::make_unique<MoveAction>(std::move(target), findClosestPosition, breakThrough);
}

std::unique_ptr<BuildAction> MeleeUnitStrategy::generateBuildAction() const
{
    return nullptr;
}

std::unique_ptr<RepairAction> MeleeUnitStrategy::generateRepairAction() const
{
    return nullptr;
}
