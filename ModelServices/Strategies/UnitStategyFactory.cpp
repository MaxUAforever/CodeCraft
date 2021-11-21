#include "UnitStategyFactory.hpp"

#include "BuilderUnitStrategy.hpp"
#include "MeleeUnitStrategy.hpp"
#include "RangedUnitStrategy.hpp"

#include "../EntityManager.hpp"
#include "../FocusAttackManager.hpp"

std::unique_ptr<IUnitStrategy> UnitStrategyFactory::create(const EntityIndex unitIndex,
                                                           const PlayerView& playerView,
                                                           const EntityManager& entityManager,
                                                           const FocusAttackManager& focusAttackManager,
                                                           AttackActionObserversList&& attackObservers)
{
    const auto& unit = playerView.entities.at(unitIndex);
    
    switch(unit.entityType)
    {
        case EntityType::BUILDER_UNIT:
            return std::make_unique<BuilderUnitStrategy>(unit, playerView, entityManager);
        case EntityType::MELEE_UNIT:
            return std::make_unique<MeleeUnitStrategy>(unit, playerView);
        case EntityType::RANGED_UNIT:
            return std::make_unique<RangedUnitStrategy>(unitIndex, playerView, focusAttackManager, std::move(attackObservers));
        default:
            return nullptr;
    }
}
