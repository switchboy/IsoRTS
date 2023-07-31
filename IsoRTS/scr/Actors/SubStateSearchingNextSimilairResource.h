#pragma once
#include "stateBase.h"

class SubStateSearchingNextSimilairResource : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;

private: 
    int _tries = 0;
};