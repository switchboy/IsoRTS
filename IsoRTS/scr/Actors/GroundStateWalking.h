#pragma once
#include "stateBase.h"

class GroundStateWalking : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};