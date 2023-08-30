#include "SubStateMeleeAttacking.h"
#include "actor.h"
#include "../objects.h"
#include "ActorHelper.h"
#include <iostream>



bool SubStateMeleeAttacking::doAction(Actor* actor) {
	const int targetId = actor->_baseState->getActionPreformedOn().x;
	const targetTypes targetType = static_cast<targetTypes>(actor->_baseState->getActionPreformedOn().y);
	
	switch (targetType) {
		case targetTypes::actor:
			if (!actorHelper::isNextToTarget(listOfActors[targetId].getActorCords(), actor->_actorCords, 1)) {
				return true;
			}
			if (actor->_timeLastUpdate + actor->_rateOfFire < currentGame.getTime()) {
				listOfActors[targetId].takeDamage(actor->_meleeDamage, actor->_actorId, targetTypes::actor);
				actor->_timeLastUpdate = currentGame.getTime();
			}
			return false;
		case targetTypes::building:

			break;
		case targetTypes::object:
			if (!actorHelper::isNextToTarget(listOfObjects[targetId].getLocation(), actor->_actorCords, 1)) {
				return true;
			}
			if (actor->_timeLastUpdate + actor->_rateOfFire < currentGame.getTime()) {
				listOfObjects[targetId].takeDamage(actor->_meleeDamage, actor->_actorId, targetTypes::actor);
				actor->_timeLastUpdate = currentGame.getTime();
			}
			return false;
		case targetTypes::groundTile:
			break;
	}

	




}