#pragma once
#include "stateBase.h"

class GroundStateFindAlternativeSource : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};