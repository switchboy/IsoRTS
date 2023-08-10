#include "GroundStateSearchAlternativeBuildingSpot.h"
#include "actor.h"

#include <iostream>
#include "../gamestate.h"
#include "ActorHelper.h"

bool GroundStateSearchAlternativeBuildingSpot::doAction(Actor* actor) {

	switch (actor->_subState->_sub) {
	case SubStateNames::NONE:		
		if (actorHelper::checkIfBuildingIsThereAndIncomplete(actor->_baseState->getActionPreformedOn(), actor->_buildingId, actor->_actorTeam) ){
			std::cout << "Actor " << actor->_actorId << ": Building still there and incomplete requesting different spot! \n";
				actor->switchSubState(SubStateNames::SearchingAlternativeBuildingSpot);
				return false;
		}
		std::cout << "Actor " << actor->_actorId << ": Nothing to build here searching alternative! \n";
		actor->switchSubState(SubStateNames::SearchingAlternativeBuildingToBuilt);
		return false;
	case SubStateNames::SearchingAlternativeBuildingToBuilt:
		return true;
	case SubStateNames::SearchingAlternativeBuildingSpot:
		std::cout << "Actor " << actor->_actorId << ": Cant reach a free building spot searching alternative! \n";
		actor->switchSubState(SubStateNames::SearchingAlternativeBuildingToBuilt);
		return false;
	}

}