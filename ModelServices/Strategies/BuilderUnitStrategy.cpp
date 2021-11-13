#include "BuilderUnitStrategy.hpp"

#include "../EntityDetector.hpp"

BuilderUnitStrategy::BuilderUnitStrategy(const Entity& unit, const PlayerView& playerView, const EntityManager& entityManager)
    : _unit{unit}
    , _playerView{playerView}
{
    const auto enemyID = playerView.players.at(0).id != playerView.myId ? playerView.players.at(0).id
                                                                        : playerView.players.at(1).id;
    
    const EntityDetector entityDetector{playerView, entityManager};
    const MapRange longRange(playerView, unit.position, 8);
    const MapRange smallRange(playerView, unit.position, 4);
    
    auto rangedUnitsNearby = entityDetector.getInRange(longRange, enemyID, {RANGED_UNIT});
    auto meleeUnitsNearby = entityDetector.getInRange(smallRange, enemyID, {MELEE_UNIT});
    
    _enemyUnits.assign(std::make_move_iterator(rangedUnitsNearby.begin()),
                       std::make_move_iterator(rangedUnitsNearby.end()));
    _enemyUnits.assign(std::make_move_iterator(meleeUnitsNearby.begin()),
                       std::make_move_iterator(meleeUnitsNearby.end()));
}

std::unique_ptr<AttackAction> BuilderUnitStrategy::generateAttackAction() const
{
    if (!_enemyUnits.empty())
    {
        return nullptr;
    }
    
    const auto& properties = _playerView.entityProperties.at(BUILDER_UNIT);
    std::vector<EntityType> validAutoAttackTargets{RESOURCE};

    auto autoAttackAction = std::make_shared<AutoAttack>(properties.sightRange, validAutoAttackTargets);
    return std::make_unique<AttackAction>(nullptr, std::move(autoAttackAction));
}

std::unique_ptr<MoveAction> BuilderUnitStrategy::generateMoveAction() const
{
    if (_enemyUnits.empty())
    {
        return nullptr;
    }
    
    Vec2Int target = _unit.position;
    for (auto unitIndex : _enemyUnits)
    {
        if (unitIndex >= _playerView.entities.size())
        {
            continue;
        }
        
        const auto& enemyUnit = _playerView.entities[unitIndex];
        
        target.x += _unit.position.x - enemyUnit.position.x;
        target.y += _unit.position.y - enemyUnit.position.y;
    }
    
    // TODO: check is target empty or not. If not - maybe try to multiply vector.
    
    return std::make_unique<MoveAction>(std::move(target), true, false);
}

std::unique_ptr<BuildAction> BuilderUnitStrategy::generateBuildAction() const
{
    return nullptr;
}