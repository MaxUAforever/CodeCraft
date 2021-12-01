#include "BuildingsManager.hpp"

#include <unordered_set>

BuildingsManager::BuildingsManager(const PlayerView& playerView, const EntityManager& entityManager)
    : _playerView{playerView}
    , _entityManager{entityManager}
{}

std::optional<EntityType> BuildingsManager::getTypeToBuild(const EntityIndex unitIndex) const
{
    if (!isBuildingAlive(BUILDER_BASE))
    {
        return BUILDER_BASE;
    }
    
    if (!isBuildingAlive(RANGED_BASE))
    {
        return RANGED_BASE;
    }
    
    const auto& unitBaseProperties = _playerView.entityProperties.at(RANGED_BASE);
    auto currentUnitsCount = _entityManager.getEntities({{_playerView.myId, RANGED_UNIT},
                                                         {_playerView.myId, BUILDER_UNIT},
                                                         {_playerView.myId, MELEE_UNIT}}).size();

    auto currentBuildings = _entityManager.getEntities({{_playerView.myId, BUILDER_BASE},
                                                        {_playerView.myId, MELEE_BASE},
                                                        {_playerView.myId, RANGED_BASE},
                                                        {_playerView.myId, HOUSE}});
    
    const auto isBuildingActive = [this](const EntityIndex entityIndex)
    {
        return _playerView.entities[entityIndex].active;
    };
    
    const auto activeBuildings = std::count_if(currentBuildings.begin(),
                                               currentBuildings.end(),
                                               isBuildingActive);
    
    auto populationProvide = activeBuildings * 5;
    const auto populationUse = currentUnitsCount;
    
    if (populationUse + 1 >= populationProvide)
    {
        return HOUSE;
    }
    
    return std::nullopt;
}

bool BuildingsManager::isBuildingAlive(const EntityType buildingType) const
{
    const auto buildings = _entityManager.getEntities({_playerView.myId, buildingType});
    if (buildings.empty())
    {
        return false;
    }
    
    const auto& building = _playerView.entities[buildings[0]];
    return building.active;
}
