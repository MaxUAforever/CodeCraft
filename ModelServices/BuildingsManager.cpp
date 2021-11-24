#include "BuildingsManager.hpp"

#include <unordered_set>

BuildingsManager::BuildingsManager(const PlayerView& playerView, const EntityManager& entityManager)
    : _playerView{playerView}
    , _entityManager{entityManager}
{
    const auto& builders = entityManager.getEntities({playerView.myId, BUILDER_UNIT});
    _builder = builders.size() < 2 ? std::nullopt : std::make_optional(builders[0]);
}

std::optional<EntityType> BuildingsManager::getTypeToBuild(const EntityIndex unitIndex) const
{
    if (_builder != unitIndex)
    {
        return std::nullopt;
    }
    
    const auto& unitBaseProperties = _playerView.entityProperties.at(RANGED_BASE);
    auto currentUnitsCount = _entityManager.getEntities({_playerView.myId, RANGED_UNIT}).size();

    const auto populationUse = _playerView.entityProperties.at(RANGED_UNIT).populationUse;
    if ((currentUnitsCount + 2) * populationUse >= unitBaseProperties.populationProvide)
    {
        return HOUSE;
    }
    
    return std::nullopt;
}

bool BuildingsManager::isEntityBuilder(const EntityIndex unitIndex) const
{
    return unitIndex == _builder;
}
