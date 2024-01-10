#pragma once
#include "stateBase.h"

class GroundStateAttacking : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};