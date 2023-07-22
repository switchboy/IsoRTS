#pragma once
#include "stateBase.h"

class SubStateSettingMyDecompositionState : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};