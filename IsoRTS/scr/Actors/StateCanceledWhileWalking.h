#pragma once
#include "stateBase.h"

class StateCanceledWhileWalking : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
    bool getMoved() override;

private:
    bool _moved = false;;
    bool _walkBackSet = false;
};