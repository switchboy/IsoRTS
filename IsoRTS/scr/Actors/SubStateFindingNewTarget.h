#pragma once
#include "stateBase.h"

class SubStateFindingNewTarget : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};