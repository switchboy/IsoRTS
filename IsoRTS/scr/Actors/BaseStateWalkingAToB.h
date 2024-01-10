#pragma once
#include "stateBase.h"

class BaseStateWalkingAToB : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};