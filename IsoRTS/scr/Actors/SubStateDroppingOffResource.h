#pragma once
#include "stateBase.h"

class SubStateDroppingOffResource : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};