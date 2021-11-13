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
        entitiesIndexes.assign(std::make_move_iterator(foundedEntites.begin()),
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
    const MapRange outerRange{_playerView, point, toDistance};
    const MapRange innerRange{_playerView, point, fromDistance ? fromDistance - 1 : 0};
    
    const auto outerEntities = getInRange(outerRange, playerID, entityTypes);
    const auto innerEntities = getInRange(innerRange, playerID, entityTypes);
    
    std::unordered_set<EntityIndex> result;
    for (auto unitIndex : outerEntities)
    {
        if (!innerEntities.contains(unitIndex))
        {
            result.emplace(unitIndex);
        }
    }
    
    return result;
}
