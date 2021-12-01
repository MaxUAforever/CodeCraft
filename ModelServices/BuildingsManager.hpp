#ifndef BuildingsManager_hpp
#define BuildingsManager_hpp

#include "../model/BuildAction.hpp"
#include "../model/Defs.hpp"
#include "../model/Entity.hpp"
#include "../model/EntityType.hpp"
#include "../model/MapRange.hpp"
#include "../model/PlayerView.hpp"

#include "EntityManager.hpp"

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class BuildingsManager
{
public:
    explicit BuildingsManager(const PlayerView& playerView, const EntityManager& entityManager);
    
    std::optional<EntityType> getTypeToBuild(const EntityIndex unitIndex) const;
    bool isEntityBuilder(const EntityIndex unitIndex) const;
    
private:
    std::unordered_set<Vec2Int> getObstaclesMap(const MapRange& rangeForBuild) const;
    
    bool isBuildingAlive(const EntityType buildingType) const;
    
private:
    const PlayerView& _playerView;
    const EntityManager& _entityManager;
    
    std::vector<EntityIndex> _builder;
};

#endif /* BuildingsManager_hpp */
