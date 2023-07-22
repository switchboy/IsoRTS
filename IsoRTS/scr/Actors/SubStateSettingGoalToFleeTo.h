#pragma once
#include "stateBase.h"

class SubStateSettingGoalToFleeTo : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};