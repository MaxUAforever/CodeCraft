#include "MyStrategy.hpp"
#include <exception>

MyStrategy::MyStrategy() {}

Action MyStrategy::getAction(const PlayerView& playerView, DebugInterface* debugInterface)
{
    Action result;
    int myId = playerView.myId;
    
    for (const auto& entity : playerView.entities)
    {
        if (entity.playerId == nullptr || *entity.playerId != myId)
        {
            continue;
        }
        
        const auto& properties = playerView.entityProperties.at(entity.entityType);

        std::shared_ptr<MoveAction> moveAction = nullptr;
        std::shared_ptr<BuildAction> buildAction = nullptr;
        
        if (properties.canMove)
        {
            Vec2Int target{playerView.mapSize - 1, playerView.mapSize - 1};
            const auto findClosestPosition = true;
            const auto breakThrough = true;
            
            moveAction = std::make_shared<MoveAction>(std::move(target), findClosestPosition, breakThrough);
        }
        else if (properties.build != nullptr)
        {
            auto entityType = properties.build->options[0];
            
            size_t currentUnits = 0;
            for (const auto& entity : playerView.entities)
            {
                if (entity.playerId != nullptr && *entity.playerId == myId && entity.entityType == entityType)
                {
                    ++currentUnits;
                }
            }
            
            const auto& populationUse = playerView.entityProperties.at(entityType).populationUse;
            if ((currentUnits + 1) * populationUse <= properties.populationProvide)
            {
                Vec2Int position{entity.position.x + properties.size, entity.position.y + properties.size - 1};
                
                buildAction = std::make_shared<BuildAction>(entityType, std::move(position));
            }
        }
        
        std::vector<EntityType> validAutoAttackTargets;
        if (entity.entityType == BUILDER_UNIT)
        {
            validAutoAttackTargets.push_back(RESOURCE);
        }
        
        auto autoAttackAction = std::make_shared<AutoAttack>(properties.sightRange, validAutoAttackTargets);
        auto attackAction = std::make_shared<AttackAction>(nullptr, std::move(autoAttackAction));
        
        result.entityActions[entity.id] = EntityAction(moveAction, buildAction, attackAction, nullptr);
    }
    
    return result;
}

void MyStrategy::debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface)
{
    debugInterface.send(DebugCommand::Clear());
    debugInterface.getState();
}
