#pragma once
#include "stateBase.h"

class SubStateSearchNextTarget : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};