#ifndef RangedUnitStrategy_hpp
#define RangedUnitStrategy_hpp

#include "IUnitStrategy.hpp"
#include "IAttackActionObserver.hpp"

#include "../EntityManager.hpp"
#include "../FocusAttackManager.hpp"

#include "../../model/Model.hpp"

class RangedUnitStrategy : public IUnitStrategy
{
public:
    RangedUnitStrategy(const EntityIndex unitIndex,
                       const PlayerView& playerView,
                       const FocusAttackManager& focusAttackManager,
                       AttackActionObserversList&& attackObservers);
    
    std::unique_ptr<AttackAction> generateAttackAction() const override;
    std::unique_ptr<BuildAction> generateBuildAction() const override;
    std::unique_ptr<MoveAction> generateMoveAction() const override;
    
private:
    void notifyAttackActionGenerated(const EntityIndex enemyIndex) const;
    
private:
    const PlayerView& _playerView;
    const FocusAttackManager& _focusAttackManager;

    EntityIndex _unitIndex;
    
    AttackActionObserversList _attackObservers;
};

#endif /* RangedUnitStrategy_hpp */
