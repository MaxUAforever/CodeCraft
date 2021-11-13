#ifndef UnitStategyFactory_hpp
#define UnitStategyFactory_hpp

#include "IUnitStrategy.hpp"

#include "../../model/Model.hpp"

class EntityManager;

class UnitStrategyFactory
{
public:
    static std::unique_ptr<IUnitStrategy> create(const PlayerView& playerView,
                                                 const EntityManager& entityManager,
                                                 const Entity& unit);
};

#endif /* UnitStategyFactory_hpp */
