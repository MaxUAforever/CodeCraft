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
    
    _squadManager.update(playerView, entityManager);
    
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
                                                                  _squadManager,
                                                                  buildingsManager,
                                                                  {&focusAttackManager});
            
            result.entityActions[entity.id] = EntityAction{unitStrategy->generateMoveAction(),
                                                           unitStrategy->generateBuildAction(),
                                                           unitStrategy->generateAttackAction(),
                                                           unitStrategy->generateRepairAction()};
        }
        else if (properties.build != nullptr)
        {
            if (entity.entityType == MELEE_BASE)
            {
                continue;
            }
            
            auto currentBuildings = entityManager.getEntities({{playerView.myId, BUILDER_BASE},
                                                               {playerView.myId, MELEE_BASE},
                                                               {playerView.myId, RANGED_BASE},
                                                               {playerView.myId, HOUSE}});
            
            const auto isBuildingActive = [&playerView](const EntityIndex entityIndex)
            {
                return playerView.entities[entityIndex].active;
            };
            
            const auto activeBuildings = std::count_if(currentBuildings.begin(),
                                                       currentBuildings.end(),
                                                       isBuildingActive);
            
            auto populationProvide = activeBuildings * 5;
            
            if (entity.entityType == BUILDER_BASE)
            {
                const auto alliasBuildersCount = entityManager.getEntities({playerView.myId, BUILDER_UNIT}).size();
                const auto enemyBuildersCount = entityManager.getEntities({enemyID, BUILDER_UNIT}).size();
                
                if (alliasBuildersCount > enemyBuildersCount && alliasBuildersCount > (populationProvide * 0.2))
                {
                    result.entityActions[entity.id] = EntityAction(nullptr, nullptr, nullptr, nullptr);
                    continue;
                }
            }
            
            if (entity.entityType == RANGED_BASE)
            {
                const auto alliasRangesCount = entityManager.getEntities({playerView.myId, RANGED_UNIT}).size();
                
                if (alliasRangesCount > (populationProvide * 0.8))
                {
                    result.entityActions[entity.id] = EntityAction(nullptr, nullptr, nullptr, nullptr);
                    continue;
                }
            }
            
            auto entityTypeToBuild = properties.build->options[0];
            Vec2Int position{entity.position.x + properties.size, entity.position.y + properties.size - 1};

            auto buildAction = std::make_unique<BuildAction>(entityTypeToBuild, std::move(position));
            result.entityActions[entity.id] = EntityAction(nullptr, std::move(buildAction), nullptr, nullptr);
        }
    }
    
    return result;
}

void MyStrategy::debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface)
{
    debugInterface.send(DebugCommand::Clear());
    debugInterface.getState();
}
