#pragma once
#include "stateBase.h"
#include "../globalfunctions.h"

class BaseStateGathering : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
    cords getActionPreformedOn() override;
    void setActionPreformedOn(cords location) override;

private:
    cords _actionPreformedOn;
};