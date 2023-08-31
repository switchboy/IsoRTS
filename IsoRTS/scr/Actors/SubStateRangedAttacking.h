#pragma once
#include "stateBase.h"

class SubStateRangedAttacking : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
private:
    bool checkAndMoveActorToRightOrientation(Actor* actor, cords const targetCords);
    void fireProjectile(Actor* actor, cords const targetCords);
};