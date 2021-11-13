#ifndef _ENTITY_MANAGER_HPP_
#define _ENTITY_MANAGER_HPP_

#include "../model/Entity.hpp"
#include "../model/EntityType.hpp"
#include "../model/PlayerView.hpp"

#include <optional>
#include <unordered_map>
#include <vector>

using PlayerID = int;
using EntityIndex = int;

struct EntityKey
{
    std::optional<PlayerID> playerID;
    EntityType entityType;
    
    bool operator==(const EntityKey& other) const;
};

struct EntityKeyHasher
{
    std::size_t operator()(const EntityKey key) const;
};

class EntityManager
{
public:
    explicit EntityManager(const PlayerView& playerView);
    
    std::vector<EntityIndex> getEntities(const EntityKey entityKey) const;
    
private:
    std::unordered_map<EntityKey, std::vector<EntityIndex>, EntityKeyHasher> _entities;
};

#endif /* _ENTITY_MANAGER_HPP_ */
