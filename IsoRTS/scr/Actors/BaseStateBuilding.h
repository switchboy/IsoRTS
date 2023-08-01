#pragma once
#include "stateBase.h"

class BaseStateBuilding : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
    cords getActionPreformedOn() override;
    void setActionPreformedOn(cords location) override;
private:
    bool initiateBuilding(Actor* actor);
    cords _originalCords;
};