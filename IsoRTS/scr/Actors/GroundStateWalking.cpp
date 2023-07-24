#include "GroundStateWalking.h"
#include "actor.h"

#include <iostream>

bool GroundStateWalking::doAction(Actor* actor) {
    // Implementation for GroundStateWalking state

    if (actor->_subState == nullptr) {
        actor->switchSubState(SubStateNames::SearchingAPath);
        return false;
    }

    switch (actor->_subState->_sub) {
    case SubStateNames::NONE:
        actor->switchSubState(SubStateNames::SearchingAPath);
        return false;
    case SubStateNames::SearchingAPath:
        actor->switchSubState(SubStateNames::WalkingToNextSquare);
        return false;
    case SubStateNames::WalkingToNextSquare:
        return true;
    }

    return false;
}