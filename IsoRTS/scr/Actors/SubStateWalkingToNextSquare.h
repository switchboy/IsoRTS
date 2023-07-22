#pragma once
#include "stateBase.h"

class SubStateWalkingToNextSquare : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};