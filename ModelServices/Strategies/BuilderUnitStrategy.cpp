#include "BuilderUnitStrategy.hpp"

#include "../EntityDetector.hpp"

BuilderUnitStrategy::BuilderUnitStrategy(const EntityIndex unitIndex,
                                         const PlayerView& playerView,
                                         const EntityManager& entityManager,
                                         const BuildingsManager& buildingsManager,
                                         const SquadManager& squadManager)
    : _playerView{playerView}
    , _entityManager{entityManager}
    , _unitIndex{unitIndex}
    , _squadManager{squadManager}
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
    
    if (_squadManager.isBuilder(unit.id))
    {
        const auto typeToBuild = buildingsManager.getTypeToBuild(_unitIndex);
        const auto buildPoint = typeToBuild ? calculateBuildPoint(*typeToBuild) : std::nullopt;
    
        _buildInfo = typeToBuild && buildPoint ? std::make_optional<BuildInfo>({*typeToBuild, *buildPoint}) : std::nullopt;
    }
    else
    {
        _buildInfo = std::nullopt;
    }
}

std::unique_ptr<AttackAction> BuilderUnitStrategy::generateAttackAction() const
{
    if (!_enemyUnits.empty() || _buildInfo)
    {
        return nullptr;
    }
    
    std::vector<EntityType> validAutoAttackTargets{RESOURCE};

    auto autoAttackAction = std::make_shared<AutoAttack>(_playerView.mapSize, validAutoAttackTargets);
    return std::make_unique<AttackAction>(nullptr, std::move(autoAttackAction));
}

std::unique_ptr<MoveAction> BuilderUnitStrategy::generateMoveAction() const
{
    if (!_enemyUnits.empty())
    {
        const auto target = calcuateAvoidEnemiesPoint();
        return std::make_unique<MoveAction>(std::move(target), true, false);
    }
    
    if (_buildInfo)
    {
        const auto& unit = _playerView.entities[_unitIndex];
        
        const auto& buildingProperties = _playerView.entityProperties.at(_buildInfo->buildType);
        const int buildingMiddle = (buildingProperties.size / 2);
        
        if (const auto& builingIndex = getInactiveBuilding(_buildInfo->buildType))
        {
            const auto& building = _playerView.entities[*builingIndex];
            const Vec2Int target{building.position.x + buildingMiddle, building.position.y + buildingMiddle};
            
            const MapRange rangeForRepairing{_playerView, target, static_cast<size_t>(buildingMiddle + 1)};
            
            const auto distanceX = std::abs(target.x - unit.position.x);
            const auto distanceY = std::abs(target.y - unit.position.y);
            const auto actualDistance = distanceX + distanceY;
            
            const auto isOnXSide = distanceX == (buildingMiddle + 1) && distanceY < (buildingMiddle + 1);
            const auto isOnYSide = distanceY == (buildingMiddle + 1) && distanceX < (buildingMiddle + 1);
            if (isOnXSide || isOnYSide)
            {
                return nullptr;
            }
            
            return std::make_unique<MoveAction>(std::move(target), true, false);
        }
        
        const Vec2Int target{_buildInfo->buildPoint.x + buildingMiddle, _buildInfo->buildPoint.y + buildingMiddle};
        const MapRange rangeForBuild{_playerView, target, static_cast<size_t>(buildingMiddle + 1)};
        
        if (rangeForBuild.contains(unit.position))
        {
            // If unit is in corner if range - move it from it.
            if (unit.position.x == rangeForBuild.getLastX() || unit.position.x == rangeForBuild.getBeginX())
            {
                if (unit.position.y == rangeForBuild.getLastY())
                {
                    return std::make_unique<MoveAction>(Vec2Int{unit.position.x, unit.position.y - 1}, true, false);
                }
                
                if (unit.position.y == rangeForBuild.getBeginY())
                {
                    return std::make_unique<MoveAction>(Vec2Int{unit.position.x, unit.position.y + 1}, true, false);
                }
            }
            
            const MapRange buildRange{_playerView, target, static_cast<size_t>(buildingMiddle)};
            if (buildRange.contains(unit.position))
            {
                // If unit take position of building - get away from it.
                return std::make_unique<MoveAction>(Vec2Int{_playerView.mapSize, _playerView.mapSize}, true, false);
            }
            
            // If unit can already build, it is no need to move.
            return nullptr;
        }
        
        return std::make_unique<MoveAction>(std::move(target), true, false);
    }
    
    return nullptr;
}

std::unique_ptr<BuildAction> BuilderUnitStrategy::generateBuildAction() const
{
    if (!_enemyUnits.empty())
    {
        return nullptr;
    }
    
    if (_buildInfo && !getInactiveBuilding(_buildInfo->buildType))
    {
        return std::make_unique<BuildAction>(_buildInfo->buildType, _buildInfo->buildPoint);
    }
    
    return nullptr;
}

std::unique_ptr<RepairAction> BuilderUnitStrategy::generateRepairAction() const
{
    if (!_enemyUnits.empty() || !_buildInfo)
    {
        return nullptr;
    }
    
    const auto buildingIndex = getInactiveBuilding(_buildInfo->buildType);
    if (!buildingIndex)
    {
        return nullptr;
    }
    
    const auto& buildingEntity = _playerView.entities[*buildingIndex];
    return std::make_unique<RepairAction>(buildingEntity.id);
}

std::unordered_set<Vec2Int> BuilderUnitStrategy::getObstaclesMap(const MapRange& rangeForBuild) const
{
    const auto obstacles = _entityManager.getEntities({{std::nullopt, RESOURCE},
                                                       {_playerView.myId, HOUSE},
                                                       {_playerView.myId, BUILDER_BASE},
                                                       {_playerView.myId, MELEE_BASE},
                                                       {_playerView.myId, RANGED_BASE},
                                                       {_playerView.myId, TURRET},
                                                       {_playerView.myId, BUILDER_UNIT},
                                                       {_playerView.myId, MELEE_UNIT},
                                                       {_playerView.myId, RANGED_UNIT}});
    
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

Vec2Int BuilderUnitStrategy::calcuateAvoidEnemiesPoint() const
{
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
}

std::optional<Vec2Int> BuilderUnitStrategy::calculateBuildPoint(const EntityType typeToBuild) const
{
    MapRange mapRangeForBuild{0, 0, 30, _playerView.mapSize};
    Vec2Int buildPoint{mapRangeForBuild.getBeginX(), mapRangeForBuild.getBeginY()};
    
    const auto& buildingProperties = _playerView.entityProperties.at(typeToBuild);
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
            buildPoint.y += buildingProperties.size + 1;
            buildPoint.x = mapRangeForBuild.getBeginX();
            
            continue;
        }
        
        if (!buildingRange.findIf(hasObstacles))
        {
            return Vec2Int{buildPoint.x, buildPoint.y};
        }
        
        const auto offset = buildPoint.y == 0 ? 0 : 1;
        buildPoint.x += buildingProperties.size + offset;
    }
    
    return std::nullopt;
}

std::optional<EntityIndex> BuilderUnitStrategy::getInactiveBuilding(const EntityType buildingType) const
{
    const auto buildings = _entityManager.getEntities({_playerView.myId, buildingType});
    for (auto buildingIndex : buildings)
    {
        const auto& buildingEntity = _playerView.entities[buildingIndex];
        if (!buildingEntity.active)
        {
            return buildingIndex;
        }
    }
    
    return std::nullopt;
}
