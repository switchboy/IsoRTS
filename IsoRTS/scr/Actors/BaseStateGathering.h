#pragma once
#include "stateBase.h"

class BaseStateGathering : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};