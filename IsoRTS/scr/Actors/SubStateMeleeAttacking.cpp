#include "SubStateMeleeAttacking.h"
#include "actor.h"
#include "../objects.h"
#include "../buildings.h"
#include "ActorHelper.h"
#include <iostream>



bool SubStateMeleeAttacking::doAction(Actor* actor) {
	const int targetId = actor->_baseState->getActionPreformedOn().x;
	const targetTypes targetType = static_cast<targetTypes>(actor->_baseState->getActionPreformedOn().y);
	switch (targetType) {
		case targetTypes::actor:
			if (!actorHelper::isNextToTarget(listOfActors[targetId].getActorCords(), actor->_actorCords, 1)) {
				std::cout << "Actor " << actor->_actorId << ": Not next to target! \n";
				return true;
			}
			if (listOfActors[targetId].getIsAlive()) {
				if (actor->_timeLastUpdate + actor->_rateOfFire < currentGame.getTime() && checkAndMoveActorToRightOrientation(actor, listOfActors[targetId].getActorCords())) {
					listOfActors[targetId].takeDamage(actor->_meleeDamage, actor->_actorId, targetTypes::actor);
					actor->_timeLastUpdate = currentGame.getTime();
				}
				return false;
			}
			std::cout << "Actor " << actor->_actorId << ": Target is dead! \n";
			return true;
		case targetTypes::building:
			if (!actorHelper::isNextToTarget(listOfBuildings[targetId].getLocation(), actor->_actorCords, 1)) {
				std::cout << "Actor " << actor->_actorId << ": Not next to target! \n";
				return true;
			}
			if (actor->_timeLastUpdate + actor->_rateOfFire < currentGame.getTime() && checkAndMoveActorToRightOrientation(actor, listOfBuildings[targetId].getLocation())) {
				listOfActors[targetId].takeDamage(actor->_meleeDamage, actor->_actorId, targetTypes::actor);
				actor->_timeLastUpdate = currentGame.getTime();
			}
			break;
		case targetTypes::object:
			if (!actorHelper::isNextToTarget(listOfObjects[targetId].getLocation(), actor->_actorCords, 1)) {
				std::cout << "Actor " << actor->_actorId << ": Not next to target! \n";
				return true;
			}
			if (actor->_timeLastUpdate + actor->_rateOfFire < currentGame.getTime() && checkAndMoveActorToRightOrientation(actor, listOfObjects[targetId].getLocation())) {
				listOfObjects[targetId].takeDamage(actor->_meleeDamage, actor->_actorId, targetTypes::actor);
				actor->_timeLastUpdate = currentGame.getTime();
			}
			return false;
		case targetTypes::groundTile:
			if (!actorHelper::isNextToTarget({ targetId % MAP_HEIGHT, targetId / MAP_HEIGHT }, actor->_actorCords, actor->_range)) {
				std::cout << "Actor " << actor->_actorId << ": Not next to target! \n";
				return true;
			}
			if (actor->_timeLastUpdate + actor->_rateOfFire < currentGame.getTime() && checkAndMoveActorToRightOrientation(actor, { targetId % MAP_HEIGHT, targetId / MAP_HEIGHT })) {
				//Nothing to be damaged right now, if one day splash damage from a Melee attack is required it can be triggerd here!
				actor->_timeLastUpdate = currentGame.getTime();
			}
			return false;
	}
}

bool SubStateMeleeAttacking::checkAndMoveActorToRightOrientation(Actor* actor,  cords const targetCords) {
	int desiredOrientation = actorHelper::actorOrientation(actor->_actorCords.x, actor->_actorCords.y, targetCords.x, targetCords.y);
	if (desiredOrientation != actor->_orientation) {
		actor->_orientation = actorHelper::newOrientation(actor->_orientation, desiredOrientation);
		return false;
	}
	return true;
}