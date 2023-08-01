#include "SubStateSearchingAlternativeBuildingToBuilt.h"
#include "actor.h"
#include "../buildings.h"
#include <iostream>
#include "../cells.h"

bool SubStateSearchingAlternativeBuildingToBuilt::doAction(Actor* actor) {

	struct buildingDetails {
		int buildingId;
		cords buildingLocation;
		int distance;
	};

	std::vector<buildingDetails> vectorOfBuildings;

	for (int i = 0; i < listOfBuildings.size(); i++)
	{
		if (!listOfBuildings[i].getCompleted() && listOfBuildings[i].getTeam() == actor->_actorTeam && listOfBuildings[i].getExists()) {
			buildingDetails tempLocation = { i, listOfBuildings[i].getLocation(), dist(actor->_actorCords.x, actor->_actorCords.y, listOfBuildings[i].getLocation().x, listOfBuildings[i].getLocation().y) };
			vectorOfBuildings.push_back(tempLocation);
		}
	}

	if (vectorOfBuildings.empty()) {
		actor->switchBaseState(BaseStateNames::Idle);
	}

	std::sort(vectorOfBuildings.begin(), vectorOfBuildings.end(),[](const buildingDetails& lhs, const buildingDetails& rhs) {return lhs.distance > rhs.distance;});
	bool newBuildingFound = false;
	while (!newBuildingFound && !vectorOfBuildings.empty()) {
		if (canReachTarget(actor->_actorCords, vectorOfBuildings.back().buildingLocation, false)) {
			nearestBuildingTile tempTile = findNearestBuildingTile(currentGame.occupiedByBuildingList[vectorOfBuildings.back().buildingLocation.x][vectorOfBuildings.back().buildingLocation.y], actor->_actorId);
			listOfBuildings[actor->getBuildingId()].removeActorFromBuildingTile(actor->_actorId);
			listOfBuildings[vectorOfBuildings.back().buildingId].claimFreeBuiildingTile(tempTile.buildingId, actor->_actorId);
			actor->_baseState->setActionPreformedOn(vectorOfBuildings.back().buildingLocation);
			actor->_actorGoal = tempTile.location;
			actor->_buildingId = vectorOfBuildings.back().buildingId;
			actor->switchGroundState(GroundStateNames::Walking);
			return true;
		}
		vectorOfBuildings.pop_back();
	}




}