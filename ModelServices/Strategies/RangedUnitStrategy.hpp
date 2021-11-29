#ifndef RangedUnitStrategy_hpp
#define RangedUnitStrategy_hpp

#include "IUnitStrategy.hpp"
#include "IAttackActionObserver.hpp"

#include "../EntityManager.hpp"
#include "../FocusAttackManager.hpp"
#include "../SquadManager.hpp"

#include "../../model/Model.hpp"

class RangedUnitStrategy : public IUnitStrategy
{
public:
    RangedUnitStrategy(const EntityIndex unitIndex,
                       const PlayerView& playerView,
                       const EntityManager& entityManager,
                       const FocusAttackManager& focusAttackManager,
                       const SquadManager& squadManager,
                       AttackActionObserversList&& attackObservers);
    
    std::unique_ptr<AttackAction> generateAttackAction() const override;
    std::unique_ptr<BuildAction> generateBuildAction() const override;
    std::unique_ptr<MoveAction> generateMoveAction() const override;
    std::unique_ptr<RepairAction> generateRepairAction() const override;
    
private:
    void notifyAttackActionGenerated(const EntityIndex enemyIndex) const;
    
private:
    const PlayerView& _playerView;
    const EntityManager& _entityManager;
    const FocusAttackManager& _focusAttackManager;
    const SquadManager& _squadManager;
    
    EntityIndex _unitIndex;
    bool _isInDanger;
    
    AttackActionObserversList _attackObservers;
};

#endif /* RangedUnitStrategy_hpp */
