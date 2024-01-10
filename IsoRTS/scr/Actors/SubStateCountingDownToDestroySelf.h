#pragma once
#include "stateBase.h"

class SubStateCountingDownToDestroySelf : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};