#ifndef IAttackActionObserver_hpp
#define IAttackActionObserver_hpp

#include "../../model/Defs.hpp"

#include <vector>

class IAttackActionObserver
{
public:
    virtual ~IAttackActionObserver() = default;
    
    virtual void onAttackActionGenerated(const EntityIndex entityIndex, const EntityIndex enemyIndex) = 0;
};

using AttackActionObserversList = std::vector<IAttackActionObserver*>;

#endif /* IAttackActionObserver_hpp */
