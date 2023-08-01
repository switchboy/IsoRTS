#include "SubStateBuildingTheBuilding.h"
#include "actor.h"
#include "../gamestate.h"
#include "../buildings.h"
#include <iostream>

bool SubStateBuildingTheBuilding::doAction(Actor* actor) {
    if (!checkIfBuildingIsThereAndIncomplete(actor)) {
        return true;
    }
    if (currentGame.elapsedTimeMS - actor->_timeLastUpdate > 1000)
    {
        listOfBuildings[currentGame.occupiedByBuildingList[actor->_baseState->getActionPreformedOn().x][actor->_baseState->getActionPreformedOn().y]].addBuildingPoint();
        actor->_timeLastUpdate = currentGame.elapsedTimeMS;
    }
    return false;
}

bool SubStateBuildingTheBuilding::checkIfBuildingIsThereAndIncomplete(Actor* actor)
{
    if (currentGame.occupiedByBuildingList[actor->_baseState->getActionPreformedOn().x][actor->_baseState->getActionPreformedOn().y] == -1) {
        return false;
    }
    if (listOfBuildings[actor->_buildingId].getCompleted() ||
        listOfBuildings[actor->_buildingId].getTeam() != actor->_actorTeam) {
        return false;
    }
    return true;
}