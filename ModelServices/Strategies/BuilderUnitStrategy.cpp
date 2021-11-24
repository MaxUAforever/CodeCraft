#include "BuilderUnitStrategy.hpp"

#include "../EntityDetector.hpp"

BuilderUnitStrategy::BuilderUnitStrategy(const EntityIndex unitIndex,
                                         const PlayerView& playerView,
                                         const EntityManager& entityManager,
                                         const BuildingsManager& buildingsManager)
    : _playerView{playerView}
    , _entityManager{entityManager}
    , _unitIndex{unitIndex}
    , _buildingsManager{buildingsManager}
{
    const auto enemyID = playerView.players.at(0).id != playerView.myId ? playerView.players.at(0).id
                                                                        : playerView.players.at(1).id;
    
    const EntityDetector entityDetector{playerView, entityManager};
    const auto& unit = _playerView.entities[_unitIndex];
    
    auto meleeUnitsNearby = entityDetector.getOnDistanse(unit.position, 0, 4, enemyID, {MELEE_UNIT});
    auto rangedUnitsNearby = entityDetector.getOnDistanse(unit.position, 0, 8, enemyID, {RANGED_UNIT});
    
    _enemyUnits.insert(_enemyUnits.end(), std::make_move_iterator(rangedUnitsNearby.begin()),
                       std::make_move_iterator(rangedUnitsNearby.end()));
    _enemyUnits.insert(_enemyUnits.end(), std::make_move_iterator(meleeUnitsNearby.begin()),
                       std::make_move_iterator(meleeUnitsNearby.end()));
}

std::unique_ptr<AttackAction> BuilderUnitStrategy::generateAttackAction() const
{
    if (!_enemyUnits.empty())
    {
        return nullptr;
    }
    
    std::vector<EntityType> validAutoAttackTargets{RESOURCE};

    auto autoAttackAction = std::make_shared<AutoAttack>(_playerView.mapSize, validAutoAttackTargets);
    return std::make_unique<AttackAction>(nullptr, std::move(autoAttackAction));
}

std::unique_ptr<MoveAction> BuilderUnitStrategy::generateMoveAction() const
{
    if (_enemyUnits.empty())
    {
        return nullptr;
    }
    
    const auto& unit = _playerView.entities[_unitIndex];
    
    Vec2Int target = unit.position;
    for (auto unitIndex : _enemyUnits)
    {
        if (unitIndex >= _playerView.entities.size())
        {
            continue;
        }
        
        const auto& enemyUnit = _playerView.entities[unitIndex];
        
        target.x += unit.position.x - enemyUnit.position.x;
        target.y += unit.position.y - enemyUnit.position.y;
    }
    
    const MapRange mapRange{0, 0, _playerView.mapSize, _playerView.mapSize};
    if (!mapRange.contains(target))
    {
        target = {0, 0};
    }
    
    return std::make_unique<MoveAction>(std::move(target), true, false);
}

std::unique_ptr<BuildAction> BuilderUnitStrategy::generateBuildAction() const
{
    if (!_enemyUnits.empty())
    {
        return nullptr;
    }
    
    const auto typeToBuild = _buildingsManager.getTypeToBuild(_unitIndex);
    if (!typeToBuild)
    {
        return nullptr;
    }
    
    MapRange mapRangeForBuild{0, 0, 30, 30};
    Vec2Int buildPoint{mapRangeForBuild.getBeginX(), mapRangeForBuild.getBeginY()};
    
    const auto& buildingProperties = _playerView.entityProperties.at(*typeToBuild);
    const auto buildingSize = buildingProperties.size;
    
    const auto obstaclesMap = getObstaclesMap(mapRangeForBuild);
    const auto hasObstacles = [&obstaclesMap](const int x, const int y)
    {
        return obstaclesMap.contains({x, y});
    };
    
    bool isInBuildRange = true;
    while (isInBuildRange)
    {
        MapRange buildingRange{buildPoint.x,
                               buildPoint.y,
                               buildPoint.x + buildingSize - 1,
                               buildPoint.y + buildingSize - 1};
        
        if (buildingRange.getLastY() > mapRangeForBuild.getLastX())
        {
            isInBuildRange = false;
            continue;
        }
        
        if (buildingRange.getLastX() > mapRangeForBuild.getLastX())
        {
            buildPoint = {0, buildPoint.y + buildingSize + 1};
            continue;
        }
        
        if (!buildingRange.findIf(hasObstacles))
        {
            return std::make_unique<BuildAction>(*typeToBuild, buildPoint);
        }
        
        buildPoint.x += buildingProperties.size + 1;
    }
    
    return nullptr;
}

std::unordered_set<Vec2Int> BuilderUnitStrategy::getObstaclesMap(const MapRange& rangeForBuild) const
{
    const auto obstacles = _entityManager.getEntities({{std::nullopt, RESOURCE},
                                                       {_playerView.myId, HOUSE},
                                                       {_playerView.myId, BUILDER_BASE},
                                                       {_playerView.myId, MELEE_BASE},
                                                       {_playerView.myId, RANGED_BASE},
                                                       {_playerView.myId, TURRET}});
    
    std::unordered_set<Vec2Int> obstaclesMap;
    for (const auto& obstacleIndex : obstacles)
    {
        const auto& obstacle = _playerView.entities[obstacleIndex];
        const auto& obstacleProperties = _playerView.entityProperties.at(obstacle.entityType);
        
        const auto pos = obstacle.position;
        if (!rangeForBuild.contains(pos))
        {
            continue;
        }
        
        for (auto x = pos.x; x < pos.x + obstacleProperties.size; ++x)
        {
            for (auto y = pos.y; y < pos.y + obstacleProperties.size; ++y)
            {
                obstaclesMap.emplace(x, y);
            }
        }
    }
    
    return obstaclesMap;
}
