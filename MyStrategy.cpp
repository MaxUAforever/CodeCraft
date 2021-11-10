#include "MyStrategy.hpp"

#include "ModelServices/EntityManager.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

MyStrategy::MyStrategy() {}

Action MyStrategy::getAction(const PlayerView& playerView, DebugInterface* debugInterface)
{
    Action result;
    const auto enemyID = playerView.players.at(0).id != playerView.myId ? playerView.players.at(0).id
                                                                        : playerView.players.at(1).id;
    
    EntityManager entityManager{playerView};
    
    for (const auto& entity : playerView.entities)
    {
        if (entity.playerId == nullptr || *entity.playerId != playerView.myId)
        {
            continue;
        }
        
        const auto& properties = playerView.entityProperties.at(entity.entityType);

        std::shared_ptr<MoveAction> moveAction = nullptr;
        std::shared_ptr<BuildAction> buildAction = nullptr;
        std::shared_ptr<AttackAction> attackAction = nullptr;
        
        if (properties.canMove)
        {
            Vec2Int target{playerView.mapSize - 1, playerView.mapSize - 1};
            const auto findClosestPosition = true;
            const auto breakThrough = true;
            
            moveAction = std::make_shared<MoveAction>(std::move(target), findClosestPosition, breakThrough);
        }
        else if (properties.build != nullptr)
        {
            auto entityTypeToBuild = properties.build->options[0];
            auto currentUnitsCount = entityManager.getEntities({playerView.myId, entityTypeToBuild}).size();

            const auto& populationUse = playerView.entityProperties.at(entityTypeToBuild).populationUse;
            if (entity.entityType != MELEE_BASE && ((currentUnitsCount + 1) * populationUse <= properties.populationProvide))
            {
                Vec2Int position{entity.position.x + properties.size, entity.position.y + properties.size - 1};

                buildAction = std::make_shared<BuildAction>(entityTypeToBuild, std::move(position));
            }
        }
        
        std::vector<EntityType> validAutoAttackTargets;
        if (entity.entityType == BUILDER_UNIT)
        {
            validAutoAttackTargets.push_back(RESOURCE);
        }
        
        auto autoAttackAction = std::make_shared<AutoAttack>(properties.sightRange, validAutoAttackTargets);
        attackAction = std::make_shared<AttackAction>(nullptr, std::move(autoAttackAction));
        
        result.entityActions[entity.id] = EntityAction(std::move(moveAction), std::move(buildAction), std::move(attackAction), nullptr);
    }
    
    return result;
}

void MyStrategy::debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface)
{
    debugInterface.send(DebugCommand::Clear());
    debugInterface.getState();
}
