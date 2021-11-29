#ifndef UnitStategyFactory_hpp
#define UnitStategyFactory_hpp

#include "IUnitStrategy.hpp"
#include "IAttackActionObserver.hpp"

#include "../FocusAttackManager.hpp"
#include "../../model/Model.hpp"

class EntityManager;
class BuildingsManager;
class SquadManager;

class UnitStrategyFactory
{
public:
    static std::unique_ptr<IUnitStrategy> create(const EntityIndex unit,
                                                 const PlayerView& playerView,
                                                 const EntityManager& entityManager,
                                                 const FocusAttackManager& focusAttackManager,
                                                 const SquadManager& squadManager,
                                                 const BuildingsManager& buildingsManager,
                                                 AttackActionObserversList&& attackObservers);
};

#endif /* UnitStategyFactory_hpp */
