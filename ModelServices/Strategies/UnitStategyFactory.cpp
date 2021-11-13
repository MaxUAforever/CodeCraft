#include "UnitStategyFactory.hpp"

#include "BuilderUnitStrategy.hpp"
#include "MeleeUnitStrategy.hpp"
#include "RangedUnitStrategy.hpp"

#include "../EntityManager.hpp"

std::unique_ptr<IUnitStrategy> UnitStrategyFactory::create(const PlayerView& playerView,
                                                           const EntityManager& entityManager,
                                                           const Entity& unit)
{
    switch(unit.entityType)
    {
        case EntityType::BUILDER_UNIT:
            return std::make_unique<BuilderUnitStrategy>(unit, playerView, entityManager);
        case EntityType::MELEE_UNIT:
            return std::make_unique<MeleeUnitStrategy>(unit, playerView);
        case EntityType::RANGED_UNIT:
            return std::make_unique<RangedUnitStrategy>(unit, playerView);
        default:
            return nullptr;
    }
}
