#pragma once
#include "stateBase.h"

class SubStateWalkingToNextSquare : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
    bool getMoved() override;

private:
    bool walkToNextRoutePoint(Actor* actor);
    bool retryWalkingOrChangeGoal(Actor* actor);
    bool _lastCellBlocked = false;
    int _timeLastRetry = 0;
    int _retries = 0;
    bool _moved = false;
};