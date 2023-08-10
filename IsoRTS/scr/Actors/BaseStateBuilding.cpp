#include "BaseStateBuilding.h"
#include "actor.h"
#include "../buildings.h"
#include "../gamestate.h"
#include "ActorHelper.h"
#include <iostream>

bool BaseStateBuilding::doAction(Actor* actor) {
    switch (actor->_groundState->_ground) {
    case GroundStateNames::NONE:
        if (initiateBuilding(actor)) {
            actor->switchGroundState(GroundStateNames::Walking);
            actor->_groundState->doAction(actor);
            return false;
        }
        actor->switchGroundState(GroundStateNames::SearchAlternativeBuildingSpot);
        actor->_groundState->doAction(actor);
        return false;
    case GroundStateNames::Walking:
        actor->switchGroundState(GroundStateNames::AtTheBuilding);
        actor->_groundState->doAction(actor);
        return false;
    case GroundStateNames::AtTheBuilding:
        actor->switchGroundState(GroundStateNames::SearchAlternativeBuildingSpot);
        actor->_groundState->doAction(actor);
        return false;
    case GroundStateNames::SearchAlternativeBuildingSpot:
        actor->switchGroundState(GroundStateNames::Walking);
        actor->_groundState->doAction(actor);
        return false;
   }
}

cords BaseStateBuilding::getActionPreformedOn()
{
    return _originalCords;
}

void BaseStateBuilding::setActionPreformedOn(cords location)
{
    _originalCords = location;
}

bool BaseStateBuilding::initiateBuilding(Actor* actor)
{
    if (currentGame.occupiedByBuildingList[actor->_actorGoal.x][actor->_actorGoal.y] == -1) {
        return false;
    }
    if (listOfBuildings[currentGame.occupiedByBuildingList[actor->_actorGoal.x][actor->_actorGoal.y]].getCompleted() ||
        listOfBuildings[currentGame.occupiedByBuildingList[actor->_actorGoal.x][actor->_actorGoal.y]].getTeam() != actor->_actorTeam) {
        return false;
    }
    nearestBuildingTile tempTile = findNearestBuildingTile(currentGame.occupiedByBuildingList[actor->_actorGoal.x][actor->_actorGoal.y], actor->_actorId);
    listOfBuildings[currentGame.occupiedByBuildingList[actor->_actorGoal.x][actor->_actorGoal.y]].claimFreeBuiildingTile(tempTile.tileId, actor->_actorId);
    _originalCords = actor->_actorGoal;
    actor->_buildingId = currentGame.occupiedByBuildingList[actor->_actorGoal.x][actor->_actorGoal.y];
    actor->_actorGoal = tempTile.location;
    
    return true;
}
