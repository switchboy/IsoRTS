#include "SubStateRangedAttacking.h"
#include "actor.h"
#include "ActorHelper.h"
#include "../objects.h"
#include "../buildings.h"
#include "../projectile.h"
#include <iostream>

bool SubStateRangedAttacking::doAction(Actor* actor) {
	const int targetId = actor->_baseState->getActionPreformedOn().x;
	const targetTypes targetType = static_cast<targetTypes>(actor->_baseState->getActionPreformedOn().y);
	switch (targetType) {
	case targetTypes::actor:
		if (!actorHelper::isNextToTarget(listOfActors[targetId].getActorCords(), actor->_actorCords, actor->_range)) {
			return true;
		}
		if (actor->_timeLastUpdate + actor->_rateOfFire < currentGame.getTime() && checkAndMoveActorToRightOrientation(actor, listOfActors[targetId].getActorCords())) {
			fireProjectile(actor, listOfActors[targetId].getActorCords());
			actor->_timeLastUpdate = currentGame.getTime();
		}
		return false;
	case targetTypes::building:
		if (!actorHelper::isNextToTarget(listOfBuildings[targetId].getLocation(), actor->_actorCords, actor->_range)) {
			return true;
		}
		if (actor->_timeLastUpdate + actor->_rateOfFire < currentGame.getTime() && checkAndMoveActorToRightOrientation(actor, listOfBuildings[targetId].getLocation())) {
			fireProjectile(actor, listOfBuildings[targetId].getLocation());
			actor->_timeLastUpdate = currentGame.getTime();
		}
		break;
	case targetTypes::object:
		if (!actorHelper::isNextToTarget(listOfObjects[targetId].getLocation(), actor->_actorCords, actor->_range)) {
			return true;
		}
		if (actor->_timeLastUpdate + actor->_rateOfFire < currentGame.getTime() && checkAndMoveActorToRightOrientation(actor, listOfObjects[targetId].getLocation())) {
			fireProjectile(actor, listOfObjects[targetId].getLocation());
			actor->_timeLastUpdate = currentGame.getTime();
		}
		return false;
	case targetTypes::groundTile:
		if (!actorHelper::isNextToTarget({ targetId % MAP_HEIGHT, targetId / MAP_HEIGHT }, actor->_actorCords, actor->_range)) {
			return true;
		}
		if (actor->_timeLastUpdate + actor->_rateOfFire < currentGame.getTime() && checkAndMoveActorToRightOrientation(actor, { targetId % MAP_HEIGHT, targetId / MAP_HEIGHT })) {
			fireProjectile(actor, { targetId % MAP_HEIGHT, targetId / MAP_HEIGHT });
			actor->_timeLastUpdate = currentGame.getTime();
		}
		return false;
	}
}


bool SubStateRangedAttacking::checkAndMoveActorToRightOrientation(Actor* actor,  cords const targetCords) {
	int desiredOrientation = actorHelper::actorOrientation(actor->_actorCords.x, actor->_actorCords.y, targetCords.x, targetCords.y);
	if (desiredOrientation != actor->_orientation) {
		actor->_orientation = actorHelper::newOrientation(actor->_orientation, desiredOrientation);
		return false;
	}
	return true;
}

void SubStateRangedAttacking::fireProjectile(Actor* actor, cords const targetCords) {
	listOfProjectiles.push_back(
		projectile(
			actor->_actorCords.x,
			actor->_actorCords.y,
			targetCords.x,
			targetCords.y,
			0,
			actor->_rangedDamage,
			actor->_splashDamage,
			actor->_actorId,
			targetTypes::actor
		)
	);
}