#pragma once
#include "stateBase.h"

class SubStateGatheringTheResource : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};