#include "GroundStateReturningTheResource.h"
#include "actor.h"
#include "../gamestate.h"
#include <iostream>

bool GroundStateReturningTheResource::doAction(Actor* actor) {

    switch (actor->_subState->_sub) {
    case SubStateNames::NONE:
        //Find nearest reachable drop off point and set its cords as goal;
        actor->switchSubState(SubStateNames::SearchingDropOffPoint);
        return false;
    case SubStateNames::SearchingDropOffPoint:
        actor->_timeLastUpdate = currentGame.elapsedTimeMS;
        actor->switchSubState(SubStateNames::WalkingToNextSquare);
        return false;
    case SubStateNames::WalkingToNextSquare:
        actor->switchSubState(SubStateNames::DroppingOffResource);
        return false;
    case SubStateNames::DroppingOffResource:
        return true;

    default:
        actor->switchSubState(SubStateNames::SearchingDropOffPoint);
        return false;
    }
}