#include "BaseStateWalkingAToB.h"
#include "actor.h"

#include <iostream>

bool BaseStateWalkingAToB::doAction(Actor* actor) {
    // Implementation for BaseStateWalkingAToB state

    if (actor->_groundState == nullptr) {
        actor->switchGroundState(GroundStateNames::Walking);
        actor->_groundState->doAction(actor);
    }
    switch (actor->_groundState->_ground) {
    case GroundStateNames::NONE:
        actor->switchGroundState(GroundStateNames::Walking);
        actor->_groundState->doAction(actor);
        break;
    case GroundStateNames::Walking:
        return true;
    }
    return false;
}