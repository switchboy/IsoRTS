#include "GroundStateSearchAlternativeBuildingSpot.h"
#include "actor.h"

#include <iostream>

bool GroundStateSearchAlternativeBuildingSpot::doAction(Actor* actor) {

	switch (actor->_subState->_sub) {
	case SubStateNames::NONE:
		actor->switchSubState(SubStateNames::SearchingAlternativeBuildingToBuilt);
		return false;
	case SubStateNames::SearchingAlternativeBuildingToBuilt:
		return true;
	}

}