#pragma once
#include "stateBase.h"

class GroundStateDecomposing : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};