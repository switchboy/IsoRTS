#pragma once
#include "stateBase.h"
#include "actorStructs.h"

enum class direction {
    upLeft,
    up,
    upRight,
    left,
    right,
    downLeft,
    down,
    downRight,
};

class SubStateSettingGoalToFleeTo : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};

cords getAgressorCords(const int& agressorId, const targetTypes& agressorType);