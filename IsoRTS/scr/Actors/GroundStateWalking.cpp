#include "GroundStateWalking.h"
#include "actor.h"
#include "../gamestate.h"

#include <iostream>

bool GroundStateWalking::doAction(Actor* actor) {
    // Implementation for GroundStateWalking state

    if (actor->_subState == nullptr) {
        actor->switchSubState(SubStateNames::SearchingAPath);
        actor->_subState->doAction(actor);
        return false;
    }

    switch (actor->_subState->_sub) {
    case SubStateNames::NONE:
        actor->switchSubState(SubStateNames::SearchingAPath);
        actor->_subState->doAction(actor);
        return false;
    case SubStateNames::SearchingAPath:
        std::cout << "Actor " << actor->_actorId << ": Found a path switch to walking! \n";
        actor->_timeLastUpdate = currentGame.elapsedTimeMS;
        actor->switchSubState(SubStateNames::WalkingToNextSquare);
        actor->_subState->doAction(actor);
        return false;
    case SubStateNames::WalkingToNextSquare:
        std::cout << "Actor " << actor->_actorId << ": Done walking! \n";
        return true;
    }

    return false;
}