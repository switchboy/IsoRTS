#pragma once
#include "stateBase.h"

class GroundStateReturningTheResource : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};