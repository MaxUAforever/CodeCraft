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
    size_t alliesInEnemyRange = 0u;
    size_t potentialDamage = 0u;
};

class FocusAttackManager : public IAttackActionObserver
{
public:
    FocusAttackManager(const PlayerView& playerView, const EntityManager& entityManager);
    
    std::optional<EntityID> calculateEnemyToAttack(const Entity& entity) const;
    
    void onAttackActionGenerated(const EntityIndex entityIndex, const EntityIndex enemyIndex) override;
    
private:
    const PlayerView& _playerView;
    
    std::unordered_map<EntityID, std::vector<EntityIndex>> _enemiesInEntityRange;
    std::unordered_map<EntityIndex, AttackInfo> _enemyAttackInfo;
};

#endif /* FocusAttackManager_hpp */
