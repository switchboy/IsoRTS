#pragma once
#include "stateBase.h"

class StateCanceldWhileWalking : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};