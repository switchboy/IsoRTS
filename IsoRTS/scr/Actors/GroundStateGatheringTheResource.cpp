#include "GroundStateGatheringTheResource.h"
#include "actor.h"
#include "../gamestate.h"
#include <iostream>

bool GroundStateGatheringTheResource::doAction(Actor* actor) {
    // Implementation for GroundStateGatheringTheResource state
    switch (actor->_subState->_sub) {
    case SubStateNames::NONE:
        actor->switchSubState(SubStateNames::WalkingToAction);
        actor->_timeLastUpdate = currentGame.elapsedTimeMS;
        actor->_subState->doAction(actor);
        return false;
    case SubStateNames::WalkingToAction:
        actor->switchSubState(SubStateNames::GatheringTheResource);
        actor->_subState->doAction(actor);
        return false;
    case SubStateNames::GatheringTheResource:
        actor->switchSubState(SubStateNames::WalkingBackFromAction);
        actor->_subState->doAction(actor);
        return false;
    case SubStateNames::WalkingBackFromAction:
        return true;
    default:
        return false;
    }
}