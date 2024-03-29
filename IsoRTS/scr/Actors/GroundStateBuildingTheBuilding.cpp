#include "GroundStateBuildingTheBuilding.h"
#include "actor.h"
#include "../buildings.h"
#include "ActorHelper.h"
#include <iostream>

bool GroundStateBuildingTheBuilding::doAction(Actor* actor) {

	switch (actor->_subState->_sub) {
	case SubStateNames::NONE:
        if (!checkIfBuildingIsThereAndIncomplete(actor)) {
            listOfBuildings[actor->_buildingId].removeActorFromBuildingTile(actor->_actorId);
            actor->switchGroundState(GroundStateNames::SearchAlternativeBuildingSpot);
            actor->_groundState->doAction(actor);
            return false;
        }
        if (actor->_orientation != actorHelper::actorOrientation(actor->_actorCords.x, actor->_actorCords.y, actor->_baseState->getActionPreformedOn().x, actor->_baseState->getActionPreformedOn().y)) {
            actor->_orientation = actorHelper::newOrientation(actor->_orientation, actorHelper::actorOrientation(actor->_actorCords.x, actor->_actorCords.y, actor->_baseState->getActionPreformedOn().x, actor->_baseState->getActionPreformedOn().y));
            return false;
        }
        actor->switchSubState(SubStateNames::WalkingToAction);
        actor->_subState->doAction(actor);
        actor->_timeLastUpdate = currentGame.elapsedTimeMS;
        return false;
    case SubStateNames::WalkingToAction:
        actor->switchSubState(SubStateNames::BuildingTheBuilding);
        actor->_subState->doAction(actor);
        return false;
    case SubStateNames::BuildingTheBuilding:
        actor->switchSubState(SubStateNames::WalkingBackFromAction);
        actor->_subState->doAction(actor);
        return false;
    case SubStateNames::WalkingBackFromAction:
        listOfBuildings[actor->_buildingId].removeActorFromBuildingTile(actor->_actorId);
        return true;
	}

}

bool GroundStateBuildingTheBuilding::checkIfBuildingIsThereAndIncomplete(Actor* actor)
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
