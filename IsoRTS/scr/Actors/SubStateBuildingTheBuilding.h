#pragma once
#include "stateBase.h"

class SubStateBuildingTheBuilding : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};