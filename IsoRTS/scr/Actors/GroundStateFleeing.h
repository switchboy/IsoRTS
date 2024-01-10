#pragma once
#include "stateBase.h"


class GroundStateFleeing : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};

