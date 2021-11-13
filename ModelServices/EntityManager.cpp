#include "EntityManager.hpp"

bool EntityKey::operator==(const EntityKey& other) const
{
    return std::tie(playerID, entityType) == std::tie(other.playerID, other.entityType);
}

std::size_t EntityKeyHasher::operator()(const EntityKey key) const
{
    std::size_t seed = 0;
    seed ^= std::hash<int>{}(key.entityType) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    seed ^= std::hash<std::optional<int>>{}(key.playerID) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    return seed;
}

EntityManager::EntityManager(const PlayerView& playerView)
{
    auto index = 0u;
    for (const auto& entity : playerView.entities)
    {
        const auto playerID = entity.playerId == nullptr ? std::nullopt : std::optional<int>(*entity.playerId);
        
        EntityKey key{playerID, entity.entityType};
        _entities[key].push_back(index);
        
        ++index;
    }
}

std::vector<EntityIndex> EntityManager::getEntities(const EntityKey entityKey) const
{
    const auto entitiesIt = _entities.find(entityKey);
    if (entitiesIt == _entities.cend())
    {
        return {};
    }
    
    return entitiesIt->second;
}
