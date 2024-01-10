#include "SubStateBuildingTheBuilding.h"
#include "actor.h"
#include "ActorHelper.h"
#include "../gamestate.h"
#include "../buildings.h"
#include <iostream>

bool SubStateBuildingTheBuilding::doAction(Actor* actor) {
    if (!actorHelper::checkIfBuildingIsThereAndIncomplete(actor->_baseState->getActionPreformedOn(), actor->_buildingId, actor->_actorTeam)) {
        return true;
    }
    if (currentGame.elapsedTimeMS - actor->_timeLastUpdate > 1000)
    {
        listOfBuildings[actor->_buildingId].addBuildingPoint();
        actor->_timeLastUpdate = currentGame.elapsedTimeMS;
    }
    return false;
}

