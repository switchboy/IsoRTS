#include "SubStateDroppingOffResource.h"
#include "actor.h"
#include "../buildings.h"
#include "../player.h"
#include <iostream>

bool SubStateDroppingOffResource::doAction(Actor* actor) {
    switch (listOfBuildings[actor->_buildingId].getRecievesWhichResources())
    {
    case resourceTypes::resourceWood:
        //recieves only wood
        listOfPlayers[actor->_actorTeam].addResources(resourceTypes::resourceWood, actor->_amountOfWood);
        actor->_amountOfWood = 0;
        break;
    case resourceTypes::resourceFood:
        //recieves only food
        listOfPlayers[actor->_actorTeam].addResources(resourceTypes::resourceFood, actor->_amountOfFood);
        actor->_amountOfFood = 0;
        break;
    case resourceTypes::resourceStone:
        //recieves only stone
        listOfPlayers[actor->_actorTeam].addResources(resourceTypes::resourceStone, actor->_amountOfStone);
        actor->_amountOfStone = 0;
        break;
    case resourceTypes::resourceGold:
        //recieves only gold
        listOfPlayers[actor->_actorTeam].addResources(resourceTypes::resourceGold, actor->_amountOfGold);
        actor->_amountOfGold = 0;
        break;
    case resourceTypes::All:
        //recieves all the resources!
        listOfPlayers[actor->_actorTeam].addResources(resourceTypes::resourceWood, actor->_amountOfWood);
        actor->_amountOfWood = 0;
        listOfPlayers[actor->_actorTeam].addResources(resourceTypes::resourceFood, actor->_amountOfFood);
        actor->_amountOfFood = 0;
        listOfPlayers[actor->_actorTeam].addResources(resourceTypes::resourceStone, actor->_amountOfStone);
        actor->_amountOfStone = 0;
        listOfPlayers[actor->_actorTeam].addResources(resourceTypes::resourceGold, actor->_amountOfGold);
        actor->_amountOfGold = 0;
        break;
    }
    return true;
}