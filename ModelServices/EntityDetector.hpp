#ifndef EntityDetector_hpp
#define EntityDetector_hpp

#include "../model/Model.hpp"
#include "../model/MapRange.hpp"

#include "../Utils/EnumUtils.hpp"

#include "EntityManager.hpp"

#include <optional>
#include <unordered_set>

class EntityDetector
{
public:
    EntityDetector(const PlayerView& playerView, const EntityManager& entityManager);
    
    std::unordered_set<EntityIndex> getInRange(const MapRange& range,
                                               const std::optional<int> playerID,
                                               const std::vector<EntityType> entityTypes) const;
    
    std::unordered_set<EntityIndex> getOnDistanse(const Vec2Int& point,
                                                  const size_t fromDistance,
                                                  const size_t toDistance,
                                                  const std::optional<int> playerID,
                                                  const std::vector<EntityType> entityTypes) const;
    
    std::unordered_set<EntityIndex> getEnemiesInEntityRange(const EntityIndex unitIndex,
                                                            const std::vector<EntityType> enemiesTypes) const;
        
private:
    const PlayerView& _playerView;
    const EntityManager& _entityManager;
};

#endif /* EntityDetector_hpp */
