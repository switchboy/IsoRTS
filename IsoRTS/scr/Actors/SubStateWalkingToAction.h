#pragma once
#include "stateBase.h"

class SubStateWalkingToAction : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};