#include "GroundStateAttacking.h"
#include "actor.h"
#include "../gamestate.h"
#include "../buildings.h"
#include "../objects.h"
#include <iostream>

bool GroundStateAttacking::doAction(Actor* actor) {
    if (actor->_subState == nullptr) {
        actor->switchSubState(SubStateNames::SearchingAPath);
        actor->_subState->doAction(actor);
        return false;
    }

    int targetId = actor->_baseState->getActionPreformedOn().x;
    targetTypes targetType = static_cast<targetTypes>(actor->_baseState->getActionPreformedOn().y);
    bool targetAlive = false;

    switch (actor->_subState->_sub) {
    case SubStateNames::NONE:
        actor->switchSubState(SubStateNames::SearchingAPath);
        actor->_subState->doAction(actor);
        return false;
    case SubStateNames::SearchingAPath:
        std::cout << "Actor " << actor->_actorId << ": Found a path switch to walking! To get to rtraget \n";
        actor->_timeLastUpdate = currentGame.elapsedTimeMS;
        actor->switchSubState(SubStateNames::WalkingToNextSquare);
        actor->_subState->doAction(actor);
        return false;
    case SubStateNames::WalkingToNextSquare:
        std::cout << "Actor " << actor->_actorId << ": is in striking distance! \n";
        actor->_timeLastUpdate = currentGame.elapsedTimeMS;
        switch (actor->_baseState->getModeOfAttack()) {
        case ModesOfAttack::ranged:
            actor->switchSubState(SubStateNames::RangedAttacking);
            actor->_subState->doAction(actor);
            std::cout << "Actor " << actor->_actorId << ": is ranged attacking! \n";
            return false;
        case ModesOfAttack::melee:
            actor->switchSubState(SubStateNames::WalkingToAction);
            actor->_timeLastUpdate = currentGame.elapsedTimeMS;
            std::cout << "Actor " << actor->_actorId << ": getting close for melee attak! \n";
            return false;
        default:
            actor->switchSubState(SubStateNames::WalkingToAction);
            actor->_timeLastUpdate = currentGame.elapsedTimeMS;
            std::cout << "Actor " << actor->_actorId << ": getting close for melee attak! \n";
            return false;
        }
        return false;
    case SubStateNames::WalkingToAction:
        actor->switchSubState(SubStateNames::MeleeAttacking);
        actor->_subState->doAction(actor);
        std::cout << "Actor " << actor->_actorId << ": is melee attacking! \n";
        return false;
    case SubStateNames::MeleeAttacking: 
        actor->switchSubState(SubStateNames::WalkingBackFromAction);
        actor->_timeLastUpdate = currentGame.elapsedTimeMS;
        std::cout << "Actor " << actor->_actorId << ": done attacking getting to own square! \n";
        return false;
    case SubStateNames::RangedAttacking: case SubStateNames::WalkingBackFromAction:
        std::cout << "Actor " << actor->_actorId << ": target out of range or destroyed! \n";
        switch (targetType) {
        case targetTypes::actor:
            targetAlive = listOfActors[targetId].getIsAlive();
            if (listOfActors[targetId].getHasRoute()) {
                actor->_actorGoal = listOfActors[targetId].getGoal();
            }
            actor->_actorGoal = listOfActors[targetId].getActorCords();
            break;
        case targetTypes::building:
            targetAlive = listOfBuildings[targetId].getExists();
            actor->_actorGoal = listOfBuildings[targetId].getLocation();
            break;
        case targetTypes::object:
            targetAlive = listOfObjects[targetId].getIsInWorld();
            actor->_actorGoal = listOfObjects[targetId].getLocation();
            break;
        default:
            break;
        }
        if (targetAlive) {
            actor->switchSubState(SubStateNames::SearchingAPath);
            actor->_subState->doAction(actor);
            return false;
        }
        actor->switchSubState(SubStateNames::FindingNewTarget);
        return false;
    case SubStateNames::FindingNewTarget:
        return true;
    }
    return false;
}