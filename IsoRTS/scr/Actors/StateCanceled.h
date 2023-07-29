#pragma once
#include "stateBase.h"

class StateCanceled : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};