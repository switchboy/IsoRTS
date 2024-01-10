#pragma once
#include "stateBase.h"

class SubStateSearchingAlternativeBuildingSpot : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};