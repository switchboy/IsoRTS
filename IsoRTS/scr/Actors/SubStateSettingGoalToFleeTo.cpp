#include "SubStateSettingGoalToFleeTo.h"
#include "actor.h"
#include "../buildings.h"
#include "../objects.h"
#include "../gamestate.h"
#include <iostream>

cords fleeToPoint(const int& agressorId, const targetTypes& agressorType, const cords& actorCords) {
    cords agressorCords = getAgressorCords(agressorId, agressorType);

    /*
    Consider all cells around NPC
    /   0   1   2   x

    0   0   1   3

    1   4   5   6

    2   7   8   9

    y

    */

    //Now we need to know on which cell the enemy is located (assuming our actor is on {1,1} cell 5
    //For example NPC  {3,3} agressor {3,4} this yields: 3-3+1 = 1 and 5-4+1 = 2. So agrossers adjusted cords are {1,2}.  Which will give us cell 5 NPC and cell 8 for the agressor!
    cords adjustedAgressorCords = { (actorCords.x-agressorCords.x) + 1, (actorCords.y-agressorCords.y) + 1 };

    std::cout << "NPC x: " << actorCords.x << " y: " << actorCords.y << " agressor x: " << agressorCords.x << " y: " << agressorCords.y << "x: " << adjustedAgressorCords.x << " y: " << adjustedAgressorCords.y << " \n";;

    std::vector<cords> safeSpaces;

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            int distanceX = abs(x - adjustedAgressorCords.x );
            int distanceY = abs(y - adjustedAgressorCords.y );

            std::cout << "x: " << adjustedAgressorCords.x << " y: " << adjustedAgressorCords.y << " dist: (" << distanceX << ' ' << distanceY << ") \n";

            if (distanceX >= 2 && distanceY >= 2 && currentGame.isPassable({ (adjustedAgressorCords.x - 1) + actorCords.x, (adjustedAgressorCords.y - 1) + actorCords.y })) {
                std::cout << "Safe space found & added to vector! \n";
                safeSpaces.push_back({ (adjustedAgressorCords.x - 1) + actorCords.x, (adjustedAgressorCords.y - 1) + actorCords.y });
            }
        }
    }

    //In the example case cell 7, 4, 0 , 1, 2 should be viable targets, for now we choose one at random in the future we might make the NPC prefer to run towards the nearest friendly
    if (safeSpaces.size() == 0) {
        std::cout << "No safe space, till the death then! \n";
        return { -1,-1 }; //No solution!
    }

    //For now we can return a random set of cords from the vector; We should note that we take care to get the exactly the same 'pseudo randomness' on all network clients to do this we use the agressor id * currentGameTick as seed
    int randomSeed = agressorId * currentGame.nextCommandWindow;
    std::default_random_engine rng(randomSeed);
    std::shuffle(std::begin(safeSpaces), std::end(safeSpaces), rng);

    return safeSpaces[0];

}

bool SubStateSettingGoalToFleeTo::doAction(Actor* actor) {
    std::cout << "Actor " << actor->_actorId << ": Evaluating flight options! \n";
    cords safeSpace = fleeToPoint(actor->_groundState->getActionPreformedOn().x, static_cast<targetTypes>(actor->_groundState->getActionPreformedOn().y), actor->_actorCords);
    if (safeSpace.x == -1 && safeSpace.y == -1) {
        //Nowhere to run; fight till the death!
        actor->switchGroundState(GroundStateNames::Attacking);
        return true;
    }
    actor->_actorGoal = safeSpace;
    actor->_actorRealGoal = actor->_actorGoal;
    return true;
}


cords getAgressorCords(const int& agressorId, const targetTypes & agressorType) {
    switch (agressorType)
    {
    case targetTypes::actor:
        if (agressorId < listOfActors.size()) {
            return listOfActors[agressorId].getActorCords();
        }
        return { -1, -1 };
    case targetTypes::building:
        if (agressorId < listOfBuildings.size()) {
            return listOfBuildings[agressorId].getLocation();
        }
        return { -1, -1 };
    case targetTypes::object:
        if (agressorId < listOfObjects.size()) {
            return listOfObjects[agressorId].getLocation();
        }
        return { -1,-1 };
    case targetTypes::groundTile:
        return  { agressorId % MAP_HEIGHT, agressorId / MAP_HEIGHT };
    }
}