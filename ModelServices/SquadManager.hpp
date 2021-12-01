#ifndef SquadManager_hpp
#define SquadManager_hpp

#include "EntityManager.hpp"

#include "../model/Model.hpp"

#include <optional>
#include <unordered_set>
#include <vector>

class MapRange;

class SquadManager
{
public:
    void update(const PlayerView& playerView, const EntityManager& entityManager);
    
    bool isBuilder(const EntityID entityID) const;
    bool isLeftDefender(const EntityID entityID) const;
    bool isRightDefender(const EntityID entityID) const;
    bool isPusher(const EntityID entityID) const;
    bool isSaboteur(const EntityID entityID) const;
    
private:
    void updatePushers(const PlayerView& playerView, const EntityManager& entityManager);
    
    void updateBuilders(const PlayerView& playerView, const EntityManager& entityManager);
    
    void updateDefenders(const PlayerView& playerView, const EntityManager& entityManager);
    
    void updateSideDefenders(std::unordered_set<EntityID>& sideDefenders,
                             const PlayerView& playerView,
                             const EntityManager& entityManager,
                             const MapRange& sideRange);
    
    void updateSaboteurs(const PlayerView& playerView, const EntityManager& entityManager);
    
private:
    std::unordered_set<EntityID> _builders;
    
    std::unordered_set<EntityID> _leftDefenders;
    std::unordered_set<EntityID> _rightDefenders;
    
    std::unordered_set<EntityID> _pushers;
    
    std::unordered_set<EntityID> _saboteurs;
};

#endif /* SquadManager_hpp */
