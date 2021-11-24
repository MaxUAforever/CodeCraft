#include "MyStrategy.hpp"

#include "model/MapRange.hpp"
#include "ModelServices/BuildingsManager.hpp"
#include "ModelServices/EntityManager.hpp"
#include "ModelServices/FocusAttackManager.hpp"
#include "ModelServices/Strategies/UnitStategyFactory.hpp"
#include "Utils/EnumUtils.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

namespace
{
constexpr auto UnitEntityTypes = Utils::makeFlagForEnum(BUILDER_UNIT) |
                                 Utils::makeFlagForEnum(MELEE_UNIT) |
                                 Utils::makeFlagForEnum(RANGED_UNIT);
} // namespace

MyStrategy::MyStrategy() {}

Action MyStrategy::getAction(const PlayerView& playerView, DebugInterface* debugInterface)
{
    Action result;
    const auto enemyID = playerView.players.at(0).id != playerView.myId ? playerView.players.at(0).id
                                                                        : playerView.players.at(1).id;
    
    EntityManager entityManager{playerView};
    FocusAttackManager focusAttackManager{playerView, entityManager};
    BuildingsManager buildingsManager{playerView, entityManager};
    
    for (auto entityIndex = 0u; entityIndex < playerView.entities.size(); ++entityIndex)
    {
        const auto& entity = playerView.entities[entityIndex];
        
        if (entity.playerId == nullptr || *entity.playerId != playerView.myId)
        {
            continue;
        }
        
        const auto& properties = playerView.entityProperties.at(entity.entityType);

        if (Utils::hasFlags(UnitEntityTypes, Utils::makeFlagForEnum(entity.entityType)))
        {
            const auto unitStrategy = UnitStrategyFactory::create(entityIndex,
                                                                  playerView,
                                                                  entityManager,
                                                                  focusAttackManager,
                                                                  buildingsManager,
                                                                  {&focusAttackManager});
            
            result.entityActions[entity.id] = EntityAction{unitStrategy->generateMoveAction(),
                                                           unitStrategy->generateBuildAction(),
                                                           unitStrategy->generateAttackAction(),
                                                           nullptr};
        }
        else if (properties.build != nullptr)
        {
            if (entity.entityType == MELEE_BASE)
            {
                continue;
            }
            
            if (entity.entityType == BUILDER_BASE)
            {
                const auto alliasBuildersCount = entityManager.getEntities({playerView.myId, BUILDER_UNIT}).size();
                const auto enemyBuildersCount = entityManager.getEntities({enemyID, BUILDER_UNIT}).size();
                
                if (alliasBuildersCount > enemyBuildersCount)
                {
                    result.entityActions[entity.id] = EntityAction(nullptr, nullptr, nullptr, nullptr);
                    continue;
                }
            }
            
            auto entityTypeToBuild = properties.build->options[0];
            auto currentUnitsCount = entityManager.getEntities({playerView.myId, entityTypeToBuild}).size();

            const auto& populationUse = playerView.entityProperties.at(entityTypeToBuild).populationUse;
            if ((currentUnitsCount + 1) * populationUse <= properties.populationProvide)
            {
                Vec2Int position{entity.position.x + properties.size, entity.position.y + properties.size - 1};

                auto buildAction = std::make_unique<BuildAction>(entityTypeToBuild, std::move(position));
                result.entityActions[entity.id] = EntityAction(nullptr, std::move(buildAction), nullptr, nullptr);
            }
        }
    }
    
    return result;
}

void MyStrategy::debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface)
{
    debugInterface.send(DebugCommand::Clear());
    debugInterface.getState();
}
