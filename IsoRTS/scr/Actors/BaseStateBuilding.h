#pragma once
#include "stateBase.h"

class BaseStateBuilding : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};