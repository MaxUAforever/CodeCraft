#ifndef IUnitStrategy_hpp
#define IUnitStrategy_hpp

#include "../../model/Model.hpp"

#include <memory>

class IUnitStrategy
{
public:
    virtual ~IUnitStrategy() = default;
    
    virtual std::unique_ptr<AttackAction> generateAttackAction() const = 0;
    virtual std::unique_ptr<BuildAction> generateBuildAction() const = 0;
    virtual std::unique_ptr<MoveAction> generateMoveAction() const = 0;
    virtual std::unique_ptr<RepairAction> generateRepairAction() const = 0;
};

#endif /* IUnitStrategy_hpp */
