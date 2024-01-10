#pragma once
#include "stateBase.h"

class GroundStateBuildingTheBuilding : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
private: 
    bool checkIfBuildingIsThereAndIncomplete(Actor* actor);
};