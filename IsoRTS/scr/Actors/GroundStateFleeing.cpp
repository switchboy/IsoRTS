#include "GroundStateFleeing.h"
#include "actor.h"
#include "../buildings.h"
#include "../objects.h"
#include "../gamestate.h"
#include <iostream>
#include <array>

bool agressorIsDeadOrFarAway(const int& agressorId, const targetTypes& agressorType, const cords& actorCords) {
    
    if (!listOfActors[agressorId].getIsAlive()) {
        return true;
    }
    
    int distanceFromWhereToIgnorePersuit = 10;
    int distX = abs(listOfActors[agressorId].getActorCords().x - actorCords.x);
    int distY = abs(listOfActors[agressorId].getActorCords().y - actorCords.y);

    if (distX >= distanceFromWhereToIgnorePersuit || distY >= distanceFromWhereToIgnorePersuit) {
        return true;
    }

    return false;
}

bool GroundStateFleeing::doAction(Actor* actor) {
    
    if (agressorIsDeadOrFarAway(actor->_groundState->getActionPreformedOn().x, static_cast<targetTypes>(actor->_groundState->getActionPreformedOn().y), actor->_actorCords)) {
        return true;
    }

    switch (actor->_subState->_sub) {
    case SubStateNames::NONE:
       actor->switchSubState(SubStateNames::SettingGoalToFleeTo);
       return false;
    case SubStateNames::SettingGoalToFleeTo:
        actor->switchSubState(SubStateNames::SearchingAPath);
        return false;
    case SubStateNames::SearchingAPath:
        std::cout << "Actor " << actor->_actorId << ": Found a path Walking away! \n";
        actor->_timeLastUpdate = currentGame.elapsedTimeMS;
        actor->switchSubState(SubStateNames::WalkingToNextSquare);
        actor->_subState->doAction(actor);
        return false;
    case SubStateNames::WalkingToNextSquare:
        actor->switchSubState(SubStateNames::SettingGoalToFleeTo);
        return false;
    }
}


