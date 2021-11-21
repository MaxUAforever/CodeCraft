#ifndef FocusAttackManager_hpp
#define FocusAttackManager_hpp

#include "Strategies/IAttackActionObserver.hpp"
#include "EntityManager.hpp"

#include "../model/Model.hpp"

#include <optional>
#include <unordered_map>
#include <vector>

struct AttackInfo
{
    std::vector<EntityIndex> alliesInEnemyRange;
    size_t potentialDamage = 0u;
};

class FocusAttackManager : public IAttackActionObserver
{
public:
    FocusAttackManager(const PlayerView& playerView, const EntityManager& entityManager);
    
    std::vector<EntityIndex> getEnemyFocusAllies(const EntityIndex enemyIndex) const;
    std::optional<EntityID> calculateEnemyToAttack(const Entity& entity) const;
    
    
public:
    void onAttackActionGenerated(const EntityIndex entityIndex, const EntityIndex enemyIndex) override;
    
private:
    const PlayerView& _playerView;
    
    std::unordered_map<EntityID, std::vector<EntityIndex>> _enemiesInEntityRange;
    std::unordered_map<EntityIndex, AttackInfo> _enemyAttackInfo;
};

#endif /* FocusAttackManager_hpp */
