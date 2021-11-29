#ifndef BuilderUnitStrategy_hpp
#define BuilderUnitStrategy_hpp

#include "IUnitStrategy.hpp"

#include "../BuildingsManager.hpp"
#include "../EntityManager.hpp"

#include "../../model/Model.hpp"

struct BuildInfo
{
    EntityType buildType;
    Vec2Int buildPoint;
};

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
    std::unique_ptr<RepairAction> generateRepairAction() const override;
    
private:
    std::unordered_set<Vec2Int> getObstaclesMap(const MapRange& rangeForBuild) const;
    
    Vec2Int calcuateAvoidEnemiesPoint() const;
    std::optional<Vec2Int> calculateBuildPoint(const EntityType typeToBuild) const;
    std::optional<EntityIndex> getInactiveBuilding(const EntityType buildingType) const;
    
private:
    const PlayerView& _playerView;
    const EntityManager& _entityManager;
    
    const EntityIndex _unitIndex;
    std::vector<EntityIndex> _enemyUnits;
    std::optional<BuildInfo> _buildInfo;
};

#endif /* BuilderUnitStrategy_hpp */
