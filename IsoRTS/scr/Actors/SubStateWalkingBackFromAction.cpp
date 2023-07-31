#include "SubStateWalkingBackFromAction.h"
#include "actor.h"
#include "../gamestate.h"
#include <iostream>

bool SubStateWalkingBackFromAction::doAction(Actor* actor) {
    if (currentGame.elapsedTimeMS - actor->_timeLastUpdate > 500) {
        return true;
    }
    return false;
}