#ifndef SquadManager_hpp
#define SquadManager_hpp

#include "EntityManager.hpp"

#include "../model/Model.hpp"

#include <optional>
#include <unordered_set>
#include <vector>

class SquadManager
{
public:
    void update(const PlayerView& playerView, const EntityManager& entityManager);
    
    bool isDefender(const EntityID entityID) const;
    bool isSaboteur(const EntityID entityID) const;
    
private:
    void updateDefenders(const PlayerView& playerView, const EntityManager& entityManager);
    void updateSaboteurs(const PlayerView& playerView, const EntityManager& entityManager);

private:
    std::unordered_set<EntityID> _defenders;
    std::unordered_set<EntityID> _saboteurs;
};

#endif /* SquadManager_hpp */
