#include "SubStateGatheringTheResource.h"
#include "actor.h"
#include "../objects.h"
#include "../gametext.h"
#include "ActorHelper.h"
#include <iostream>

bool SubStateGatheringTheResource::doAction(Actor* actor) {
    // Implementation for SubStateGatheringTheResource state
    if (!actorHelper::isReallyNextToResource(actor->_actorCords.x, actor->_actorCords.y, actor->_baseState->getActionPreformedOn().x, actor->_baseState->getActionPreformedOn().y) ||
        currentGame.objectLocationList[actor->_baseState->getActionPreformedOn().x][actor->_baseState->getActionPreformedOn().y] == -1) {
        //resource not here!
        std::stringstream error;
        error << "Resources no longer here! Going back to base.";
        gameText.addNewMessage(error.str(), 1);
        actor->_timeLastUpdate = currentGame.elapsedTimeMS;
        return true;
    }

    if (currentGame.elapsedTimeMS - actor->_timeLastUpdate > 2000)
    {
        switch (actor->_resourceBeingGatherd)
        {
        case resourceTypes::resourceWood:  //wood
            actor->_amountOfWood += +1;
            break;
        case resourceTypes::resourceFood: //food
            actor->_amountOfFood += +1;
            break;
        case resourceTypes::resourceStone: //stone
            actor->_amountOfStone += +1;
            break;
        case resourceTypes::resourceGold: // gold
            actor->_amountOfGold += +1;
            break;
        }
        listOfObjects[currentGame.objectLocationList[actor->_baseState->getActionPreformedOn().x][actor->_baseState->getActionPreformedOn().y]].substractResource();
        actor->_timeLastUpdate = currentGame.elapsedTimeMS;
    }

    if ((actor->_amountOfFood == 10) || (actor->_amountOfWood == 10) || (actor->_amountOfStone == 10) || (actor->_amountOfGold == 10)){
        return true;
    }
    return false;


}