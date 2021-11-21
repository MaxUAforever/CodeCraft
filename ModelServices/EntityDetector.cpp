#include "EntityDetector.hpp"

EntityDetector::EntityDetector(const PlayerView& playerView, const EntityManager& entityManager)
    : _playerView(playerView)
    , _entityManager(entityManager)
{}

std::unordered_set<EntityIndex> EntityDetector::getInRange(const MapRange& range,
                                                           const std::optional<int> playerID,
                                                           const std::vector<EntityType> entityTypes) const
{
    std::vector<EntityIndex> entitiesIndexes;
    
    for (auto entityType : entityTypes)
    {
        auto foundedEntites = _entityManager.getEntities({playerID, entityType});
        entitiesIndexes.insert(entitiesIndexes.end(),
                               std::make_move_iterator(foundedEntites.begin()),
                               std::make_move_iterator(foundedEntites.end()));
    }
    
    std::unordered_set<EntityIndex> result;
    for (auto unitIndex : entitiesIndexes)
    {
        if (unitIndex >= _playerView.entities.size())
        {
            continue;
        }
        
        const auto& unit = _playerView.entities[unitIndex];
        if (range.contains(unit.position))
        {
            result.emplace(unitIndex);
        }
    }
    
    return result;
}

std::unordered_set<EntityIndex> EntityDetector::getOnDistanse(const Vec2Int& point,
                                                              const size_t fromDistance,
                                                              const size_t toDistance,
                                                              const std::optional<int> playerID,
                                                              const std::vector<EntityType> entityTypes) const
{
    std::vector<EntityIndex> entitiesIndexes;
    for (auto entityType : entityTypes)
    {
        auto foundedEntites = _entityManager.getEntities({playerID, entityType});
        entitiesIndexes.insert(entitiesIndexes.end(),
                               std::make_move_iterator(foundedEntites.begin()),
                               std::make_move_iterator(foundedEntites.end()));
    }
    
    std::unordered_set<EntityIndex> result;
    for (auto unitIndex : entitiesIndexes)
    {
        if (unitIndex >= _playerView.entities.size())
        {
            continue;
        }
        
        const auto& unit = _playerView.entities[unitIndex];
        const auto distanceX = std::abs(point.x - unit.position.x);
        const auto distanceY = std::abs(point.y - unit.position.y);
        const auto actualDistance = distanceX + distanceY;
        
        if (actualDistance >= fromDistance && actualDistance <= toDistance)
        {
            result.emplace(unitIndex);
        }
    }
    
    return result;
}
