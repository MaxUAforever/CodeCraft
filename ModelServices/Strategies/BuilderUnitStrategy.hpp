#ifndef BuilderUnitStrategy_hpp
#define BuilderUnitStrategy_hpp

#include "IUnitStrategy.hpp"

#include "../BuildingsManager.hpp"
#include "../EntityManager.hpp"

#include "../../model/Model.hpp"

class BuilderUnitStrategy : public IUnitStrategy
{
public:
    BuilderUnitStrategy(const EntityIndex unitIndex,
                        const PlayerView& playerView,
                        const EntityManager& entityManager,
                        const BuildingsManager& buildingsManager);
    
    std::unique_ptr<AttackAction> generateAttackAction() const override;
    std::unique_ptr<BuildAction> generateBuildAction() const override;
    std::unique_ptr<MoveAction> generateMoveAction() const override;
    
private:
    std::unordered_set<Vec2Int> getObstaclesMap(const MapRange& rangeForBuild) const;

private:
    const PlayerView& _playerView;
    const EntityManager& _entityManager;
    const BuildingsManager& _buildingsManager;
    
    const EntityIndex _unitIndex;
    std::vector<EntityIndex> _enemyUnits;
};

#endif /* BuilderUnitStrategy_hpp */
