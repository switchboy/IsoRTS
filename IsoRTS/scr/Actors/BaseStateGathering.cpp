#include "BaseStateGathering.h"
#include "actor.h"
#include "../gamestate.h"
#include "../objects.h"
#include <iostream>

bool BaseStateGathering::doAction(Actor* actor) {
    if (actor->_groundState == nullptr) {
        actor->switchGroundState(GroundStateNames::Walking);
    }
    switch (actor->_groundState->_ground) {
    case GroundStateNames::NONE:
        if (currentGame.objectLocationList[actor->_actorGoal.x][actor->_actorGoal.y] != -1){
            actor->_resourceBeingGatherd = listOfObjects[currentGame.objectLocationList[actor->_actorGoal.x][actor->_actorGoal.y]].getTypeOfResource();
            _actionPreformedOn = actor->_actorGoal;
        }
        actor->switchGroundState(GroundStateNames::Walking);
        actor->_groundState->doAction(actor);
        break;
    case GroundStateNames::Walking:
        actor->switchGroundState(GroundStateNames::AtTheResource);
        actor->_groundState->doAction(actor);
        break;
    case GroundStateNames::AtTheResource:
        actor->switchGroundState(GroundStateNames::ReturningTheResource);
        actor->_groundState->doAction(actor);
        break;
    case GroundStateNames::ReturningTheResource:
        actor->_actorGoal = _actionPreformedOn;
        if (currentGame.objectLocationList[actor->_actorGoal.x][actor->_actorGoal.y] == -1) {
            actor->switchGroundState(GroundStateNames::FindAlternativeSource);
            actor->_groundState->doAction(actor);
        }
        else {
            actor->switchGroundState(GroundStateNames::Walking);
            actor->_groundState->doAction(actor);
        }
        break;
    case GroundStateNames::FindAlternativeSource:
        actor->switchGroundState(GroundStateNames::Walking);
        actor->switchSubState(SubStateNames::WalkingToNextSquare);
        actor->_subState->doAction(actor);
        break;
    }
    return false;
}

cords BaseStateGathering::getActionPreformedOn()
{
    return _actionPreformedOn;
}

void BaseStateGathering::setActionPreformedOn(cords location)
{
    _actionPreformedOn = location;
}