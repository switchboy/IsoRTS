#pragma once
#include "stateBase.h"

class BaseStateDead : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};