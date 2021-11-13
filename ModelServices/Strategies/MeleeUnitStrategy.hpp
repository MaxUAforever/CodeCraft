#ifndef MeleeUnitStrategy_hpp
#define MeleeUnitStrategy_hpp

#include "IUnitStrategy.hpp"

#include "../EntityManager.hpp"

#include "../../model/Model.hpp"

class MeleeUnitStrategy : public IUnitStrategy
{
public:
    MeleeUnitStrategy(const Entity& unit, const PlayerView& playerView);
    
    std::unique_ptr<AttackAction> generateAttackAction() const override;
    std::unique_ptr<BuildAction> generateBuildAction() const override;
    std::unique_ptr<MoveAction> generateMoveAction() const override;
    
private:
    const Entity& _unit;
    const PlayerView _playerView;
};

#endif /* MeleeUnitStrategy_hpp */
