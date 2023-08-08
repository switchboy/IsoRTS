#include "GroundStateSearchAlternativeBuildingSpot.h"
#include "actor.h"

#include <iostream>

bool GroundStateSearchAlternativeBuildingSpot::doAction(Actor* actor) {

	switch (actor->_subState->_sub) {
	case SubStateNames::NONE:
		//TODO:
		//check if building to be build exist and needs building
		//Try to find reachable slot
		//Else find other building to build

		actor->switchSubState(SubStateNames::SearchingAlternativeBuildingToBuilt);
		actor->_subState->doAction(actor);
		return false;
	case SubStateNames::SearchingAlternativeBuildingToBuilt:
		return true;
	}

}