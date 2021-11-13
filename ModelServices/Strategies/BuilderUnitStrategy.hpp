#ifndef BuilderUnitStrategy_hpp
#define BuilderUnitStrategy_hpp

#include "IUnitStrategy.hpp"

#include "../EntityManager.hpp"

#include "../../model/Model.hpp"

class BuilderUnitStrategy : public IUnitStrategy
{
public:
    BuilderUnitStrategy(const Entity& unit, const PlayerView& playerView, const EntityManager& entityManager);
    
    std::unique_ptr<AttackAction> generateAttackAction() const override;
    std::unique_ptr<BuildAction> generateBuildAction() const override;
    std::unique_ptr<MoveAction> generateMoveAction() const override;
    
private:
    const Entity& _unit;
    const PlayerView& _playerView;
    
    std::vector<EntityIndex> _enemyUnits;
};

#endif /* BuilderUnitStrategy_hpp */
