#ifndef RangedUnitStrategy_hpp
#define RangedUnitStrategy_hpp

#include "IUnitStrategy.hpp"

#include "../EntityManager.hpp"

#include "../../model/Model.hpp"

class RangedUnitStrategy : public IUnitStrategy
{
public:
    RangedUnitStrategy(const Entity& unit, const PlayerView& playerView);
    
    std::unique_ptr<AttackAction> generateAttackAction() const override;
    std::unique_ptr<BuildAction> generateBuildAction() const override;
    std::unique_ptr<MoveAction> generateMoveAction() const override;
    
private:
    const Entity& _unit;
    const PlayerView _playerView;
};

#endif /* RangedUnitStrategy_hpp */
