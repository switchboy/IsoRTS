#pragma once
#include "stateBase.h"

class SubStateSearchingAPath : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;

private:
    int _tries = 0;
};